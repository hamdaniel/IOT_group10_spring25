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
	int convertIdx(int idx);
	public:
	LedMatrix();
	~LedMatrix() = default;
	void initMatrix();
	void lightPixel(int idx, uint32_t color);
	void winAnimation();
	void clearPixels();
	void show();

	uint32_t generateColor(int r, int g, int b);
	
};

#endif //LEDMATRIX