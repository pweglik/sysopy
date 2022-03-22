static: copy_static ../zad2/main.o ../zad2/static_main test_static clean

copy_static:
	cp ../zad1/alloc_library.c ../zad2/lib
	cp ../zad1/out/liballoc_library.a ../zad2/lib

../zad2/main.o: ../zad2/main.c
	gcc $(OPTIMIZATION) -c ../zad2/main.c -o ../zad2/main.o

../zad2/static_main: ../zad2/main.o ../zad2/lib/liballoc_library.a
	gcc $(OPTIMIZATION) ../zad2/main.o -l alloc_library -L ../zad2/lib -o ../zad2/bin/static_main

test_static: ../zad2/bin/static_main
	cat test_cases/test_final | ../zad2/bin/static_main

shared: ../zad2/copy_shared ../zad2/shared_main test_shared clean

../zad2/copy_shared:
	cp ../zad1/alloc_library.c ../zad2/lib
	cp ../zad1/out/liballoc_library.so ../zad2/lib

../zad2/shared_main: ../zad2/lib/liballoc_library.so
	gcc $(OPTIMIZATION) ../zad2/main.c -lalloc_library -L ../zad2/lib -o ../zad2/bin/shared_main

test_shared: ../zad2/bin/shared_main
	cat test_cases/test_final | ../zad2/bin/shared_main

dynamic: ../zad2/bin/dynamic_main test_dynamic clean

../zad2/bin/dynamic_main:
	gcc $(OPTIMIZATION) ../zad2/main.c -ldl -D DYNAMIC_LOADING -o ../zad2/bin/dynamic_main

test_dynamic: ../zad2/bin/dynamic_main
	cat test_cases/test_final | ../zad2/bin/dynamic_main

clean:
	rm ../zad2/*.o


test:

	for lib in static shared dynamic ; do \
		for o in O1 O2 O3; do \
			printf "$$lib $$o\n" ; \
			make $$lib OPTIMIZATION=-$$o; \
		done ; \
	done
