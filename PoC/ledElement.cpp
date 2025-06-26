#include "ledElement.h"

LedElement::LedElement(Adafruit_NeoPixel* p, int o, int n) : pixels(p), offset(o), num_pixels(n)
{
  clearPixels();
}

uint32_t LedElement::generateColor(int r, int g, int b)
{
  return pixels->Color(r > 255 ? 255 : r, g > 255 ? 255 : g, b > 255 ? 255 : b);
}


int LedElement::convertIdx(int idx)
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

void LedElement::lightPixel(int idx, uint32_t color)
{
	pixels->setPixelColor(offset + idx, color);
}


void LedElement::lightSolid(uint32_t color)
{
  for(int i = 0; i < num_pixels; i++)
  {
    lightPixel(i, color);
  }
  show();
}

void LedElement::clearPixels()
{
	lightSolid(generateColor(0,0,0));
}

void LedElement::show()
{
  pixels->show();
}
 
