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
 
void LedMatrix::startIdleAnimation() {
  if(idle_anim.active)
  {
    return;
  }
  idle_anim = IdleAnimationState();
  idle_anim.active = true;
  idle_anim.phase = 0;
  idle_anim.lastUpdate = millis();
}

void LedMatrix::updateIdleAnimation() {
  if (!idle_anim.active) return;

  unsigned long now = millis();

  // Phase 0: Show pattern
  if (idle_anim.phase == 0 && now - idle_anim.lastUpdate >= 0) {
    for (int y = 2; y < 13; y++) {
      if (y == 7) continue;
      for (int x = 1; x < 16; x++) {
        if (x == 4 || x == 8 || x == 12 || (x >= 8 && y < 7)) continue;
        if (x == 2 && ((y >= 3 && y <= 6) || y == 8 || (y >= 10 && y <= 12))) continue;
        if (x == 6 && ((y >= 3 && y <= 5) || y == 9 || (y >= 11 && y <= 12))) continue;
        if (x == 9 && (y >= 9 && y <= 10)) continue;
        if (x == 10 && (y == 9 || y == 11)) continue;
        if (x == 11 && (y >= 10 && y <= 11)) continue;
        if (x == 14 && (y == 9 || y == 11)) continue;
        if (x == 15 && (y == 9 || y == 11)) continue;

        lightPixel(x + 16 * y, text_color);
      }
    }
    show();
    idle_anim.phase = 1;
    idle_anim.lastUpdate = now;
  }

  // Phase 1: Wait and clear
  else if (idle_anim.phase == 1 && now - idle_anim.lastUpdate >= 3000) {
    clearPixels();
    idle_anim.phase = 2;
    idle_anim.lastUpdate = now;
  }

  // Phase 2: Done
  else if (idle_anim.phase == 2 && now - idle_anim.lastUpdate >= 1000) {
    idle_anim.active = false;
  }
}

  