compile:
	gcc -c efeitos.c -o efeitos.o
	gcc -c interface.c -o interface.o
	ar rcs libprocoffee.a efeitos.o interface.o
	gcc main.c -L. -lprocoffee -lsndfile -lm -lasound -o main
	cp ./main ~/procoffee_processing

run:
	./main

write:
	gcc -c efeitos.c -o efeitos.o
	gcc -c interface.c -o interface.o
	ar rcs libprocoffee.a efeitos.o interface.o
	gcc write.c -L. -lprocoffee -lm -o write
	./write
