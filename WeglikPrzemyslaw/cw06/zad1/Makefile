CFLAGS=-Wall -Wpedantic -Wextra -g

all: bin/client bin/server 

bin/client: client.c utils.h
	gcc $(CFLAGS) client.c -o bin/client

bin/server: server.c utils.h
	gcc $(CFLAGS) server.c -o bin/server