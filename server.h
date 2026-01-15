#ifndef SERVER_H
#define SERVER_H


#include <stdint.h>
#include <stddef.h>
#include "buffer.h"
#include <poll.h>

typedef enum {
    STATE_REQ,
    STATE_RES,
    STATE_END,
} ConnState;

typedef struct Conn {
   int fd;
   ConnState state;
   Buffer req_buffer;
   Buffer res_buffer;
} Conn;

typedef struct Server {
   int listen_fd;
   struct pollfd *pool_args;
   struct Conn **conns;
   size_t count;
   size_t capacity;
} Server;

void server_init(Server *server, int listen_fd);
void server_add_connection(Server *server, int client_fd);
void server_run(Server *server);

#endif // SERVER_H
