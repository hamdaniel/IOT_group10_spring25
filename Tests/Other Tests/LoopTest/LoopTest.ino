#include "Timer.h"

Timer* timer = nullptr;

void setup()
{
  timer = new Timer();
  timer->start(10);
}

void loop()
{
    timer->changeDisplay();
}