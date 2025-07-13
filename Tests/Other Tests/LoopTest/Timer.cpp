#include "Timer.h"

Timer::Timer() : display(CLK, DIO)
{
    lastUpdateTime = 0;
    lastBlinkTime = 0;
    blinking = false;
    blinkCount = 0;
    blinkState = false;
    display.setBrightness(0x0f);
    //display.showNumberDec(secLeft, true);
    
}
Timer::~Timer()
{
    // Destructor
}

void Timer::start(int time)
{
    lastUpdateTime = millis();
    timeToElapse = time;
    secLeft = timeToElapse;
}
void Timer::changeDisplay()
{
    unsigned long currentMillis = millis();
    if(currentMillis - lastUpdateTime >= TEST_DELAY && (lastUpdateTime!=0))
    {
        if (!blinking) {
            // Countdown logic
            if (secLeft >= 0) {
                display.showNumberDec(secLeft, true);
                lastUpdateTime = currentMillis;
                secLeft--;
            }
            // Start blinking when countdown ends
            if (secLeft < 0) {
                blinking = true;
                blinkCount = 0;
                blinkState = false;
                lastBlinkTime = currentMillis;
            }
        } else {
            // Blinking logic: blink zero three times
            if (currentMillis - lastBlinkTime >= TEST_DELAY) {
                blinkState = !blinkState;
                lastBlinkTime = currentMillis;
                if (blinkState) {
                    display.showNumberDec(0, true);
                    blinkCount++;
                } else {
                    display.clear();
                    display.show();
                }
                // Stop blinking after five blinks
                if (blinkCount >= 5) {
                    blinking = false; // Stop blinking
                    secLeft = timeToElapse; // Reset countdown
                    display.clear();
                    display.show();
                }
            }
        }
    }
}