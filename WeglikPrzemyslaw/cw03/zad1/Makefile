CFLAGS=-Wall -Wpedantic -Wextra -g

all: bin/main

test: bin/main valgrind_verify benchmark

valgrind_verify: bin/main
	valgrind ./bin/main 1000

benchmark: bin/main
	./bin/main 1000

bin/main: main.c bin/child
	gcc $(CFLAGS) main.c -o bin/main

bin/child: child.c
	gcc $(CFLAGS) child.c -o bin/child

clean:
	rm *.o
