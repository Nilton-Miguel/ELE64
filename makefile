compile:
	gcc -c geradores.c -o geradores.o
	ar rcs libgeradores.a geradores.o
	gcc main.c -L. -lgeradores -lfftw3 -lm -o main
run:
	./main
