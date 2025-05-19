compile:
	gcc -c geradores.c -o geradores.o
	gcc -c efeitos.c -o efeitos.o
	ar rcs libprocoffee.a geradores.o efeitos.o
	gcc main.c -L. -lprocoffee -lfftw3 -lm -o main
run:
	./main
