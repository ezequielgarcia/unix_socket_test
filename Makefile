CC=gcc
CFLAGS=-Wall

all: server client

server: server.c
	$(CC) $(CFLAGS) $^ -o $@

client: client.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f server client
