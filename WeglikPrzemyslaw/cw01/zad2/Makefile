static: copy_static main.o static_main clean

copy_static:
	cp ../zad1/alloc_library.c lib
	cp ../zad1/out/liballoc_library.a lib

main.o: main.c
	gcc -c main.c -o main.o

static_main: main.o lib/liballoc_library.a
	gcc main.o -l alloc_library -L ./lib -o bin/static_main

shared: copy_shared shared_main

copy_shared:
	cp ../zad1/alloc_library.c lib
	cp ../zad1/out/liballoc_library.so lib

shared_main: lib/liballoc_library.so
	gcc main.c -lalloc_library -L ./lib -o bin/shared_main

clean:
	rm *.o

# might be shared_main as well
test_simple: bin/static_main
	cat test_cases/test_simple | bin/static_main

test_final: bin/static_main
	cat test_cases/test_final | bin/static_main