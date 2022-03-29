CFLAGS=-Wall -Wpedantic -Wextra

all: bin/main

test: bin/main valgrind_verify benchmark
	

valgrind_verify: bin/main
	valgrind ./bin/main 1000

benchmark: bin/main
	./bin/main 1000


bin/main: main.c
	gcc $(CFLAGS) main.c -o bin/main

clean:
	rm *.o
