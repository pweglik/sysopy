CFLAGS=-Wall -Wpedantic -Wextra

all: bin/main_lib bin/main_sys

test: bin/main_lib bin/main_sys  data/benchmark_input
	

valgrind_verify:
	valgrind ./bin/main_lib data/input data/output 
	valgrind ./bin/main_lib data/input data/output 

benchmark: bin/main_lib bin/main_sys data/benchmark_input
	./bin/main_lib data/benchmark_input data/benchmark_output 
	./bin/main_sys data/benchmark_input data/benchmark_output 

bin/main_lib: main_lib.c
	gcc $(CFLAGS) main_lib.c -o bin/main_lib

bin/main_sys: main_sys.c
	gcc $(CFLAGS) main_sys.c -o bin/main_sys

clean:
	rm *.o
