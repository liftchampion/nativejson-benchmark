---
title: facil.io - Getting Started
toc: true
---
# {{ page.title }}

Following is a quick overview and examples for both HTTP/ Websocket WebApps and custom protocol network services.

These examples demonstrate how easy and empowering the `facil.io` framework can be.

## Creating your first facil.io application

Since facil.io is a source code library, it can be easily added to existing projects and it can be easily customized.

To start a new application using facil.io, run the following command from the terminal, replacing `appname` with the name of the new application.

     $ bash <(curl -s https://raw.githubusercontent.com/boazsegev/facil.io/master/scripts/new/app) appname

You can review the script in the GitHub repo. In short, it will create a new folder, download a copy of the latest release, add some demo boiler plate code and run `make clean` (which is required to build the `tmp` folder structure used for compiling the application).

Next, edit the `makefile` to remove any generic features you don't need, such as the `DUMP_LIB` feature, the `DEBUG` flag or the `DISAMS` disassembler and start development.

The new application's boiler plate code is a simple HTTP application that presents some text. You can build and run the application using the following command-line script from withing the application's folder:

    $ DEBUG=1 make run

You can also use the scripts in the provided scripts folder from the OS's folder viewer (i.e., finder on macOS).

## Writing HTTP and Websocket services in C? Easy!

Websockets and HTTP are super common, so `facil.io` comes with HTTP and Websocket extensions, allowing us to easily write HTTP/1.1 and Websocket services.

The framework's code is heavily documented using comments. You can use Doxygen to create automated documentation for the API.

The simplest example, of course, would be the famous "Hello World" application... this is so easy, it's practically boring (so we add custom headers and cookies):

```c
#include "http.h"

#include "http.h"

void on_request(http_request_s* request) {
  http_response_s * response = http_response_create(request);
  // http_response_log_start(response); // logging ?
  http_response_set_cookie(response, .name = "my_cookie", .value = "data");
  http_response_write_header(response, .name = "X-Data", .value = "my data");
  http_response_write_body(response, "Hello World!\r\n", 14);
  http_response_finish(response);
}

int main() {
  char* public_folder = NULL;
  // listen on port 3000, any available network binding (NULL == 0.0.0.0)
  http_listen("3000", NULL, .on_request = on_request,
               .public_folder = public_folder, .log_static = 0);
  // start the server
  facil_run(.threads = 4, .processes = 4);
}
```

But `facil.io` really shines when it comes to Websockets and real-time applications, where the `kqueue`/`epoll` engine gives the framework a high performance running start.

Here's a full-fledge example of a Websocket echo server:

```c
#include "websockets.h" // includes the "http.h" header

#include <stdio.h>
#include <stdlib.h>

/* ******************************
The Websocket echo implementation
*/

void ws_open(ws_s * ws) {
  fprintf(stderr, "Opened a new websocket connection (%p)\n", (void * )ws);
}

void ws_echo(ws_s * ws, char * data, size_t size, uint8_t is_text) {
  // echos the data to the current websocket
  websocket_write(ws, data, size, is_text);
}

void ws_shutdown(ws_s * ws) { websocket_write(ws, "Shutting Down", 13, 1); }

void ws_close(ws_s * ws) {
  fprintf(stderr, "Closed websocket connection (%p)\n", (void * )ws);
}

/* ********************
The HTTP implementation
*/

void on_request(http_request_s * request) {
  http_response_s * response = http_response_create(request);
  http_response_log_start(response); // logging
  // websocket upgrade.
  if (request->upgrade) {
    websocket_upgrade(.request = request, .on_message = ws_echo,
                      .on_open = ws_open, .on_close = ws_close, .timeout = 40,
                      .on_shutdown = ws_shutdown, .response = response);
    return;
  }
  // HTTP response
  http_response_write_body(response, "Hello World! Why not test me using Websockets\x3f", 46);
  http_response_finish(response);
}

/****************
The main function
*/

#define THREAD_COUNT 1
int main(void) {
  const char* public_folder = NULL;
  http_listen("3000", NULL, .on_request = on_request,
               .public_folder = public_folder, .log_static = 1);
  facil_run(.threads = THREAD_COUNT);
  return 0;
}
```

## Simple API (Echo example)

[facil.io](http://facil.io)'s API is designed for both simplicity and an object oriented approach, using network protocol objects and structs to avoid bloating function arguments and to provide sensible default behavior.

Here's a simple Echo example (test with telnet to port `"3000"`).

```c
#include "facil.h" // the core library header

// Performed whenever there's pending incoming data on the socket
static void perform_echo(intptr_t uuid, protocol_s * prt) {
  (void)prt;
  char buffer[1024] = {'E', 'c', 'h', 'o', ':', ' '};
  ssize_t len;
  while ((len = sock_read(uuid, buffer + 6, 1018)) > 0) {
    sock_write(uuid, buffer, len + 6);
    if ((buffer[6] | 32) == 'b' && (buffer[7] | 32) == 'y' &&
        (buffer[8] | 32) == 'e') {
      sock_write(uuid, "Goodbye.\n", 9);
      sock_close(uuid); // closes after `write` had completed.
      return;
    }
  }
}
// performed whenever "timeout" is reached.
static void echo_ping(intptr_t uuid, protocol_s * prt) {
  (void)prt;
  sock_write(uuid, "Server: Are you there?\n", 23);
}
// performed during server shutdown, before closing the socket.
static void echo_on_shutdown(intptr_t uuid, protocol_s *prt) {
  (void)prt;
  sock_write(uuid, "Echo server shutting down\nGoodbye.\n", 35);
}
// performed after the socket was closed and the currently running task had
// completed.
static void destroy_echo_protocol(intptr_t old_uuid, protocol_s * echo_proto) {
  if (echo_proto) // always error check, even if it isn't needed.
    free(echo_proto);
  fprintf(stderr, "Freed Echo protocol at %p\n", (void * )echo_proto);
  (void)old_uuid;
}
// performed whenever a new connection is accepted.
static inline protocol_s * create_echo_protocol(intptr_t uuid, void * arg) {
  // create a protocol object
  protocol_s * cho_proto = malloc(sizeof( * echo_proto));
  // set the callbacks
  * echo_proto = (protocol_s){
      .service = "echo",
      .on_data = perform_echo,
      .on_shutdown = echo_on_shutdown,
      .ping = echo_ping,
      .on_close = destroy_echo_protocol,
  };
  // write data to the socket and set timeout
  sock_write(uuid, "Echo Service: Welcome. Say \"bye\" to disconnect.\n", 48);
  facil_set_timeout(uuid, 10);
  // print log
  fprintf(stderr, "New Echo connection %p for socket UUID %p\n",
          (void * )echo_proto, (void * )uuid);
  // return the protocol object to attach it to the socket.
  return echo_proto;
  (void)arg; // we don't use this
}
// creates and runs the server
int main(void) {
  // listens on port 3000 for echo services.
  facil_listen(.port = "3000", .on_open = create_echo_protocol);
  // starts and runs the server
  facil_run(.threads = 10);
  return 0;
}
```

---

## SSL/TLS?

Although encryption is important, separating the encryption layer from the application layer is often preferred and more effective.

The idea behind "separation of concerns" is valid between programs, not only between code modules.

It is recommended to achieve SSL/TLS using a separate encryption layer, such as proxy (i.e. nginx, apache, etc') or an encryption tunnel (i.e., spiped etc').

Since facil.io supports unix domain sockets, these solutions could be applied safely to both server and client applications.

However, if you need to expose the application directly to the web or insist on integrating encryption within the app itself, it is possible to implement SSL/TLS support using `sock`'s read/write hooks.

Using `sock`'s read-write hooks (`sock_rw_hook_set`) it's possible to attach any SSL/TLS library. Use `sock_uuid2fd` to convert a connection's UUID to it's system assigned `fd` when the SSL/TLS library needs the information.

---

## A word about concurrency

It should be notes that network applications always have to keep concurrency in mind. For instance, the connection might be closed by one machine while the other is still preparing (or writing) it's response.

Worst, when a slow response is being prepared for a disconnected client (a really slow response, so this isn't common), a new client might connect to the system with the newly available (same) file descriptor, so the finalized response might get sent to the wrong client!

the `sock` library and `facil.io` protect us from such scenarios.

If you will use `facil.io`'s multi-threading mode, it's concurrency will be limited to two types of tasks - writing tasks (`on_ready`, `ping` and `on_shutdown` callbacks) and potentially mutable tasks (i.e. `on_data`, or most tasks scheduled with `facil_defer`).

Each task "family" uses it's own lock, so no two tasks can run concurrently for the same connection. This also means that the "writing" tasks should avoid using/setting any protocol specific information that is intended to be mutable, or collisions might ensue.

In addition to multi-threading, `facil.io` allows us to easily setup the network service's concurrency using processes (`fork`ing), which act differently then threads (i.e. memory space isn't shared, so that processes don't share accepted connections).

For best results, assume everything could run concurrently. `facil.io` will do it's best to prevent collisions, but it is a generic library, so it might not know what to expect from your specific application.
