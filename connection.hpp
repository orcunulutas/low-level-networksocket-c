#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <vector>
#include <cstdint>

struct Conn {
    int fd =-1;
    bool want_read = false;
    bool want_write = false;
    std::vector<uint8_t> read_buffer;
    std::vector<uint8_t> write_buffer;
};

#endif // CONNECTION_HPP
