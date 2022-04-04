CFLAGS=-Wall -Wpedantic -Wextra -g

all: bin/child bin/main 

test: bin/main benchmark

valgrind_verify: bin/main
	valgrind --leak-check=full --show-leak-kinds=all ./bin/main test0 aaa 5
	valgrind --leak-check=full --show-leak-kinds=all ./bin/child test0 0 5 aaa


benchmark: bin/main
	./bin/main test0 aaa 5
	./bin/main test0 bbb 1
	./bin/main test0 bbb 5

bin/main: main.c bin/child
	gcc $(CFLAGS) main.c -o bin/main

bin/child: child.c
	gcc $(CFLAGS) child.c -o bin/child

clean:
	rm *.o
