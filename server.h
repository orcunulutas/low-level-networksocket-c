#ifndef SERVER_H
#define SERVER_H


#include <stdint.h>
#include <stddef.h>
#include "buffer.h"
#include <pool.h>

typedef enum {
    STATE_REQ,
    STATE_RES,
    STATE_END,
} ConnState;

typedef Conn {
   int fd;
   ConnState state;
   struct Buffer req_buffer;
    struct Buffer res_buffer;
}

struct Server {
   int listen_fd;
   struct pullfd *pool_args;
   struct Conn **conns;
   size_t count;
   size_t capacity;
}

