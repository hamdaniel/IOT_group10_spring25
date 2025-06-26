#include "ledMatrix.h"

LedMatrix::LedMatrix() : pixels(NUMPIXELS, MATRIX_PIN, NEO_GRB + NEO_KHZ800)
{
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels.begin();
  pixels.clear();
  pixels.show();

	text_color = generateColor(10,5,0);
}

uint32_t LedMatrix::generateColor(int r, int g, int b)
{
  return pixels.Color(r > 255 ? 255 : r, g > 255 ? 255 : g, b > 255 ? 255 : b);
}


int LedMatrix::convertIdx(int idx)
{
  // return (num % 16) * COL_LEN + (((num % 16) % 2 )? (COL_LEN - num / 16 - 1) : num / 16);
  int x = idx % COL_LEN;
  int y = idx / COL_LEN;
  if(x % 2)
  {
    return x * COL_LEN + COL_LEN - y - 1;
  }

  return x * COL_LEN + y;
}

void LedMatrix::lightPixel(int idx, uint32_t color)
{
	pixels.setPixelColor(convertIdx(idx), color);
}

void LedMatrix::clearPixels()
{
	pixels.clear();
  pixels.show();
}

void LedMatrix::show()
{
  pixels.show();
}
 
