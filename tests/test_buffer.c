#include "../buffer.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>



void test_buffer_init() {
  Buffer b;
  buffer_init(&b);
  assert(b.data == NULL);
  assert(b.size == 0);
  assert(b.capacity == 0);
    printf("test_buffer_init passed\n");
}


void test_buffer_append () {
    Buffer b;
    buffer_init(&b);
    char *msg="hello world";
    buffer_append(&b,(uint8_t*)msg,strlen(msg));
    assert(b.size = strlen(msg));
    assert(b.capacity >= b.size);
    assert(memcmp(b.data,msg,b.size) ==0);
    buffer_free(&b);
    printf("test_buffer_append passed\n");
}

int main () {
   printf("Running buffer tests...\n");
    test_buffer_init();
    test_buffer_append();
    printf("All buffer tests passed!\n");

  return 0;
}
