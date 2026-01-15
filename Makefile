CC=gcc
CFLAGS= -Wall -I.

all: test_run

buffer.o : buffer.c buffer.h
	$(CC) $(CFLAGS) -c buffer.c -o buffer.o


test_run: tests/test_buffer.c buffer.o
	$(CC) $(CFLAGS) tests/test_buffer.c buffer.o -o test_run

clean:
	rm -f *.o test_run
