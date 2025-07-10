interface_raspberry: interface_raspberry.cpp status.h status.cpp display.c display.h
	g++ -c display.c -o display.o -lcairo -lX11
	g++ -c status.cpp -o status.o -lwiringPi
	g++ interface_raspberry.cpp status.o display.o -o interface_raspberry -L. -lwiringPi -lcairo -lX11
	cp interface_raspberry ~/procoffee_interface
