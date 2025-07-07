#include <iostream>
#include <wiringPi.h>

int main() {
	wiringPiSetupPinType(WPI_PIN_WPI);

	pinMode(9, INPUT);
	//pullUpDnControl(8, PUD_UP);

	while(1) {
		std::cout << digitalRead(9) << "\n";
	}
}