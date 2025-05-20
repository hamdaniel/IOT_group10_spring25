#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 19
#define DIO 18

#define TEST_DELAY   1000

class Timer{
    int timeToElapse;
    int secLeft;
    unsigned long lastUpdateTime;
    unsigned long lastBlinkTime;
    bool blinking;
    int blinkCount;
    bool blinkState;

    TM1637Display display;
    uint8_t blank[4] = { 0x00, 0x00, 0x00, 0x00 };


    public:
    Timer();
    ~Timer();
    void start(int time);
    void changeDisplay();
};

#endif //TIMER_H