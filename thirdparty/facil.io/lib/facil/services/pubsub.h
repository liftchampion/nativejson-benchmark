/*
Copyright: Boaz segev, 2017
License: MIT

Feel free to copy, use and enjoy according to the license provided.
*/
#ifndef H_FACIL_PUBSUB_H
/**
* This pub/sub API is designed to unload pub/sub stress from external messanging
* systems onto the local process.

* For example, the NULL pub/sub engine, which is routed to the facil_cluster
* engine, will only publish a single message per process instead of a message
per
* client, allowing the cluster communication channel to be less crowded when
* possible.
*
* This should allow pub/sub engines, such as Redis, to spread their workload
* between all of an application's processes, enhancing overall performance.
*/
#define H_FACIL_PUBSUB_H
#include "facil.h"
#include "fiobj.h"

/* support C++ */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef FIO_PUBBSUB_MAX_CHANNEL_LEN
#define FIO_PUBBSUB_MAX_CHANNEL_LEN 1024
#endif

/** An opaque pointer used to identify a subscription. */
typedef struct pubsub_sub_s *pubsub_sub_pt;

/** A pub/sub engine data structure. See details later on. */
typedef struct pubsub_engine_s pubsub_engine_s;

/** The default pub/sub engine.
 * This engine performs pub/sub within a group of processes (process cluster).
 *
 * The process cluser is initialized by the `facil_run` command with `processes`
 * set to more than 1.
 */
extern pubsub_engine_s const *PUBSUB_CLUSTER_ENGINE;

/** An engine that performs pub/sub only within a single process. */
extern pubsub_engine_s const *PUBSUB_PROCESS_ENGINE;

/** Allows process wide changes to the default Pub/Sub Engine.
 * Setting a new default before calling `facil_run` will change the default for
 * the whole process cluster.
 */
extern pubsub_engine_s *PUBSUB_DEFAULT_ENGINE;

/** Publishing and on_message callback arguments. */
typedef struct pubsub_message_s {
  /** The pub/sub engine that should be used to farward this message. */
  pubsub_engine_s const *engine;
  /** The pub/sub target channnel. */
  FIOBJ channel;
  /** The pub/sub message. */
  FIOBJ message;
  /** The subscription that prompted the message to be routed to the client. */
  pubsub_sub_pt subscription;
  /** Client opaque data pointer (from the `subscribe`) function call. */
  void *udata1;
  /** Client opaque data pointer (from the `subscribe`) function call. */
  void *udata2;
} pubsub_message_s;

/** The arguments used for `pubsub_subscribe` or `pubsub_find_sub`. */
struct pubsub_subscribe_args {
  /** The channel namr used for the subscription. */
  FIOBJ channel;
  /** The on message callback. the `*msg` pointer is to a temporary object. */
  void (*on_message)(pubsub_message_s *msg);
  /** An optional callback for when a subscription is fully canceled. */
  void (*on_unsubscribe)(void *udata1, void *udata2);
  /** Opaque user data#1 */
  void *udata1;
  /** Opaque user data#2 .. using two allows allocation to be avoided. */
  void *udata2;
  /** Use pattern matching for channel subscription. */
  unsigned use_pattern : 1;
};

/**
 * Subscribes to a specific channel.
 *
 * Returns a subscription pointer or NULL (failure).
 */
pubsub_sub_pt pubsub_subscribe(struct pubsub_subscribe_args);
#define pubsub_subscribe(...)                                                  \
  pubsub_subscribe((struct pubsub_subscribe_args){__VA_ARGS__})

/**
 * This helper searches for an existing subscription.
 *
 * Use with care, NEVER call `pubsub_unsubscribe` more times than you have
 * called `pubsub_subscribe`, since the subscription handle memory is realesed
 * onnce the reference count reaches 0.
 *
 * Returns a subscription pointer or NULL (none found).
 */
pubsub_sub_pt pubsub_find_sub(struct pubsub_subscribe_args);
#define pubsub_find_sub(...)                                                   \
  pubsub_find_sub((struct pubsub_subscribe_args){__VA_ARGS__})

/**
 * Unsubscribes from a specific subscription.
 *
 * Note: This should be called exactly the same number times as
 * `pubsub_subscribe`. Any less and a memory leak might occur. Any more, and the
 * program might crash.
 *
 * Returns 0 on success and -1 on failure.
 */
int pubsub_unsubscribe(pubsub_sub_pt subscription);

/**
 * Publishes a message to a channel belonging to a pub/sub service (engine).
 *
 * Returns 0 on success and -1 on failure (i.e., no channel, no message or no
 * known subscriptions).
 *
 * NOTE: Memory ownership is retained by the calling function. Both the channel
 * and the message should be freed when the caller is done with them.
 */
int pubsub_publish(struct pubsub_message_s);
#define pubsub_publish(...)                                                    \
  pubsub_publish((struct pubsub_message_s){__VA_ARGS__})

/**
 * defers message hadling if it can't be performed (i.e., resource is busy) or
 * should be fragmented (allowing large tasks to be broken down).
 *
 * This should only be called from within the `on_message` callback.
 *
 * It's recommended that the `on_message` callback return immediately following
 * this function call, as code might run concurrently.
 *
 * Uses reference counting for zero copy.
 *
 * It's impossible to use a different `on_message` callbck without resorting to
 * memory allocations... so when in need, manage routing withing the
 * `on_message` callback.
 */
void pubsub_defer(pubsub_message_s *msg);

/**
 * Pub/Sub services (engines) MUST provide the listed function pointers.
 *
 * Engines should also register using the `pubsub_engine_register` function.
 *
 * Engines should deregister, before being destroyed, by using the
 * `pubsub_engine_deregister` function.
 *
 * When an engine received a message to publish, they should call the
 * `pubsub_publish` function with the engine to which the message is forwarded.
 * i.e.:
 *
 *       pubsub_publish(
 *           .engine = PUBSUB_PROCESS_ENGINE,
 *           .channel = channel_name,
 *           .message = msg_body );
 *
 * Engines MUST NOT free any of the FIOBJ objects they receive.
 *
 */
struct pubsub_engine_s {
  /* Must subscribe channel. Failures are ignored. */
  void (*subscribe)(const pubsub_engine_s *eng, FIOBJ channel,
                    uint8_t use_pattern);
  /* Must unsubscribe channel. Failures are ignored. */
  void (*unsubscribe)(const pubsub_engine_s *eng, FIOBJ channel,
                      uint8_t use_pattern);
  /** Should return 0 on success and -1 on failure. */
  int (*publish)(const pubsub_engine_s *eng, FIOBJ channel, FIOBJ msg);
};

/** Registers an engine, so it's callback can be called. */
void pubsub_engine_register(pubsub_engine_s *engine);

/** Unregisters an engine, so it could be safely destroyed. */
void pubsub_engine_deregister(pubsub_engine_s *engine);

/**
 * Engines can ask facil.io to resubscribe to all active channels.
 *
 * This allows engines that lost their connection to their Pub/Sub service to
 * resubscribe all the currently active channels with the new connection.
 *
 * CAUTION: This is an evented task... try not to free the engine's memory while
 * resubscriptions are under way...
 */
void pubsub_engine_resubscribe(pubsub_engine_s *eng);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* H_FACIL_PUBSUB_H */
