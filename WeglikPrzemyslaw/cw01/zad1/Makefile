static: alloc_library.o liballoc_library.a clean

alloc_library.o: alloc_library.c
	gcc -c alloc_library.c

liballoc_library.a: alloc_library.o
	ar rcs out/liballoc_library.a alloc_library.o


shared: alloc_library_shared.o liballoc_library.so clean

alloc_library_shared.o: alloc_library.c
	gcc -fPIC -c alloc_library.c -o alloc_library_shared.o

liballoc_library.so: alloc_library.o
	gcc -shared -fPIC -o out/liballoc_library.so alloc_library_shared.o


clean:
	rm *.o