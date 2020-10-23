/*
Copyright: Boaz Segev, 2016-2017
License: MIT

Feel free to copy, use and enjoy according to the license provided.
*/
#include "spnlock.inc"

#include "evio.h"
#include "facil.h"
#include "fio_hashmap.h"
#include "fiobj4sock.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* *****************************************************************************
Data Structures
***************************************************************************** */
typedef struct ProtocolMetadata {
  spn_lock_i locks[3];
  unsigned rsv : 8;
} protocol_metadata_s;

union protocol_metadata_union_u {
  size_t opaque;
  protocol_metadata_s meta;
};
#define prt_meta(prt) (((union protocol_metadata_union_u *)(&(prt)->rsv))->meta)

struct connection_data_s {
  protocol_s *protocol;
  time_t active;
  uint8_t timeout;
  spn_lock_i scheduled;
  spn_lock_i lock;
};

static struct facil_data_s {
  spn_lock_i global_lock;
  uint8_t need_review;
  pool_pt thread_pool;
  pid_t parent;
  uint16_t active;
  uint16_t threads;
  ssize_t capacity;
  void (*on_idle)(void);
  void (*on_finish)(void);
  struct timespec last_cycle;
  struct connection_data_s conn[];
} * facil_data;

#define fd_data(fd) (facil_data->conn[(fd)])
#define uuid_data(uuid) fd_data(sock_uuid2fd((uuid)))
// #define uuid_prt_meta(uuid) prt_meta(uuid_data((uuid)).protocol)

static inline void clear_connection_data_unsafe(intptr_t uuid,
                                                protocol_s *protocol) {
  uuid_data(uuid) =
      (struct connection_data_s){.active = facil_data->last_cycle.tv_sec,
                                 .protocol = protocol,
                                 .lock = uuid_data(uuid).lock};
}
/** locks a connection's protocol returns a pointer that need to be unlocked. */
inline static protocol_s *protocol_try_lock(intptr_t fd,
                                            enum facil_protocol_lock_e type) {
  if (spn_trylock(&fd_data(fd).lock))
    goto would_block;
  protocol_s *pr = fd_data(fd).protocol;
  if (!pr) {
    spn_unlock(&fd_data(fd).lock);
    errno = EBADF;
    return NULL;
  }
  if (spn_trylock(&prt_meta(pr).locks[type])) {
    spn_unlock(&fd_data(fd).lock);
    goto would_block;
  }
  spn_unlock(&fd_data(fd).lock);
  return pr;
would_block:
  errno = EWOULDBLOCK;
  return NULL;
}
/** See `facil_protocol_try_lock` for details. */
inline static void protocol_unlock(protocol_s *pr,
                                   enum facil_protocol_lock_e type) {
  spn_unlock(&prt_meta(pr).locks[type]);
}

/* *****************************************************************************
Deferred event handlers
***************************************************************************** */
static void deferred_on_close(void *uuid_, void *pr_) {
  protocol_s *pr = pr_;
  if (pr->rsv)
    goto postpone;
  pr->on_close((intptr_t)uuid_, pr);
  return;
postpone:
  defer(deferred_on_close, uuid_, pr_);
}

static void deferred_on_shutdown(void *arg, void *arg2) {
  if (!uuid_data(arg).protocol) {
    return;
  }
  protocol_s *pr = protocol_try_lock(sock_uuid2fd(arg), FIO_PR_LOCK_WRITE);
  if (!pr) {
    if (errno == EBADF)
      return;
    goto postpone;
  }
  pr->on_shutdown((intptr_t)arg, pr);
  protocol_unlock(pr, FIO_PR_LOCK_WRITE);
  sock_close((intptr_t)arg);
  return;
postpone:
  defer(deferred_on_shutdown, arg, NULL);
  (void)arg2;
}

static void deferred_on_ready(void *arg, void *arg2) {
  if (!uuid_data(arg).protocol) {
    return;
  }
  protocol_s *pr = protocol_try_lock(sock_uuid2fd(arg), FIO_PR_LOCK_WRITE);
  if (!pr) {
    if (errno == EBADF)
      return;
    goto postpone;
  }
  pr->on_ready((intptr_t)arg, pr);
  if (sock_has_pending((intptr_t)arg))
    evio_add(sock_uuid2fd((intptr_t)arg), arg);
  protocol_unlock(pr, FIO_PR_LOCK_WRITE);
  return;
postpone:
  defer(deferred_on_ready, arg, NULL);
  (void)arg2;
}

static void deferred_on_data(void *arg, void *arg2) {
  if (!uuid_data(arg).protocol) {
    return;
  }
  protocol_s *pr = protocol_try_lock(sock_uuid2fd(arg), FIO_PR_LOCK_TASK);
  if (!pr) {
    if (errno == EBADF)
      return;
    goto postpone;
  }
  spn_unlock(&uuid_data(arg).scheduled);
  pr->on_data((intptr_t)arg, pr);
  protocol_unlock(pr, FIO_PR_LOCK_TASK);
  if (!spn_trylock(&uuid_data(arg).scheduled)) {
    evio_add(sock_uuid2fd((intptr_t)arg), arg);
  }
  // else
  //   fprintf(stderr, "skipped evio_add\n");
  return;
postpone:
  defer(deferred_on_data, arg, NULL);
  (void)arg2;
}

static void deferred_ping(void *arg, void *arg2) {
  if (!uuid_data(arg).protocol ||
      (uuid_data(arg).timeout &&
       (uuid_data(arg).timeout >
        (facil_data->last_cycle.tv_sec - uuid_data(arg).active)))) {
    return;
  }
  protocol_s *pr = protocol_try_lock(sock_uuid2fd(arg), FIO_PR_LOCK_WRITE);
  if (!pr)
    goto postpone;
  pr->ping((intptr_t)arg, pr);
  protocol_unlock(pr, FIO_PR_LOCK_WRITE);
  return;
postpone:
  defer(deferred_ping, arg, NULL);
  (void)arg2;
}

/* *****************************************************************************
Event Handlers (evio)
***************************************************************************** */
static void sock_flush_defer(void *arg, void *ignored) {
  (void)ignored;
  sock_flush((intptr_t)arg);
}

void evio_on_ready(void *arg) {
  defer(sock_flush_defer, arg, NULL);
  defer(deferred_on_ready, arg, NULL);
}
void evio_on_close(void *arg) { sock_force_close((intptr_t)arg); }
void evio_on_error(void *arg) { sock_force_close((intptr_t)arg); }
void evio_on_data(void *arg) { defer(deferred_on_data, arg, NULL); }

/* *****************************************************************************
Forcing IO events
***************************************************************************** */

void facil_force_event(intptr_t uuid, enum facil_io_event ev) {
  switch (ev) {
  case FIO_EVENT_ON_DATA:
    spn_trylock(&uuid_data(uuid).scheduled);
    evio_on_data((void *)uuid);
    break;
  case FIO_EVENT_ON_TIMEOUT:
    defer(deferred_ping, (void *)uuid, NULL);
    break;
  case FIO_EVENT_ON_READY:
    evio_on_ready((void *)uuid);
    break;
  }
}

/**
 * Temporarily prevents `on_data` events from firing.
 *
 * The `on_data` event will be automatically rescheduled when (if) the socket's
 * outgoing buffer fills up or when `facil_force_event` is called with
 * `FIO_EVENT_ON_DATA`.
 */
void facil_quite(intptr_t uuid) {
  if (sock_isvalid(uuid))
    spn_trylock(&uuid_data(uuid).scheduled);
}

/* *****************************************************************************
Socket callbacks
***************************************************************************** */

void sock_on_close(intptr_t uuid) {
  spn_lock(&uuid_data(uuid).lock);
  protocol_s *old_protocol = uuid_data(uuid).protocol;
  clear_connection_data_unsafe(uuid, NULL);
  spn_unlock(&uuid_data(uuid).lock);
  if (old_protocol)
    defer(deferred_on_close, (void *)uuid, old_protocol);
}

void sock_touch(intptr_t uuid) {
  uuid_data(uuid).active = facil_data->last_cycle.tv_sec;
}

/* *****************************************************************************
Mock Protocol Callbacks and Service Funcions
***************************************************************************** */
static void mock_on_ev(intptr_t uuid, protocol_s *protocol) {
  (void)uuid;
  (void)protocol;
}

static void mock_on_close(intptr_t uuid, protocol_s *protocol) {
  (void)(protocol);
  (void)(uuid);
}

static void mock_on_finish(intptr_t uuid, void *udata) {
  (void)(udata);
  (void)(uuid);
}

static void mock_ping(intptr_t uuid, protocol_s *protocol) {
  (void)(protocol);
  sock_force_close(uuid);
}
static void mock_idle(void) {}

/* Support for the default pub/sub cluster engine */
#pragma weak pubsub_cluster_init
void pubsub_cluster_init(void) {}

/* perform initialization for external services. */
static void facil_external_init(void) {}

/* perform cleanup for external services. */
static void facil_external_cleanup(void) {}

#pragma weak http_lib_init
void http_lib_init(void) {}
#pragma weak http_lib_cleanup
void http_lib_cleanup(void) {}

/* perform initialization for external services. */
static void facil_external_root_init(void) {
  http_lib_init();
  pubsub_cluster_init();
}
/* perform cleanup for external services. */
static void facil_external_root_cleanup(void) { http_lib_cleanup(); }

/* *****************************************************************************
Initialization and Cleanup
***************************************************************************** */
static spn_lock_i facil_libinit_lock = SPN_LOCK_INIT;

static void facil_libcleanup(void) {
  /* free memory */
  spn_lock(&facil_libinit_lock);
  if (facil_data) {
    munmap(facil_data,
           sizeof(*facil_data) + ((size_t)facil_data->capacity *
                                  sizeof(struct connection_data_s)));
    facil_external_root_cleanup();
    facil_data = NULL;
  }
  spn_unlock(&facil_libinit_lock);
}

static void facil_lib_init(void) {
  ssize_t capa = sock_max_capacity();
  if (capa < 0) {
    perror("ERROR: socket capacity unknown / failure");
    exit(ENOMEM);
  }
  size_t mem_size =
      sizeof(*facil_data) + ((size_t)capa * sizeof(struct connection_data_s));
  spn_lock(&facil_libinit_lock);
  if (facil_data)
    goto finish;
  facil_data = mmap(NULL, mem_size, PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (!facil_data) {
    perror("ERROR: Couldn't initialize the facil.io library");
    exit(0);
  }
  memset(facil_data, 0, mem_size);
  *facil_data = (struct facil_data_s){.capacity = capa, .parent = getpid()};
  facil_external_root_init();
  atexit(facil_libcleanup);
#ifdef DEBUG
  if (FACIL_PRINT_STATE)
    fprintf(stderr,
            "Initialized the facil.io library.\n"
            "facil.io's memory footprint per connection == %lu Bytes X %lu\n"
            "=== facil.io's memory footprint: %lu ===\n\n",
            (unsigned long)sizeof(struct connection_data_s),
            (unsigned long)facil_data->capacity, (unsigned long)mem_size);
#endif
finish:
  spn_unlock(&facil_libinit_lock);
  clock_gettime(CLOCK_REALTIME, &facil_data->last_cycle);
}

static void facil_stop(void) {
  if (!facil_data)
    return;
  facil_data->active = 0;
  if (facil_data->thread_pool)
    defer_pool_stop(facil_data->thread_pool);
}

/* *****************************************************************************
The listenning protocol
***************************************************************************** */
#undef facil_listen

static const char *listener_protocol_name =
    "listening protocol __facil_internal__";

struct ListenerProtocol {
  protocol_s protocol;
  void (*on_open)(void *uuid, void *udata);
  void *udata;
  void (*on_start)(intptr_t uuid, void *udata);
  void (*on_finish)(intptr_t uuid, void *udata);
  char *port;
  char *address;
  uint8_t quite;
};

static void listener_ping(intptr_t uuid, protocol_s *plistener) {
  // fprintf(stderr, "*** Listener Ping Called for %ld\n", sock_uuid2fd(uuid));
  uuid_data(uuid).active = facil_data->last_cycle.tv_sec;
  return;
  (void)plistener;
}

static void listener_on_data(intptr_t uuid, protocol_s *plistener) {
  intptr_t new_client;
  if ((new_client = sock_accept(uuid)) == -1) {
    if (errno == ECONNABORTED || errno == ECONNRESET)
      goto reschedule;
    else if (errno != EWOULDBLOCK && errno != EAGAIN)
      perror("ERROR: socket accept error");
    return;
  }

  // to defer or not to defer...? TODO: answer the question
  struct ListenerProtocol *listener = (struct ListenerProtocol *)plistener;
  defer(listener->on_open, (void *)new_client, listener->udata);

reschedule:
  facil_force_event(uuid, FIO_EVENT_ON_DATA);
  return;
  (void)plistener;
}

static void free_listenner(void *li) { free(li); }

static void listener_on_close(intptr_t uuid, protocol_s *plistener) {
  struct ListenerProtocol *listener = (void *)plistener;
  listener->on_finish(uuid, listener->udata);
  if (FACIL_PRINT_STATE && facil_data->parent == getpid()) {
    if (listener->port) {
      fprintf(stderr, "* Stopped listening on port %s\n", listener->port);
    } else {
      fprintf(stderr, "* Stopped listening on Unix Socket %s\n",
              listener->address);
    }
  }
  if (!listener->port) {
    unlink(listener->address);
  }
  free_listenner(listener);
}

static inline struct ListenerProtocol *
listener_alloc(struct facil_listen_args settings) {
  if (!settings.on_start)
    settings.on_start = mock_on_finish;
  if (!settings.on_finish)
    settings.on_finish = mock_on_finish;
  size_t port_len = 0;
  size_t addr_len = 0;
  if (settings.port) {
    port_len = strlen(settings.port) + 1;
  }
  if (settings.address) {
    addr_len = strlen(settings.address) + 1;
  }
  struct ListenerProtocol *listener =
      malloc(sizeof(*listener) + addr_len + port_len);

  if (listener) {
    *listener = (struct ListenerProtocol){
        .protocol.service = listener_protocol_name,
        .protocol.on_data = listener_on_data,
        .protocol.on_close = listener_on_close,
        .protocol.ping = listener_ping,
        .on_open = (void (*)(void *, void *))settings.on_open,
        .udata = settings.udata,
        .on_start = settings.on_start,
        .on_finish = settings.on_finish,
    };
    if (settings.port) {
      listener->port = (char *)(listener + 1);
      memcpy(listener->port, settings.port, port_len);
    }
    if (settings.address) {
      listener->address = (char *)(listener + 1);
      listener->address += port_len;
      memcpy(listener->address, settings.address, addr_len);
    }
    return listener;
  }
  return NULL;
}

inline static void listener_on_start(int fd) {
  intptr_t uuid = sock_fd2uuid((int)fd);
  if (uuid < 0) {
    fprintf(stderr, "ERROR: listening socket dropped?\n");
    kill(0, SIGINT);
    exit(4);
  }
  if (evio_add(fd, (void *)uuid) < 0) {
    perror("Couldn't register listening socket");
    kill(0, SIGINT);
    exit(4);
  }
  fd_data(fd).active = facil_data->last_cycle.tv_sec;
  // call the on_init callback
  struct ListenerProtocol *listener =
      (struct ListenerProtocol *)uuid_data(uuid).protocol;
  listener->on_start(uuid, listener->udata);
}

/**
Listens to a server with the following server settings (which MUST include
a default protocol).

This method blocks the current thread until the server is stopped (either
though a `srv_stop` function or when a SIGINT/SIGTERM is received).
*/
int facil_listen(struct facil_listen_args settings) {
  if (!facil_data)
    facil_lib_init();
  if (settings.on_open == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (!settings.port || settings.port[0] == 0 ||
      (settings.port[0] == '0' && settings.port[1] == 0)) {
    settings.port = NULL;
  }
  intptr_t uuid = sock_listen(settings.address, settings.port);
  if (uuid == -1) {
    return -1;
  }
  protocol_s *protocol = (void *)listener_alloc(settings);
  facil_attach(uuid, protocol);
  if (!protocol) {
    sock_close(uuid);
    return -1;
  }
  if (FACIL_PRINT_STATE && facil_data->parent == getpid()) {
    if (settings.port)
      fprintf(stderr, "* Listening on port %s\n", settings.port);
    else
      fprintf(stderr, "* Listening on Unix Socket at %s\n", settings.address);
  }
  return 0;
}

/* *****************************************************************************
Connect (as client)
***************************************************************************** */

static const char *connector_protocol_name = "connect protocol __internal__";

struct ConnectProtocol {
  protocol_s protocol;
  void (*on_connect)(void *uuid, void *udata);
  void (*on_fail)(intptr_t uuid, void *udata);
  void *udata;
  intptr_t uuid;
  uint8_t opened;
};

/* The first `ready` signal is fired when a connection was established */
static void connector_on_ready(intptr_t uuid, protocol_s *_connector) {
  struct ConnectProtocol *connector = (void *)_connector;
  sock_touch(uuid);
  if (connector->opened == 0) {
    connector->opened = 1;
    facil_set_timeout(uuid, 0); /* remove connection timeout settings */
    connector->on_connect((void *)uuid, connector->udata);
  }
  return;
}

/* If data events reach this protocol, delay their execution. */
static void connector_on_data(intptr_t uuid, protocol_s *connector) {
  (void)connector;
  facil_force_event(uuid, FIO_EVENT_ON_DATA);
}

/* Failed to connect */
static void connector_on_close(intptr_t uuid, protocol_s *pconnector) {
  struct ConnectProtocol *connector = (void *)pconnector;
  if (connector->opened == 0 && connector->on_fail)
    connector->on_fail(connector->uuid, connector->udata);
  free(connector);
  (void)uuid;
}

#undef facil_connect
intptr_t facil_connect(struct facil_connect_args opt) {
  intptr_t uuid = -1;
  if (!opt.on_connect || (!opt.address && !opt.port))
    goto error;
  if (!opt.timeout)
    opt.timeout = 30;
  struct ConnectProtocol *connector = malloc(sizeof(*connector));
  if (!connector)
    goto error;
  *connector = (struct ConnectProtocol){
      .on_connect = (void (*)(void *, void *))opt.on_connect,
      .on_fail = opt.on_fail,
      .udata = opt.udata,
      .protocol.service = connector_protocol_name,
      .protocol.on_data = connector_on_data,
      .protocol.on_ready = connector_on_ready,
      .protocol.on_close = connector_on_close,
      .opened = 0,
  };
  uuid = connector->uuid = sock_connect(opt.address, opt.port);
  /* check for errors, always invoke the on_fail if required */
  if (uuid == -1) {
    goto error;
  }
  if (facil_attach(uuid, &connector->protocol) == -1) {
    sock_close(uuid);
    goto error;
  }
  facil_set_timeout(uuid, opt.timeout);
  return uuid;
error:
  if (opt.on_fail)
    opt.on_fail(uuid, opt.udata);
  return -1;
}
#define facil_connect(...)                                                     \
  facil_connect((struct facil_connect_args){__VA_ARGS__})

/* *****************************************************************************
Timers
***************************************************************************** */

/* *******
Timer Protocol
******* */
typedef struct {
  protocol_s protocol;
  size_t milliseconds;
  size_t repetitions;
  void (*task)(void *);
  void (*on_finish)(void *);
  void *arg;
} timer_protocol_s;

#define prot2timer(protocol) (*((timer_protocol_s *)(protocol)))

static const char *timer_protocol_name = "timer protocol __facil_internal__";

static void timer_on_data(intptr_t uuid, protocol_s *protocol) {
  prot2timer(protocol).task(prot2timer(protocol).arg);
  if (prot2timer(protocol).repetitions == 0)
    goto reschedule;
  prot2timer(protocol).repetitions -= 1;
  if (prot2timer(protocol).repetitions)
    goto reschedule;
  sock_force_close(uuid);
  return;
reschedule:
  spn_trylock(&uuid_data(uuid).scheduled);
  evio_set_timer(sock_uuid2fd(uuid), (void *)uuid,
                 prot2timer(protocol).milliseconds);
}

static void timer_on_close(intptr_t uuid, protocol_s *protocol) {
  prot2timer(protocol).on_finish(prot2timer(protocol).arg);
  free(protocol);
  (void)uuid;
}

static void timer_ping(intptr_t uuid, protocol_s *protocol) {
  sock_touch(uuid);
  (void)protocol;
}

static inline timer_protocol_s *timer_alloc(void (*task)(void *), void *arg,
                                            size_t milliseconds,
                                            size_t repetitions,
                                            void (*on_finish)(void *)) {
  if (!on_finish)
    on_finish = (void (*)(void *))mock_on_close;
  timer_protocol_s *t = malloc(sizeof(*t));
  if (t)
    *t = (timer_protocol_s){
        .protocol.service = timer_protocol_name,
        .protocol.on_data = timer_on_data,
        .protocol.on_close = timer_on_close,
        .protocol.ping = timer_ping,
        .arg = arg,
        .task = task,
        .on_finish = on_finish,
        .milliseconds = milliseconds,
        .repetitions = repetitions,
    };
  return t;
}

inline static void timer_on_server_start(int fd) {
  if (evio_set_timer(fd, (void *)sock_fd2uuid(fd),
                     prot2timer(fd_data(fd).protocol).milliseconds)) {
    perror("Couldn't register a required timed event.");
    kill(0, SIGINT);
    exit(4);
  }
}

/**
 * Creates a system timer (at the cost of 1 file descriptor).
 *
 * The task will repeat `repetitions` times. If `repetitions` is set to 0, task
 * will repeat forever.
 *
 * Returns -1 on error or the new file descriptor on succeess.
 *
 * The `on_finish` handler is always called (even on error).
 */
int facil_run_every(size_t milliseconds, size_t repetitions,
                    void (*task)(void *), void *arg,
                    void (*on_finish)(void *)) {
  if (task == NULL)
    goto error_fin;
  timer_protocol_s *protocol = NULL;
  intptr_t uuid = -1;
  int fd = evio_open_timer();
  if (fd == -1) {
    perror("ERROR: couldn't create a timer fd");
    goto error;
  }
  uuid = sock_open(fd);
  if (uuid == -1)
    goto error;
  protocol = timer_alloc(task, arg, milliseconds, repetitions, on_finish);
  if (protocol == NULL)
    goto error;
  facil_attach(uuid, (protocol_s *)protocol);
  if (evio_isactive() && evio_set_timer(fd, (void *)uuid, milliseconds) == -1)
    goto error;
  return 0;
error:
  if (uuid != -1) {
    const int old = errno;
    sock_close(uuid);
    errno = old;
  } else if (fd != -1) {
    const int old = errno;
    close(fd);
    errno = old;
  }
error_fin:
  if (on_finish) {
    const int old = errno;
    on_finish(arg);
    errno = old;
  }
  return -1;
}

/* *****************************************************************************
Cluster Messaging - using Unix Sockets
***************************************************************************** */

#ifdef __BIG_ENDIAN__
inline static uint32_t cluster_str2uint32(uint8_t *str) {
  return ((str[0] & 0xFF) | (((uint32_t)str[1] << 8) & 0xFF00) |
          (((uint32_t)str[2] << 16) & 0xFF0000) |
          (((uint32_t)str[3] << 24) & 0xFF000000));
}
inline static void cluster_uint2str(uint8_t *dest, uint32_t i) {
  dest[0] = i & 0xFF;
  dest[1] = (i >> 8) & 0xFF;
  dest[2] = (i >> 16) & 0xFF;
  dest[3] = (i >> 24) & 0xFF;
}
#else
inline static uint32_t cluster_str2uint32(uint8_t *str) {
  return ((((uint32_t)str[0] << 24) & 0xFF000000) |
          (((uint32_t)str[1] << 16) & 0xFF0000) |
          (((uint32_t)str[2] << 8) & 0xFF00) | (str[3] & 0xFF));
}
inline static void cluster_uint2str(uint8_t *dest, uint32_t i) {
  dest[0] = (i >> 24) & 0xFF;
  dest[1] = (i >> 16) & 0xFF;
  dest[2] = (i >> 8) & 0xFF;
  dest[3] = i & 0xFF;
}
#endif

#define CLUSTER_READ_BUFFER 16384
typedef struct {
  protocol_s pr;
  FIOBJ channel;
  FIOBJ msg;
  uint32_t exp_channel;
  uint32_t exp_msg;
  uint32_t type;
  int32_t filter;
  uint32_t length;
  uint8_t buffer[];
} cluster_pr_s;

typedef struct {
  void (*on_message)(int32_t filter, FIOBJ, FIOBJ);
  FIOBJ channel;
  FIOBJ msg;
  int32_t filter;
} cluster_msg_data_s;

static void cluster_on_new_peer(intptr_t srv, protocol_s *pr);
static void cluster_on_listening_ping(intptr_t srv, protocol_s *pr);
static void cluster_on_listening_close(intptr_t srv, protocol_s *pr);

static struct {
  protocol_s listening;
  intptr_t root;
  fio_hash_s clients;
  fio_hash_s handlers;
  spn_lock_i lock;
  uint8_t client_mode;
  char cluster_name[128];
} facil_cluster_data = {
    .lock = SPN_LOCK_INIT,
    .root = -1,
    .listening =
        {
            .on_close = cluster_on_listening_close,
            .ping = cluster_on_listening_ping,
            .on_data = cluster_on_new_peer,
        },
};

enum cluster_message_type_e {
  CLUSTER_MESSAGE_FORWARD,
  CLUSTER_MESSAGE_JSON,
  CLUSTER_MESSAGE_SHUTDOWN,
  CLUSTER_MESSAGE_ERROR,
  CLUSTER_MESSAGE_PING,
};

static void facil_worker_startup(uint8_t sentinal);
static void facil_worker_cleanup(void);

static void cluster_deferred_handler(void *msg_data_, void *ignr) {
  cluster_msg_data_s *data = msg_data_;
  data->on_message(data->filter, data->channel, data->msg);
  fiobj_free(data->channel);
  fiobj_free(data->msg);
  free(data);
  (void)ignr;
}

static void cluster_forward_msg2handlers(cluster_pr_s *c) {
  spn_lock(&facil_cluster_data.lock);
  void *target_ =
      fio_hash_find(&facil_cluster_data.handlers, (FIO_HASH_KEY_TYPE)c->filter);
  spn_unlock(&facil_cluster_data.lock);
  // fprintf(stderr, "handler for %d: %p\n", c->filter, target_);
  if (target_) {
    cluster_msg_data_s *data = malloc(sizeof(*data));
    if (!data) {
      perror("FATAL ERROR: (facil.io cluster) couldn't allocate memory");
      exit(errno);
    }
    *data = (cluster_msg_data_s){
        .on_message = ((cluster_msg_data_s *)(&target_))->on_message,
        .channel = fiobj_dup(c->channel),
        .msg = fiobj_dup(c->msg),
        .filter = c->filter,
    };
    defer(cluster_deferred_handler, data, NULL);
  }
}

static inline FIOBJ cluster_wrap_message(uint32_t ch_len, uint32_t msg_len,
                                         uint32_t type, int32_t id,
                                         uint8_t *ch_data, uint8_t *msg_data) {
  FIOBJ buf = fiobj_str_buf(ch_len + msg_len + 16);
  fio_cstr_s f = fiobj_obj2cstr(buf);
  cluster_uint2str(f.bytes, ch_len);
  cluster_uint2str(f.bytes + 4, msg_len);
  cluster_uint2str(f.bytes + 8, type);
  cluster_uint2str(f.bytes + 12, (uint32_t)id);
  if (ch_data) {
    memcpy(f.bytes + 16, ch_data, ch_len);
  }
  if (msg_data) {
    memcpy(f.bytes + 16 + ch_len, msg_data, msg_len);
  }
  fiobj_str_resize(buf, ch_len + msg_len + 16);
  return buf;
}

static inline void cluster_send2clients(uint32_t ch_len, uint32_t msg_len,
                                        uint32_t type, int32_t id,
                                        uint8_t *ch_data, uint8_t *msg_data,
                                        intptr_t uuid) {
  if (facil_cluster_data.clients.count == 0)
    return;
  FIOBJ forward =
      cluster_wrap_message(ch_len, msg_len, type, id, ch_data, msg_data);
  spn_lock(&facil_cluster_data.lock);
  FIO_HASH_FOR_LOOP(&facil_cluster_data.clients, i) {
    if (i->obj) {
      if ((intptr_t)i->key != uuid)
        fiobj_send_free((intptr_t)i->key, fiobj_dup(forward));
    }
  }
  spn_unlock(&facil_cluster_data.lock);
  fiobj_free(forward);
}

static inline void cluster_send2traget(uint32_t ch_len, uint32_t msg_len,
                                       uint32_t type, int32_t id,
                                       uint8_t *ch_data, uint8_t *msg_data) {
  if (facil_cluster_data.client_mode) {
    FIOBJ forward =
        cluster_wrap_message(ch_len, msg_len, type, id, ch_data, msg_data);
    fiobj_send_free(facil_cluster_data.root, fiobj_dup(forward));
  } else {
    cluster_send2clients(ch_len, msg_len, type, id, ch_data, msg_data, 0);
  }
}

static void cluster_on_client_message(cluster_pr_s *c, intptr_t uuid) {
  switch ((enum cluster_message_type_e)c->type) {
  case CLUSTER_MESSAGE_JSON: {
    fio_cstr_s s = fiobj_obj2cstr(c->channel);
    FIOBJ tmp = FIOBJ_INVALID;
    if (fiobj_json2obj(&tmp, s.bytes, s.len)) {
      fiobj_free(c->channel);
      c->channel = tmp;
      tmp = FIOBJ_INVALID;
    } else {
      fprintf(stderr,
              "WARNING: (facil.io cluster) JSON message isn't valid JSON.\n");
    }
    s = fiobj_obj2cstr(c->msg);
    if (fiobj_json2obj(&tmp, s.bytes, s.len)) {
      fiobj_free(c->msg);
      c->msg = tmp;
    } else {
      fprintf(stderr,
              "WARNING: (facil.io cluster) JSON message isn't valid JSON.\n");
    }
  }
  /* fallthrough */
  case CLUSTER_MESSAGE_FORWARD:
    cluster_forward_msg2handlers(c);
    break;

  case CLUSTER_MESSAGE_ERROR:
  case CLUSTER_MESSAGE_SHUTDOWN:
    facil_stop();
    sock_close(uuid);
    facil_cluster_data.root = -1;
    break;

  case CLUSTER_MESSAGE_PING:
    /* do nothing, really. */
    break;
  }
}

static void cluster_on_server_message(cluster_pr_s *c, intptr_t uuid) {
  switch ((enum cluster_message_type_e)c->type) {
  case CLUSTER_MESSAGE_JSON:
  case CLUSTER_MESSAGE_FORWARD: {
    if (fio_hash_count(&facil_cluster_data.clients)) {
      fio_cstr_s cs = fiobj_obj2cstr(c->channel);
      fio_cstr_s ms = fiobj_obj2cstr(c->msg);
      cluster_send2clients((uint32_t)cs.len, (uint32_t)ms.len, c->type,
                           c->filter, cs.bytes, ms.bytes, uuid);
    }
    if (c->type == CLUSTER_MESSAGE_JSON) {
      fio_cstr_s s = fiobj_obj2cstr(c->channel);
      FIOBJ tmp = FIOBJ_INVALID;
      if (fiobj_json2obj(&tmp, s.bytes, s.len)) {
        fiobj_free(c->channel);
        c->channel = tmp;
        tmp = FIOBJ_INVALID;
      } else {
        fprintf(stderr,
                "WARNING: (facil.io cluster) JSON message isn't valid JSON.\n");
      }
      s = fiobj_obj2cstr(c->msg);
      if (fiobj_json2obj(&tmp, s.bytes, s.len)) {
        fiobj_free(c->msg);
        c->msg = tmp;
      } else {
        fprintf(stderr,
                "WARNING: (facil.io cluster) JSON message isn't valid JSON.\n");
      }
    }
    cluster_forward_msg2handlers(c);
    break;
  }
  case CLUSTER_MESSAGE_SHUTDOWN:
  case CLUSTER_MESSAGE_ERROR:
  case CLUSTER_MESSAGE_PING:
    /* do nothing, really. */
    break;
  }
}

static void cluster_on_server_close(intptr_t uuid, protocol_s *pr_) {
  if (facil_cluster_data.client_mode)
    return; /* we respawned. */
  spn_lock(&facil_cluster_data.lock);
  fio_hash_insert(&facil_cluster_data.clients, (FIO_HASH_KEY_TYPE)uuid, NULL);
  spn_unlock(&facil_cluster_data.lock);
  cluster_pr_s *c = (cluster_pr_s *)pr_;
  if (facil_data->active) {
#if DEBUG
    cluster_send2clients(0, 0, CLUSTER_MESSAGE_SHUTDOWN, 0, NULL, NULL, 0);
    sock_close(uuid);
    if (FACIL_PRINT_STATE)
      fprintf(stderr,
              "* (%d) Worker crash detected, signaling for exit (debug).\n",
              getpid());
    facil_stop();
#else
    fprintf(stderr, "ERROR: Wroker crash detected, spinning new worker.\n");
    if (!facil_fork()) {
      defer_on_fork();
      facil_cluster_data.client_mode = 1;
      sock_close(facil_cluster_data.root);
      facil_cluster_data.root = -1;
      evio_close();

      for (intptr_t i = 0; i < facil_data->capacity; i++) {
        if (fd_data(i).protocol &&
            (fd_data(i).protocol->service != listener_protocol_name &&
             fd_data(i).protocol->service != timer_protocol_name)) {
          close(i); /* close first to prevent TCP/IP shutdown */
          sock_close(sock_fd2uuid(i));
        }
      }
      defer_pool_stop(facil_data->thread_pool);
      defer_pool_wait(facil_data->thread_pool);
      facil_data->thread_pool = NULL;
      facil_worker_startup(0);
      defer_pool_wait(facil_data->thread_pool);
      facil_worker_cleanup();
      exit(0);
    }
#endif
  }
  fiobj_free(c->msg);
  fiobj_free(c->channel);
  free(c);
}
static void cluster_on_client_close(intptr_t uuid, protocol_s *pr_) {
  cluster_pr_s *c = (cluster_pr_s *)pr_;
  /* no shutdown message received - parent crashed. */
  if (facil_cluster_data.root == uuid && c->type != CLUSTER_MESSAGE_SHUTDOWN &&
      facil_data->active) {
    if (FACIL_PRINT_STATE)
      fprintf(stderr,
              "* (%d) Parent Process crash detected, signaling for exit.\n",
              getpid());
    facil_stop();
    unlink(facil_cluster_data.cluster_name);
  }
  fiobj_free(c->msg);
  fiobj_free(c->channel);
  free(c);
  facil_cluster_data.root = -1;
}

static void cluster_on_shutdown(intptr_t uuid, protocol_s *pr_) {
  cluster_send2traget(0, 0, CLUSTER_MESSAGE_SHUTDOWN, 0, NULL, NULL);
  (void)pr_;
  (void)uuid;
}

static void cluster_on_data(intptr_t uuid, protocol_s *pr_) {
  cluster_pr_s *c = (cluster_pr_s *)pr_;
  ssize_t i =
      sock_read(uuid, c->buffer + c->length, CLUSTER_READ_BUFFER - c->length);
  if (i <= 0)
    return;
  c->length += i;
  i = 0;
  do {
    if (!c->exp_channel && !c->exp_msg) {
      if (c->length - i < 16)
        break;
      c->exp_channel = cluster_str2uint32(c->buffer + i);
      c->exp_msg = cluster_str2uint32(c->buffer + i + 4);
      c->type = cluster_str2uint32(c->buffer + i + 8);
      c->filter = (int32_t)cluster_str2uint32(c->buffer + i + 12);
      if (c->exp_channel)
        c->channel = fiobj_str_buf(c->exp_channel);
      if (c->exp_msg)
        c->msg = fiobj_str_buf(c->exp_msg);
      i += 16;
    }
    if (c->exp_channel) {
      if (c->exp_channel + i > c->length) {
        fiobj_str_write(c->channel, (char *)c->buffer + i,
                        (size_t)(c->length - i));
        i = c->length;
        c->exp_channel -= i;
        break;
      } else {
        fiobj_str_write(c->channel, (char *)c->buffer + i, c->exp_channel);
        i += c->exp_channel;
        c->exp_channel = 0;
      }
    }
    if (c->exp_msg) {
      if (c->exp_msg + i > c->length) {
        fiobj_str_write(c->msg, (char *)c->buffer + i, (size_t)(c->length - i));
        i = c->length;
        c->exp_msg -= i;
        break;
      } else {
        fiobj_str_write(c->msg, (char *)c->buffer + i, c->exp_msg);
        i += c->exp_msg;
        c->exp_msg = 0;
      }
    }
    if (facil_cluster_data.client_mode) {
      cluster_on_client_message(c, uuid);
    } else {
      cluster_on_server_message(c, uuid);
    }
    fiobj_free(c->msg);
    fiobj_free(c->channel);
    c->msg = FIOBJ_INVALID;
    c->channel = FIOBJ_INVALID;
  } while (c->length > i);
  c->length -= i;
  if (c->length) {
    memmove(c->buffer, c->buffer + i, c->length);
  }
  (void)pr_;
}
static void cluster_ping(intptr_t uuid, protocol_s *pr_) {
  static uint8_t buffer[12];
  cluster_uint2str(buffer, (uint32_t)0);
  cluster_uint2str(buffer + 4, CLUSTER_MESSAGE_PING);
  cluster_uint2str(buffer + 8, 0);
  sock_write2(.uuid = uuid, .buffer = buffer, .length = 12,
              .dealloc = SOCK_DEALLOC_NOOP);
  (void)pr_;
}

static void cluster_on_open(intptr_t fd, void *udata) {
  cluster_pr_s *pr = malloc(sizeof(*pr) + CLUSTER_READ_BUFFER);
  *pr = (cluster_pr_s){
      .pr =
          {
              .service = "facil_io_cluster_protocol",
              .on_data = cluster_on_data,
              .on_shutdown = cluster_on_shutdown,
              .on_close =
                  (facil_cluster_data.client_mode ? cluster_on_client_close
                                                  : cluster_on_server_close),
              .ping = cluster_ping,
          },
  };
  if (facil_cluster_data.root != fd) {
    spn_lock(&facil_cluster_data.lock);
    fio_hash_insert(&facil_cluster_data.clients, (FIO_HASH_KEY_TYPE)fd,
                    (void *)fd);
    spn_unlock(&facil_cluster_data.lock);
  }
  if (facil_attach(fd, &pr->pr) == -1)
    fprintf(stderr, "ERROR: (facil.io cluster) couldn't attach connection\n");
  (void)udata;
}

static void cluster_on_new_peer(intptr_t srv, protocol_s *pr) {
  intptr_t client = sock_accept(srv);
  if (client == -1)
    fprintf(stderr, "ERROR: (facil.io cluster) couldn't accept connection\n");
  else {
    cluster_on_open(client, NULL);
  }
  (void)pr;
}
static void cluster_on_listening_close(intptr_t srv, protocol_s *pr) {
  fio_hash_free(&facil_cluster_data.clients);
  facil_cluster_data.clients = (fio_hash_s){0};
  if (facil_parent_pid() == getpid())
    unlink(facil_cluster_data.cluster_name);
  facil_cluster_data.root = -1;
  (void)srv;
  (void)pr;
}
static void cluster_on_listening_ping(intptr_t srv, protocol_s *pr) {
  sock_touch(srv);
  (void)pr;
}

static void cluster_on_start(void *udata1, void *udata) {
  if (facil_data->active <= 1)
    return;
  if (facil_parent_pid() == getpid()) {
    facil_cluster_data.client_mode = 0;
    if (facil_attach(facil_cluster_data.root, &facil_cluster_data.listening)) {
      perror("FATAL ERROR: (facil.io) couldn't attach cluster socket");
    }
    // facil_force_event(facil_cluster_data.root, FIO_EVENT_ON_DATA);
  } else {
    facil_cluster_data.client_mode = 1;
    close(sock_uuid2fd(facil_cluster_data.root)); /* prevent `shutdown` */
    sock_close(facil_cluster_data.root);
    FIO_HASH_FOR_LOOP(&facil_cluster_data.clients, i) {
      sock_close((intptr_t)i->key);
    }
    fio_hash_free(&facil_cluster_data.clients);
    facil_cluster_data.clients = (fio_hash_s)FIO_HASH_INIT;
    facil_cluster_data.root =
        facil_connect(.address = facil_cluster_data.cluster_name,
                      .on_connect = cluster_on_open);
    if (facil_cluster_data.root == -1) {
      perror(
          "FATAL ERROR: (facil.io cluster) couldn't connect to cluster socket");
      fprintf(stderr, "         socket: %s\n", facil_cluster_data.cluster_name);
      facil_stop();
    }
  }
  (void)udata;
  (void)udata1;
}

static int facil_cluster_init(void) {
  /* create a unique socket name */
  char *tmp_folder = getenv("TMPDIR");
  uint32_t tmp_folder_len = 0;
  if (!tmp_folder || ((tmp_folder_len = (uint32_t)strlen(tmp_folder)) > 100)) {
    tmp_folder = P_tmpdir;
    if (tmp_folder)
      tmp_folder_len = (uint32_t)strlen(tmp_folder);
  }
  if (tmp_folder_len >= 100)
    tmp_folder_len = 0;
  if (tmp_folder_len) {
    memcpy(facil_cluster_data.cluster_name, tmp_folder, tmp_folder_len);
    if (facil_cluster_data.cluster_name[tmp_folder_len - 1] != '/')
      facil_cluster_data.cluster_name[tmp_folder_len++] = '/';
  }
  memcpy(facil_cluster_data.cluster_name + tmp_folder_len, "facil-io-sock-",
         14);
  tmp_folder_len += 14;
  tmp_folder_len +=
      fio_ltoa(facil_cluster_data.cluster_name + tmp_folder_len, getpid(), 8);
  facil_cluster_data.cluster_name[tmp_folder_len] = 0;

  /* remove if existing */
  unlink(facil_cluster_data.cluster_name);
  /* create, bind, listen */
  facil_cluster_data.root = sock_listen(facil_cluster_data.cluster_name, NULL);

  if (facil_cluster_data.root == -1) {
    perror("FATAL ERROR: (facil.io cluster) failed to open cluster socket.\n"
           "             check file permissions");
    return -1;
  }
  return 0;
}

void facil_cluster_set_handler(int32_t filter,
                               void (*on_message)(int32_t id, FIOBJ ch,
                                                  FIOBJ msg)) {
  spn_lock(&facil_cluster_data.lock);
  fio_hash_insert(&facil_cluster_data.handlers, (uint64_t)filter,
                  (void *)(uintptr_t)on_message);
  spn_unlock(&facil_cluster_data.lock);
}

int facil_cluster_send(int32_t filter, FIOBJ ch, FIOBJ msg) {
  if (!facil_data) {
    fprintf(stderr, "ERROR: cluster inactive, can't send message.\n");
    return -1;
  }
  uint32_t type = CLUSTER_MESSAGE_FORWARD;

  if ((!ch || FIOBJ_TYPE_IS(ch, FIOBJ_T_STRING)) &&
      (!msg || FIOBJ_TYPE_IS(msg, FIOBJ_T_STRING))) {
    fiobj_dup(ch);
    fiobj_dup(msg);
  } else {
    type = CLUSTER_MESSAGE_JSON;
    ch = fiobj_obj2json(ch, 0);
    msg = fiobj_obj2json(msg, 0);
  }
  fio_cstr_s cs = fiobj_obj2cstr(ch);
  fio_cstr_s ms = fiobj_obj2cstr(msg);
  cluster_send2traget((uint32_t)cs.len, (uint32_t)ms.len, type, filter,
                      cs.bytes, ms.bytes);
  fiobj_free(ch);
  fiobj_free(msg);
  return 0;
}

/* *****************************************************************************
Running the server
***************************************************************************** */

static void print_pid(void *arg, void *ignr) {
  (void)arg;
  (void)ignr;
  fprintf(stderr, "* %d is running.\n", getpid());
}

static void facil_review_timeout(void *arg, void *ignr) {
  (void)ignr;
  protocol_s *tmp;
  time_t review = facil_data->last_cycle.tv_sec;
  intptr_t fd = (intptr_t)arg;

  uint16_t timeout = fd_data(fd).timeout;
  if (!timeout)
    timeout = 300; /* enforced timout settings */

  if (!fd_data(fd).protocol || (fd_data(fd).active + timeout >= review))
    goto finish;
  tmp = protocol_try_lock(fd, FIO_PR_LOCK_STATE);
  if (!tmp)
    goto reschedule;
  if (prt_meta(tmp).locks[FIO_PR_LOCK_TASK] ||
      prt_meta(tmp).locks[FIO_PR_LOCK_WRITE])
    goto unlock;
  defer(deferred_ping, (void *)sock_fd2uuid((int)fd), NULL);
unlock:
  protocol_unlock(tmp, FIO_PR_LOCK_STATE);
finish:
  do {
    fd++;
  } while (!fd_data(fd).protocol && (fd < facil_data->capacity));

  if (facil_data->capacity <= fd) {
    facil_data->need_review = 1;
    return;
  }
reschedule:
  defer(facil_review_timeout, (void *)fd, NULL);
}

static void perform_idle(void *arg, void *ignr) {
  facil_data->on_idle();
  (void)arg;
  (void)ignr;
}

static void facil_cycle(void *arg, void *ignr) {
  (void)ignr;
  static int idle = 0;
  clock_gettime(CLOCK_REALTIME, &facil_data->last_cycle);
  int events;
  if (defer_has_queue()) {
    events = evio_review(0);
    if (events < 0) {
      goto error;
    }
    if (events > 0)
      idle = 1;
  } else {
    events = evio_review(512);
    if (events < 0)
      goto error;
    if (events > 0) {
      idle = 1;
    } else if (idle) {
      defer(perform_idle, arg, ignr);
      idle = 0;
    }
  }
  if (!facil_data->active)
    return;
  if (facil_data->need_review) {
    facil_data->need_review = 0;
    defer(facil_review_timeout, (void *)0, NULL);
  }
  defer(facil_cycle, arg, ignr);
  return;
error:
  if (facil_data->active)
    defer(facil_cycle, arg, ignr);
  (void)1;
}

/**
OVERRIDE THIS to replace the default `fork` implementation or to inject hooks
into the forking function.

Behaves like the system's `fork`.
*/
#pragma weak facil_fork
int facil_fork(void) { return (int)fork(); }

static void facil_worker_startup(uint8_t sentinal) {
  evio_create();
  clock_gettime(CLOCK_REALTIME, &facil_data->last_cycle);
  if (sentinal == 0) {
    for (int i = 0; i < facil_data->capacity; i++) {
      errno = 0;
      if (fd_data(i).protocol) {
        if (fd_data(i).protocol->service == listener_protocol_name)
          listener_on_start(i);
        else if (fd_data(i).protocol->service == timer_protocol_name)
          timer_on_server_start(i);
        else {
          evio_add(i, (void *)sock_fd2uuid(i));
        }
      }
    }
  } else {
    for (int i = 0; i < facil_data->capacity; i++) {
      if (fd_data(i).protocol &&
          fd_data(i).protocol->service == timer_protocol_name)
        timer_on_server_start(i);
      if (fd_data(i).protocol &&
          fd_data(i).protocol->service != listener_protocol_name)
        evio_add(i, (void *)sock_fd2uuid(i));
    }
  }
  facil_data->need_review = 1;
  facil_external_init();
  if (facil_data->active > 1)
    defer(cluster_on_start, NULL, NULL);
  defer(facil_cycle, NULL, NULL);

  if (FACIL_PRINT_STATE && facil_data->parent == getpid()) {
    fprintf(stderr, "Server is running %u %s X %u %s, press ^C to stop\n",
            facil_data->active, facil_data->active > 1 ? "workers" : "worker",
            facil_data->threads,
            facil_data->threads > 1 ? "threads" : "thread");
  }
  defer(print_pid, NULL, NULL);
  facil_data->thread_pool = defer_pool_start(facil_data->threads);
}

static void facil_worker_cleanup(void) {
  facil_data->active = 0;
  fprintf(stderr, "* %d cleanning up.\n", getpid());
  for (int i = 0; i < facil_data->capacity; i++) {
    intptr_t uuid;
    if (fd_data(i).protocol && (uuid = sock_fd2uuid(i)) >= 0) {
      defer(deferred_on_shutdown, (void *)uuid, NULL);
    }
  }
  evio_review(100);
  defer_perform();
  sock_flush_all();
  evio_review(0);
  sock_flush_all();
  facil_data->on_finish();
  defer_perform();
  evio_close();
  facil_external_cleanup();

  if (facil_data->parent == getpid()) {
    while (wait(NULL) != -1)
      ;
    if (FACIL_PRINT_STATE) {
      fprintf(stderr, "\n   ---  Completed Shutdown  ---\n");
    }
  }
}

/* handles the SIGINT and SIGTERM signals by shutting down workers */
static void sig_int_handler(int sig) {
  if (sig != SIGINT && sig != SIGTERM)
    return;
  facil_stop();
}

/* handles the SIGINT and SIGTERM signals by shutting down workers */
static void facil_setp_signal_handler(void) {
  /* setup signal handling */
  struct sigaction act, old;

  act.sa_handler = sig_int_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_RESTART | SA_NOCLDSTOP;

  if (sigaction(SIGINT, &act, &old)) {
    perror("couldn't set signal handler");
    return;
  };

  if (sigaction(SIGTERM, &act, &old)) {
    perror("couldn't set signal handler");
    return;
  };

  act.sa_handler = SIG_IGN;
  if (sigaction(SIGPIPE, &act, &old)) {
    perror("couldn't set signal handler");
    return;
  };
}

/*
 * Zombie Reaping
 * With thanks to Dr Graham D Shaw.
 * http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
 */
static void reap_child_handler(int sig) {
  (void)(sig);
  int old_errno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;
  errno = old_errno;
}

/* initializes zombie reaping for the process */
void facil_reap_children(void) {
  struct sigaction sa;
  sa.sa_handler = reap_child_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  if (sigaction(SIGCHLD, &sa, 0) == -1) {
    perror("Child reaping initialization failed");
    kill(0, SIGINT);
    exit(errno);
  }
}

/** returns facil.io's parent (root) process pid. */
pid_t facil_parent_pid(void) {
  if (!facil_data)
    facil_lib_init();
  return facil_data->parent;
}

#undef facil_run
void facil_run(struct facil_run_args args) {
  signal(SIGPIPE, SIG_IGN);
  if (!facil_data)
    facil_lib_init();
  if (!args.on_idle)
    args.on_idle = mock_idle;
  if (!args.on_finish)
    args.on_finish = mock_idle;
#ifdef _SC_NPROCESSORS_ONLN
  if (!args.threads && !args.processes) {
    ssize_t cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpu_count > 0)
      args.threads = args.processes = (int16_t)cpu_count;
  } else if (args.threads < 0 || args.processes < 0) {
    ssize_t cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpu_count > 0) {
      if (args.threads < 0)
        args.threads = (int16_t)cpu_count;
      if (args.processes < 0)
        args.processes = (int16_t)cpu_count;
    }
  }
#endif

  if (args.processes <= 0)
    args.processes = 1;
  if (args.threads <= 0)
    args.threads = 1;

  /* listen to SIGINT / SIGTERM */
  facil_setp_signal_handler();

  /* activate facil, fork if needed */
  facil_data->active = (uint16_t)args.processes;
  facil_data->threads = (uint16_t)args.threads;
  facil_data->on_finish = args.on_finish;
  facil_data->on_idle = args.on_idle;
  /* initialize cluster */
  if (args.processes > 1) {
    if (facil_cluster_init()) {
      kill(0, SIGINT);
      goto cleanup;
    }
    while (args.processes) {
      --args.processes;
      int pid = facil_fork();
      if (pid == -1) {
        perror("FATAL ERROR: couldn't spawn workers at startup");
        kill(0, SIGINT);
        goto cleanup;
      }
      if (!pid)
        break;
    }
    facil_worker_startup(facil_data->parent == getpid());
  } else {
    facil_worker_startup(0);
  }
  defer_pool_wait(facil_data->thread_pool);
  facil_worker_cleanup();
  return;

cleanup:
  facil_data->active = 0;
  if (facil_data->parent == getpid()) {
    while (wait(NULL) != -1)
      ;
    if (FACIL_PRINT_STATE) {
      fprintf(stderr, "\n   !!!  Crashed trying to start the service  !!!\n");
    }
  }
  exit(-1);
}

/**
 * returns true (1) if the facil.io engine is already running.
 */
int facil_is_running(void) { return facil_data->active > 0; }

/* *****************************************************************************
Setting the protocol
***************************************************************************** */

static int facil_attach_state(intptr_t uuid, protocol_s *protocol,
                              protocol_metadata_s state) {
  if (uuid == -1)
    return -1;
  if (!facil_data)
    facil_lib_init();
  if (protocol) {
    if (!protocol->on_close)
      protocol->on_close = mock_on_close;
    if (!protocol->on_data)
      protocol->on_data = mock_on_ev;
    if (!protocol->on_ready)
      protocol->on_ready = mock_on_ev;
    if (!protocol->ping)
      protocol->ping = mock_ping;
    if (!protocol->on_shutdown)
      protocol->on_shutdown = mock_on_ev;
    prt_meta(protocol) = state;
  }
  spn_lock(&uuid_data(uuid).lock);
  if (!sock_isvalid(uuid)) {
    spn_unlock(&uuid_data(uuid).lock);
    if (protocol)
      defer(deferred_on_close, (void *)uuid, protocol);
    return -1;
  }
  protocol_s *old_protocol = uuid_data(uuid).protocol;
  uuid_data(uuid).protocol = protocol;
  uuid_data(uuid).active = facil_data->last_cycle.tv_sec;
  spn_unlock(&uuid_data(uuid).lock);
  if (old_protocol)
    defer(deferred_on_close, (void *)uuid, old_protocol);
  else if (evio_isactive()) {
    evio_add(sock_uuid2fd(uuid), (void *)uuid);
  }
  return 0;
}

/** Attaches (or updates) a protocol object to a socket UUID.
 * Returns -1 on error and 0 on success.
 */
int facil_attach(intptr_t uuid, protocol_s *protocol) {
  return facil_attach_state(uuid, protocol, (protocol_metadata_s){.rsv = 0});
}

/**
 * Attaches (or updates) a LOCKED protocol object to a socket UUID.
 */
int facil_attach_locked(intptr_t uuid, protocol_s *protocol) {
  {
    protocol_metadata_s state = {.rsv = 0};
    spn_lock(state.locks + FIO_PR_LOCK_TASK);
    return facil_attach_state(uuid, protocol, state);
  }
}

/** Sets a timeout for a specific connection (if active). */
void facil_set_timeout(intptr_t uuid, uint8_t timeout) {
  if (sock_isvalid(uuid)) {
    uuid_data(uuid).active = facil_data->last_cycle.tv_sec;
    uuid_data(uuid).timeout = timeout;
  }
}
/** Gets a timeout for a specific connection. Returns 0 if there's no set
 * timeout or the connection is inactive. */
uint8_t facil_get_timeout(intptr_t uuid) { return uuid_data(uuid).timeout; }

/* *****************************************************************************
Misc helpers
***************************************************************************** */

/**
Returns the last time the server reviewed any pending IO events.
*/
struct timespec facil_last_tick(void) {
  if (!facil_data) {
    facil_lib_init();
    clock_gettime(CLOCK_REALTIME, &facil_data->last_cycle);
  }
  return facil_data->last_cycle;
}

/**
 * This function allows out-of-task access to a connection's `protocol_s`
 * object by attempting to lock it.
 */
protocol_s *facil_protocol_try_lock(intptr_t uuid,
                                    enum facil_protocol_lock_e type) {
  if (!sock_isvalid(uuid) || !uuid_data(uuid).protocol) {
    errno = EBADF;
    return NULL;
  }
  return protocol_try_lock(sock_uuid2fd(uuid), type);
}
/** See `facil_protocol_try_lock` for details. */
void facil_protocol_unlock(protocol_s *pr, enum facil_protocol_lock_e type) {
  if (!pr)
    return;
  protocol_unlock(pr, type);
}
/** Counts all the connections of a specific type. */
size_t facil_count(void *service) {
  size_t count = 0;
  for (intptr_t i = 0; i < facil_data->capacity; i++) {
    void *tmp = NULL;
    spn_lock(&fd_data(i).lock);
    if (fd_data(i).protocol && fd_data(i).protocol->service)
      tmp = (void *)fd_data(i).protocol->service;
    spn_unlock(&fd_data(i).lock);
    if (tmp != listener_protocol_name && tmp != timer_protocol_name &&
        (!service || (tmp == service)))
      count++;
  }
  return count;
}

/* *****************************************************************************
Task Management - `facil_defer`, `facil_each`
***************************************************************************** */

struct task {
  intptr_t origin;
  void (*func)(intptr_t uuid, protocol_s *, void *arg);
  void *arg;
  void (*on_done)(intptr_t uuid, void *arg);
  const void *service;
  uint32_t count;
  enum facil_protocol_lock_e task_type;
  spn_lock_i lock;
};

static inline struct task *alloc_facil_task(void) {
  return malloc(sizeof(struct task));
}

static inline void free_facil_task(struct task *task) { free(task); }

static void mock_on_task_done(intptr_t uuid, void *arg) {
  (void)uuid;
  (void)arg;
}

static void perform_single_task(void *v_uuid, void *v_task) {
  struct task *task = v_task;
  if (!uuid_data(v_uuid).protocol)
    goto fallback;
  protocol_s *pr = protocol_try_lock(sock_uuid2fd(v_uuid), task->task_type);
  if (!pr)
    goto defer;
  if (pr->service == connector_protocol_name) {
    protocol_unlock(pr, task->task_type);
    goto defer;
  }
  task->func((intptr_t)v_uuid, pr, task->arg);
  protocol_unlock(pr, task->task_type);
  free_facil_task(task);
  return;
fallback:
  task->on_done((intptr_t)v_uuid, task->arg);
  free_facil_task(task);
  return;
defer:
  defer(perform_single_task, v_uuid, v_task);
  return;
}

static void finish_multi_task(void *v_fd, void *v_task) {
  struct task *task = v_task;
  if (spn_trylock(&task->lock))
    goto reschedule;
  task->count--;
  if (task->count) {
    spn_unlock(&task->lock);
    return;
  }
  task->on_done(task->origin, task->arg);
  free_facil_task(task);
  return;
reschedule:
  defer(finish_multi_task, v_fd, v_task);
}

static void perform_multi_task(void *v_fd, void *v_task) {
  if (!fd_data((intptr_t)v_fd).protocol) {
    finish_multi_task(v_fd, v_task);
    return;
  }
  struct task *task = v_task;
  protocol_s *pr = protocol_try_lock((intptr_t)v_fd, task->task_type);
  if (!pr)
    goto reschedule;
  if (pr->service == task->service)
    task->func(sock_fd2uuid((int)(intptr_t)v_fd), pr, task->arg);
  protocol_unlock(pr, task->task_type);
  defer(finish_multi_task, v_fd, v_task);
  return;
reschedule:
  // fprintf(stderr, "rescheduling multi for %p\n", v_fd);
  defer(perform_multi_task, v_fd, v_task);
}

static void schedule_multi_task(void *v_fd, void *v_task) {
  struct task *task = v_task;
  intptr_t fd = (intptr_t)v_fd;
  for (size_t i = 0; i < 64; i++) {
    if (!fd_data(fd).protocol)
      goto finish;
    if (spn_trylock(&fd_data(fd).lock))
      goto reschedule;
    if (!fd_data(fd).protocol ||
        fd_data(fd).protocol->service != task->service || fd == task->origin) {
      spn_unlock(&fd_data(fd).lock);
      goto finish;
    }
    spn_unlock(&fd_data(fd).lock);
    spn_lock(&task->lock);
    task->count++;
    spn_unlock(&task->lock);
    defer(perform_multi_task, (void *)fd, task);
  finish:
    do {
      fd++;
    } while (!fd_data(fd).protocol && (fd < facil_data->capacity));
    if (fd >= (intptr_t)facil_data->capacity)
      goto complete;
  }
reschedule:
  schedule_multi_task((void *)fd, v_task);
  return;
complete:
  defer(finish_multi_task, NULL, v_task);
}
/**
 * Schedules a protected connection task. The task will run within the
 * connection's lock.
 *
 * If the connection is closed before the task can run, the
 * `fallback` task wil be called instead, allowing for resource cleanup.
 */
#undef facil_defer
void facil_defer(struct facil_defer_args_s args) {
  if (!args.fallback)
    args.fallback = mock_on_task_done;
  if (!args.type)
    args.type = FIO_PR_LOCK_TASK;
  if (!args.task || !uuid_data(args.uuid).protocol || args.uuid < 0 ||
      !sock_isvalid(args.uuid))
    goto error;
  struct task *task = alloc_facil_task();
  if (!task)
    goto error;
  *task = (struct task){
      .func = args.task, .arg = args.arg, .on_done = args.fallback};
  defer(perform_single_task, (void *)args.uuid, task);
  return;
error:
  defer((void (*)(void *, void *))args.fallback, (void *)args.uuid, args.arg);
}

/**
 * Schedules a protected connection task for each `service` connection.
 * The tasks will run within each of the connection's locks.
 *
 * Once all the tasks were performed, the `on_complete` callback will be called.
 */
#undef facil_each
int facil_each(struct facil_each_args_s args) {
  if (!args.on_complete)
    args.on_complete = mock_on_task_done;
  if (!args.task_type)
    args.task_type = FIO_PR_LOCK_TASK;
  if (!args.task)
    goto error;
  struct task *task = alloc_facil_task();
  if (!task)
    goto error;
  *task = (struct task){.origin = args.origin,
                        .func = args.task,
                        .arg = args.arg,
                        .on_done = args.on_complete,
                        .service = args.service,
                        .task_type = args.task_type,
                        .count = 1};
  defer(schedule_multi_task, (void *)0, task);
  return 0;
error:
  defer((void (*)(void *, void *))args.on_complete, (void *)args.origin,
        args.arg);
  return -1;
}
