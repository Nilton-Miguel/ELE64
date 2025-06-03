compile:
	gcc -c efeitos.c -o efeitos.o
	gcc -c interface.c -o interface.o
	ar rcs libprocoffee.a efeitos.o interface.o
	gcc main.c -L. -lprocoffee -lfftw3 -lsndfile -lm -o main
run:
	clear
	./main