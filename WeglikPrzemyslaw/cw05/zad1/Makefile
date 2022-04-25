CFLAGS=-Wall -Wpedantic -Wextra -g

all: bin/main 

test: bin/main
	bin/main test.txt


bin/main: main.c
	gcc $(CFLAGS) main.c -o bin/main
