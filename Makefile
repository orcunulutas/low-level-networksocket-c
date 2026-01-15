CC=gcc
CFLAGS= -Wall -I.

all: test_run server

buffer.o : buffer.c buffer.h
	$(CC) $(CFLAGS) -c buffer.c -o buffer.o

server.o : server.c server.h
	$(CC) $(CFLAGS) -c server.c -o server.o

main.o : main.c server.h
	$(CC) $(CFLAGS) -c main.c -o main.o

server: main.o server.o buffer.o
	$(CC) $(CFLAGS) main.o server.o buffer.o -o server

test_run: tests/test_buffer.c buffer.o
	$(CC) $(CFLAGS) tests/test_buffer.c buffer.o -o test_run

clean:
	rm -f *.o test_run server
