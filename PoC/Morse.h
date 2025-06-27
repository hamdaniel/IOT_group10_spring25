#ifndef MORSE_H
#define MORSE_H

#include <stdbool.h>
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h> 

#include "ledElement.h"
#include "sound.h"
#include "puzzle.h"
#include "BigBtn.h"

#define MORSE_LED_PIN 4

class Morse : public Puzzle {
	private:
		enum class MorseState {
		SHOW_MORSE,
		WAIT_INPUT,
		END_ANIMATION,
		DONE
		};


		BigBtn* btn;
		String orig_word;
		char permutation[4];
		int target_val;
		
		MorseState state;
		String scrambled;
		int currentLetterIdx;
		String currentMorse;
		int currentSymbolIdx ;
		unsigned long lastActionTime;
		bool symbolOn;

		void showInMorse(char c);

	public:
		Morse(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, BigBtn* bb, String ow);
		~Morse() override {};

		void play() override;
		void endAnimation() override {};
};



#endif // MORSE_H