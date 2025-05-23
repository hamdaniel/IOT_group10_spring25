#ifndef LEDMATRIX_H
#define LEDMATRIX_H

#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define MATRIX_PIN        13 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 256
#define COL_LEN 16


class LedMatrix
{
	Adafruit_NeoPixel pixels;
	uint32_t text_color;



	struct IdleAnimationState {
		bool active = false;
		int phase = 0; // 0 = display, 1 = clear
		unsigned long lastUpdate = 0;
	};

	int convertIdx(int idx);

	IdleAnimationState idle_anim;

	
	public:
	LedMatrix();
	~LedMatrix() = default;
	void lightPixel(int idx, uint32_t color);
	
	
	
	void clearPixels();
	void show();
	
	void startIdleAnimation();
	void stopIdleAnimation();
	void updateIdleAnimation();
	
	uint32_t generateColor(int r, int g, int b);
	
};

#endif //LEDMATRIX