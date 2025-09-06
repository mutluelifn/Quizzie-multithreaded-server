CC=gcc
CFLAGS=-g -O1 -Wall
LDLIBS=-lpthread

all: client server


client: client.c csapp.c
	$(CC) $(CFLAGS) -o client client.c csapp.c $(LDLIBS)

server: server.c csapp.c
	$(CC) $(CFLAGS) -o server server.c csapp.c $(LDLIBS)

clean:
	rm -f *.o *~ *.exe client server csapp.o

