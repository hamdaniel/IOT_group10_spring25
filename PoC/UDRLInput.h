#ifndef UDRLINPUT_H
#define UDRLINPUT_H

#include <stdbool.h>
#include <Arduino.h>

#define U_PIN             23
#define D_PIN             22
#define R_PIN             25
#define L_PIN             33


class UDRLInput{
	bool prevButtonState[4]; //UDRL
	bool currButtonState[4]; //UDRL
	int buttonPins[4]; //UDRL

	public:
	UDRLInput();
	~UDRLInput() = default;
	
	void readInput();
	bool moved(int i);
};

#endif // UDRLINPUT_H