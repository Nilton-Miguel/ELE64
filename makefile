compile:
	gcc -c efeitos.c -o efeitos.o
	ar rcs libprocoffee.a efeitos.o
	gcc main.c -L. -lprocoffee -lfftw3 -lsndfile -lm -o main
run:
	./main