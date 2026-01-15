#include <stdio.h>
#include <string.h>
#include "server.h"


void server_init (struct Server *server, int listen_fd) {
   server->listen_fd = listen_fd;
   server->count=0;
   server->capacity=4;
   
   server->pool_args = malloc(sizeof(struct pollfd)*server->capacity);
   server->conns = malloc (sizeof(struct Conn*)*server->capacity);

   if (!server->pool_args || server->conns ) {
        perror ("malloc failed");
        exit (EXIT_FAILURE);
     }

  for (size_t i=0; i< server->capacity; i++) {
      server->conns[i] = NULL;
      server->pool_args[i].fd = -1;
      server->pool_args[i].events = 0;
      server->pool_args[i].revents =0;
  }
  server->pool_args[0].fd = listen_fd;
  server->pool_args[0].events = POLLIN;
  server->pool_args[0].revents = 0;

  server->count=1;

}


void server_add_connection (struct Server *server,int client_fd) {
     if (server-> count == server->capacity) {
       size_t new_cap = server->capacity*2;
       struct pollfd * new_pool = realloc(server->pool_args,sizeof(struct pollfd)*new_cap);
       struct Conn ** new_conns = realloc(server->conns,sizeof(struct Conn*)*new_cap);
       if (!new_pool || !new_conns) {
              perror ("realloc failed");
                exit (EXIT_FAILURE);
       }
       server->pool_args= new_pool;
         server->conns = new_conns;
         for (size_t i=server->capacity;i<new_cap;i++) {
                server->conns[i] = NULL;
                server->pool_args[i].fd = -1;
                server->pool_args[i].events =0;
                server->pool_args[i].revents=0;
         }
            server->capacity = new_cap;
     }
       server->pool_args[server->count].fd = client_fd;
       server->pool_args[server->count].events = POLLIN;
       server->pool_args[server->count].revents =0;


       struct Conn *new_con = malloc(sizeof(struct Conn));
       if (!new_con) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
       }
       new_con->fd=client_fd;
       new_con->state= STATE_REQ;
       buffer_init(&new_con->req_buffer);
       buffer_init(&new_con->res_buffer);
       server->conns[server->count] = new_con;
         server->count +=1;

}
