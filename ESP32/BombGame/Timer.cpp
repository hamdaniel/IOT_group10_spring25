#include "Timer.h"

Timer::Timer() : active(false), timeToElapse(0),
                 startTime(0), lastUpdateTime(0), lastBlinkTime(0),
                 pausedTimeToElapse(0), paused(false), blinking(false),
                 blinkCount(0), blinkState(false), lastDisplayedSec(-1),
                 taskHandle(nullptr), display(CLK, DIO)
{
    
    display.setBrightness(0x0a);
    display.setSegments(blank);
    
}
Timer::~Timer()
{
    // Destructor
}

void Timer::start(int time)
{
    startTime = millis();
    active = true;
    timeToElapse = time;
    lastDisplayedSec = timeToElapse;
    pausedTimeToElapse = 0;
    paused = false;
    display.showNumberDec(timeToElapse, true, 4);
    lastUpdateTime = 0;

    // If update task is not already running, create it
    if (taskHandle == nullptr) {
        xTaskCreatePinnedToCore(
            Timer::updateWrapper,
            "TimerUpdateTask",
            2048,
            this,
            0, // lowest priority
            &taskHandle,
            0  // core 0
        );
    }
}

void Timer::updateWrapper(void* timer_instance)
{
    Timer* instance = static_cast<Timer*>(timer_instance);

    while (true) {
        instance->update();  // call your method
        vTaskDelay(50 / portTICK_PERIOD_MS);  // ~20Hz polling, change if needed
    }
  
}

void Timer::update()
{
    if (!active || paused)
        return;

    unsigned long currentMillis = millis();

    if (blinking)
    {
        if (currentMillis - lastBlinkTime >= TEST_DELAY)
        {
            blinkState = !blinkState;
            lastBlinkTime = currentMillis;

            if (blinkState)
            {
                display.showNumberDec(0, true, 4);
            }
            else
            {
                blinkCount++;
                display.setSegments(blank);
            }

            if (blinkCount >= 5)
            {
                blinking = false;
                display.setSegments(blank);
                active = false;
            }
        }
    }

    // Check if enough time has passed for an update
    else if (currentMillis - lastUpdateTime >= TEST_DELAY || lastUpdateTime == 0)
    {
        unsigned long elapsed = currentMillis - startTime;
        int remainingSec = ((timeToElapse * 1000 - elapsed) + 999) / 1000;

        if (remainingSec > 0)
        {
            if (remainingSec != lastDisplayedSec)
            {
                display.showNumberDec(remainingSec, true, 4);
                lastDisplayedSec = remainingSec;
            }
            lastUpdateTime = currentMillis;
        }
        else if (!blinking)
        {
            display.showNumberDec(0, true, 4);
            blinking = true;
            blinkCount = 0;
            blinkState = false;
            lastBlinkTime = currentMillis;
        }
    }

    
}

bool Timer::timeIsUp() const
{
    return (!paused && startTime != 0 && (((long)(timeToElapse * 1000) - (long)(millis() - startTime) + 999) / 1000 < 0));
}

bool Timer::finished() const
{
    return !active && blinkCount >= 5;
}

void Timer::reset(bool clear_disp)
{
    active = false;
    startTime = 0;
    lastUpdateTime = 0;
    paused = false;
    lastBlinkTime = 0;
    blinking = false;
    blinkCount = 0;
    blinkState = false;
    if(clear_disp)
        display.setSegments(blank);

    
    if (taskHandle != nullptr) {
        vTaskDelete(taskHandle);     // FreeRTOS deletes the task
        taskHandle = nullptr;        // Reset the handle so we can restart later
    }
}

void Timer::interruptTimer(bool end)
{
    if(end)
	{
        // Freeze the timer on the current time
		active = false;
	}
	else
	{
        // Make the timer blink 0's 
        reset();
        start(0);
	}
}

void Timer::pause()
{
    if(!paused)
    {
        paused = true;
        pausedTimeToElapse = ((timeToElapse * 1000 - (millis()- startTime)) + 999) / 1000;
    }
}

void Timer::resume()
{
    if(paused)
    {
        reset(false);
        start(pausedTimeToElapse);
    }
}