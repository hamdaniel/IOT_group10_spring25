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
}

void LedMatrix::show()
{
  pixels.show();
}

void LedMatrix::winAnimation()
{
  clearPixels();
  show();

  const int centerX = (NUMPIXELS / COL_LEN) / 2;
  const int centerY = COL_LEN / 2;
  const int steps = COL_LEN;  // number of animation steps (waves)
  const int reps = 30;

  for(int i = 0; i < reps; i ++)
  {

    for (int r = 0; r < steps; r++)
    {
      for (int y = 0; y < 16; y++)
      {
        for (int x = 0; x < 16; x++)
        {
          // Manhattan distance as a stand-in for radius
          int dx = x - centerX;
          int dy = y - centerY;
          int dist = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
          
          if (dist == r)
          {
            // Simple hue-like color variation using the radius
            int red = (r * 20) % 256;
            int green = (255 - r * 10) % 256;
            int blue = (r * 40) % 256;
            lightPixel(y * 16 + x, generateColor(red, green, blue));
          }
        }
      }
      show();
      delay(1);
    }
    
    // Fade out
    for (int fade = 255; fade >= 0; fade -= 25)
    {
      for (int y = 0; y < 16; y++)
      {
        for (int x = 0; x < 16; x++)
        {
          int idx = y * 16 + x;
          uint32_t color = pixels.getPixelColor(convertIdx(idx));
          uint8_t r = (color >> 16) & 0xFF;
          uint8_t g = (color >> 8) & 0xFF;
          uint8_t b = color & 0xFF;
          
          lightPixel(idx, generateColor((r * fade) / 255, (g * fade) / 255, (b * fade) / 255));
        }
      }
      show();
      delay(1);
    }
    // delay(500);
  }
}

uint32_t LedMatrix::generateColor(int r, int g, int b)
{
  return pixels.Color(r > 255 ? 255 : r, g > 255 ? 255 : g, b > 255 ? 255 : b);
}

  
void LedMatrix::idleAnimation()
{
  
  for(int y = 2; y < 13; y++)
  {
    if(y == 7)
    {
      continue;
    }
    for(int x = 1; x < 16; x++)
    {
      if(x == 4 || x == 8 || x == 12 || (x >= 8 && y < 7))
      {
        continue;
      }
      if(x == 2 && (( y >=3 && y <= 6 ) || y == 8 || (y >= 10 && y <= 12)))
      {
        continue;
      }
      if(x == 6 && ((y >=3 && y <= 5) || y == 9 || (y >= 11 && y <= 12)))
      {
        continue;
      }
      if(x == 9 && ((y >=9 && y <= 10)))
      {
        continue;
      }
      if(x == 10 && (y == 9 || y == 11) )
      {
        continue;
      }
      if(x == 11 && ((y >=10 && y <= 11)))
      {
        continue;
      }
      if(x == 14 && (y == 9 || y == 11) )
      {
        continue;
      }
      if(x == 15 && (y == 9 || y == 11) )
      {
        continue;
      }
      lightPixel(x + 16*y, text_color);
    }
  }  
  show();
  delay(3000);
  clearPixels();
  show();
  delay(2000);
}
