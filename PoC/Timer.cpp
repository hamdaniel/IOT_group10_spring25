#include "Timer.h"

Timer::Timer() : display(CLK, DIO)
{
    active = false;
    startTime = 0;
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

void Timer::start(int time)
{
    startTime = millis();
    active = true;
    lastUpdateTime = millis();
    timeToElapse = time;
    secLeft = time;
    Serial.println(String("creating update wrapper. blink is: ") + (blinking ? "true" : "false"));
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
    Serial.println(String("inside update wrapper. blink is: ") + (instance->blinking ? "true" : "false"));

    while (true) {
        instance->update();  // call your method
        vTaskDelay(50 / portTICK_PERIOD_MS);  // ~20Hz polling, change if needed
    }
  
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
            Serial.println("starting blink!");
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
    return startTime != 0 && (millis() - startTime) >= (timeToElapse * 1000);
}

void Timer::reset()
{
    active = false;
    startTime = 0;
    lastUpdateTime = 0;
    lastBlinkTime = 0;
    blinking = false;
    blinkCount = 0;
    blinkState = false;
    display.setSegments(blank);

    
    if (taskHandle != nullptr) {
        vTaskDelete(taskHandle);     // FreeRTOS deletes the task
        taskHandle = nullptr;        // Reset the handle so we can restart later
    }
}

