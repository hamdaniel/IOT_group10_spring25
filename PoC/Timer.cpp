#include "Timer.h"

Timer::Timer() : display(CLK, DIO)
{
    active = false;
    lastUpdateTime = 0;
    lastBlinkTime = 0;
    blinking = false;
    blinkCount = 0;
    blinkState = false;
    display.setBrightness(0x0a);
    display.setSegments(blank);
    //display.showNumberDec(secLeft, true);
    
}
Timer::~Timer()
{
    // Destructor
}

void Timer::start(String time)
{
    active = true;
    lastUpdateTime = millis();
    // timeToElapse = (time[2] -'0') +  ((time[1] -'0')  * 10) + ((time[2] -'0') * 100);
    secLeft = (time[2] -'0') +  ((time[1] -'0')  * 10) + ((time[0] -'0') * 100);
}
void Timer::update()
{
    if(!active)
    {
        return;
    }
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
                    display.setSegments(blank);
                }
                // Stop blinking after five blinks
                if (blinkCount >= 5) {
                    blinking = false; // Stop blinking
                    display.setSegments(blank);
                    active = false;
                }
            }
        }
    }
}

bool Timer::timeIsUp()
{
    return blinking;
}

void Timer::reset()
{
    active = false;
    display.setSegments(blank);   
}

