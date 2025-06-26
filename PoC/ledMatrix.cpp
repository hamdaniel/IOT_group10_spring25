#include "ledMatrix.h"

LedMatrix::LedMatrix(Adafruit_NeoPixel* p, int o, int n) : LedElement(p,o,n) {} 


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
	pixels->setPixelColor(offset + convertIdx(idx), color);
}
