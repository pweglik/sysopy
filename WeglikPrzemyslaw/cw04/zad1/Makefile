CFLAGS=-Wall -Wpedantic -Wextra -g

all: bin/main

test: bin/main valgrind_verify benchmark

valgrind_verify: bin/main
	valgrind ./bin/main ignore exec
	valgrind ./bin/main pending exec
	valgrind ./bin/main mask exec
	valgrind ./bin/main handler exec

run: bin/main
	./bin/main ignore exec
	./bin/main pending exec
	./bin/main mask exec
	./bin/main handler exec
	./bin/main ignore fork
	./bin/main pending fork
	./bin/main mask fork
	./bin/main handler fork

bin/main: main.c bin/child
	gcc $(CFLAGS) main.c -o bin/main

bin/child: child.c
	gcc $(CFLAGS) child.c -o bin/child

clean:
	rm *.o
