CFLAGS=-Wall -Wpedantic -Wextra

all: bin/main_lib bin/main_sys

test: bin/main_lib bin/main_sys data/benchmark_input valgrind_verify benchmark
	

valgrind_verify: ./bin/main_lib ./bin/main_sys data/input
	valgrind ./bin/main_lib c data/input
	valgrind ./bin/main_sys c data/input

benchmark: bin/main_lib bin/main_sys data/benchmark_input
	./bin/main_lib a data/benchmark_input
	./bin/main_sys a data/benchmark_input

bin/main_lib: main_lib.c
	gcc $(CFLAGS) main_lib.c -o bin/main_lib

bin/main_sys: main_sys.c
	gcc $(CFLAGS) main_sys.c -o bin/main_sys

clean:
	rm *.o
