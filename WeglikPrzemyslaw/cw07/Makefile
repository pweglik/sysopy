CFLAGS=-Wall -Wpedantic -Wextra -g

all: build test clean

CC=gcc -Wall

all: build test clean

build:
	$(CC) -o main main.c
	$(CC) -o cook cook.c shared.c
	$(CC) -o driver driver.c shared.c

test:
	./main

clean:
	rm -f main cook driver