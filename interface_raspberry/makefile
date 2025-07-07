interface_raspberry: interface_raspberry.cpp status.h status.cpp
	g++ -c status.cpp -o status.o -lwiringPi
	g++ interface_raspberry.cpp status.o -o interface_raspberry -lwiringPi