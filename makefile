compile:
	gcc -c geradores.c -o geradores.o
	ar rcs libgeradores.a geradores.o
	gcc main.c -L. -lgeradores -lm -lfftw3 -o main
run:
	./main
