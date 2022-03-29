CFLAGS=-Wall -Wpedantic -Wextra -std=c18 -g

all: bin/main

test: bin/main valgrind_verify benchmark

valgrind_verify: bin/main
	valgrind ./bin/main 0.001 10

benchmark: bin/main
	./bin/main 0.001 100
	./bin/main 0.000001 100
	./bin/main 0.000001 1000
	./bin/main 0.000001 10000

bin/main: main.c bin/child
	gcc $(CFLAGS) main.c -o bin/main

bin/child: child.c
	gcc $(CFLAGS) child.c -o bin/child

clean:
	rm *.o
