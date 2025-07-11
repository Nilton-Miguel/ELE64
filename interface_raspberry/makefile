interface_raspberry: interface_raspberry.cpp status.o display.o
	g++ interface_raspberry.cpp status.o display.o -o interface_raspberry -L. -lwiringPi -lcairo -lX11
	cp interface_raspberry ~/procoffee_interface

status.o: status.cpp status.h
	g++ -c status.cpp -o status.o -lwiringPi

display.o: display.c display.h
	g++ -c display.c -o display.o -lcairo -lX11

clean:
	rm status.o display.o interface_raspberry
