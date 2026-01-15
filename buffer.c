#include "buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>




// hersey initializer seviyesinde set ediliyor
void buffer_init(Buffer *buf) {
    buf->data = NULL;
    buf->size = 0;
    buf->capacity = 0;
}


int buffer_append(Buffer *buf, const uint8_t *data, size_t data_size) {
     if (buf->size + data_size > buf->capacity) {
         size_t new_cap = (buf->capacity ==0) ? 1024 : buf->capacity*2;
         while (new_cap< buf->size + data_size ) {
                new_cap *=2;
         }
         uint8_t *new_ptr = realloc(buf->data,new_cap);
         if (!new_ptr) {
             perror("realloc failed");
             exit(EXIT_FAILURE);
         }
         buf->data = new_ptr;
         buf->capacity = new_cap;
     }
     memcpy(buf->data + buf->size, data, data_size);
     buf->size += data_size;
    return 0;
}

void buffer_free (Buffer *buf) {
  free(buf->data);
  buf->data = NULL;
  buf->size = 0;
  buf->capacity = 0;
}


