CFLAGS=-Wall -Wpedantic -Wextra

all: bin/main_stat bin/main_nftw

test: bin/main_stat bin/main_nftw data/benchmark_input valgrind_verify
	

valgrind_verify: ./bin/main_stat ./bin/main_nftw data/input
	valgrind ./bin/main_stat testdir
	valgrind ./bin/main_nftw testdir

bin/main_stat: main_stat.c
	gcc $(CFLAGS) main_stat.c -o bin/main_stat

bin/main_nftw: main_nftw.c
	gcc $(CFLAGS) main_nftw.c -o bin/main_nftw

clean:
	rm *.o
