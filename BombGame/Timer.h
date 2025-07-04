#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 26
#define DIO 27

#define TEST_DELAY   1000

class Timer{
    private:
        bool active;
        int timeToElapse;
        unsigned long startTime;
        unsigned long lastUpdateTime;
        unsigned long lastBlinkTime;
        int pausedTimeToElapse;
        bool paused;
        bool blinking;
        int blinkCount;
        bool blinkState;
        int lastDisplayedSec;
        TaskHandle_t taskHandle;
        TM1637Display display;
        const uint8_t blank[4] = { 0x00, 0x00, 0x00, 0x00 };


    public:


        Timer();
        ~Timer();
        void start(int time);
        static void updateWrapper(void* timer_instance);
        void update();
        void reset(bool clear_disp = true);

        bool timeIsUp() const;
        bool finished() const;
        void interruptTimer(bool end);
        void pause();
        void resume();

};

#endif // TIMER_H