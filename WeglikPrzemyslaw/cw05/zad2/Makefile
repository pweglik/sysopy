CFLAGS=-Wall -Wpedantic -Wextra -g

all: bin/main 

test: bin/main
	bin/main przemek title hello_darkness_my_old_friend
	bin/main data


bin/main: main.c
	gcc $(CFLAGS) main.c -o bin/main
