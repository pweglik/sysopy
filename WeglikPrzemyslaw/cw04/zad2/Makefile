CFLAGS=-Wall -Wpedantic -Wextra -g

all: bin/main

test: bin/main benchmark

run: bin/main
	./bin/main


bin/main: main.c bin/child
	gcc $(CFLAGS) main.c -o bin/main

clean:
	rm *.o
