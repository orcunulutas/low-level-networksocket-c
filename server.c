#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "server.h"


void server_init (struct Server *server, int listen_fd) {
   server->listen_fd = listen_fd;
   server->count=0;
   server->capacity=4;
   
   server->pool_args = malloc(sizeof(struct pollfd)*server->capacity);
   server->conns = malloc (sizeof(struct Conn*)*server->capacity);

   if (!server->pool_args || !server->conns ) {
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

void server_run(Server *server) {
    printf("Server running...\n");
    while (1) {
        int poll_count = poll(server->pool_args, server->count, -1);
        if (poll_count == -1) {
            perror("poll");
            break;
        }

        for (size_t i = 0; i < server->count; i++) {
            if (server->pool_args[i].revents & POLLIN) {
                if (server->pool_args[i].fd == server->listen_fd) {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(server->listen_fd, (struct sockaddr*)&client_addr, &client_len);
                    if (client_fd >= 0) {
                        server_add_connection(server, client_fd);
                        printf("New connection: %d\n", client_fd);
                    } else {
                        perror("accept");
                    }
                } else {
                    Conn *conn = server->conns[i];
                    uint8_t buf[1024];
                    ssize_t n = read(conn->fd, buf, sizeof(buf));
                    if (n <= 0) {
                        if (n == 0) {
                            printf("Client %d disconnected\n", conn->fd);
                        } else {
                            perror("read");
                        }
                        close(conn->fd);
                        buffer_free(&conn->req_buffer);
                        buffer_free(&conn->res_buffer);
                        free(conn);

                        server->conns[i] = server->conns[server->count - 1];
                        server->pool_args[i] = server->pool_args[server->count - 1];
                        server->count--;
                        i--;
                        continue;
                    } else {
                         buffer_append(&conn->res_buffer, buf, n);
                         conn->state = STATE_RES;
                         server->pool_args[i].events |= POLLOUT;
                    }
                }
            }

            if (server->pool_args[i].revents & POLLOUT) {
                Conn *conn = server->conns[i];
                if (conn->state == STATE_RES && conn->res_buffer.size > 0) {
                    ssize_t n = write(conn->fd, conn->res_buffer.data, conn->res_buffer.size);
                    if (n < 0) {
                         perror("write");
                         close(conn->fd);
                         buffer_free(&conn->req_buffer);
                         buffer_free(&conn->res_buffer);
                         free(conn);
                         server->conns[i] = server->conns[server->count - 1];
                         server->pool_args[i] = server->pool_args[server->count - 1];
                         server->count--;
                         i--;
                         continue;
                    } else {
                        if ((size_t)n < conn->res_buffer.size) {
                             size_t remaining = conn->res_buffer.size - n;
                             memmove(conn->res_buffer.data, conn->res_buffer.data + n, remaining);
                             conn->res_buffer.size = remaining;
                        } else {
                            conn->res_buffer.size = 0;
                            conn->state = STATE_REQ;
                            server->pool_args[i].events &= ~POLLOUT;
                        }
                    }
                }
            }
        }
    }
}
