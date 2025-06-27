#include "ledElement.h"

LedElement::LedElement(Adafruit_NeoPixel* p, int o, int n) : pixels(p), offset(o), num_pixels(n)
{
  clearPixels();
}

uint32_t LedElement::generateColor(int r, int g, int b)
{
  return pixels->Color(
    r > 255 ? 255 : r,
    g > 255 ? 255 : g,
    b > 255 ? 255 : b
  );
}

void LedElement::lightPixel(int idx, uint32_t color)
{
	pixels->setPixelColor(offset + idx, color);
}

void LedElement::lightFraction(double fraction, uint32_t color)
{
  clearPixels();
  int num_to_light = (int)(num_pixels * fraction + 0.999);

  if (num_to_light < 0)
    num_to_light = 0;
  if (num_to_light > num_pixels)
    num_to_light = num_pixels;

  for (int i = 0; i < num_to_light; i++)
    lightPixel(i, color);
  
}

void LedElement::lightSolid(uint32_t color)
{
  for(int i = 0; i < num_pixels; i++)
  {
    lightPixel(i, color);
  }  
}

int LedElement::getNumPixels() const
{
  return num_pixels;
}
void LedElement::clearPixels()
{
	lightSolid(generateColor(0,0,0));
}

