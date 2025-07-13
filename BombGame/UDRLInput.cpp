#include "UDRLInput.h"

UDRLInput::UDRLInput()
{
  buttonPins[0] = U_PIN;
  buttonPins[1] = D_PIN;
  buttonPins[2] = R_PIN;
  buttonPins[3] = L_PIN;

  for (int i = 0; i < 4; i++)
  {
    prevButtonState[i] = HIGH;
    pinMode(buttonPins[i], INPUT_PULLUP); 
  }
}


void UDRLInput::readInput()
{
  for (int i = 0; i < 4; i++)
  {
    prevButtonState[i] = currButtonState[i];
    currButtonState[i] = digitalRead(buttonPins[i]);
  }
}

bool UDRLInput::moved(int i)
{
	return currButtonState[i] && !prevButtonState[i];
}