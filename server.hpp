#ifndef   SERVER_HPP
#define   SERVER_HPP

#include <vector>
#include <poll.h>
#include "connection.hpp"


class RedisServer {
  public:
      RedisServer(int port);
      void run();
  private:
      int listen_fd;
      std::vector<Conn *> connections;
      std::vector<struct pollfd> poll_fds;
      void handle_new_connection();
      void handle_client_data(Conn* conn);

}



#endif // SERVER_HPP
