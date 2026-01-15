#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <stddef.h>


typedef struct {
   uint8_t *data;
   size_t size;
   size_t capacity;
}Buffer;


void buffer_init(Buffer *buf);
void buffer_free(Buffer *buf);
int buffer_append(Buffer *buf, const uint8_t *data, size_t data_size);

#endif // BUFFER_H
