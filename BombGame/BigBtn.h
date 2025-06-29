#ifndef BIGBTN_H
#define BIGBTN_H

#include <Arduino.h>

// Define the button pin here
#define BIGBTN_PIN 18

class BigBtn {
public:
    enum PressType {
        SINGLE,
        DOUBLE,
        LONG,
        NONE
    };
    
    static const PressType PressTypes[3];

    BigBtn(); // Constructor takes no arguments

    void update();
    PressType getPressType();

private:
    bool lastPhysicalState;
    bool stableState;
    unsigned long lastDebounceTime;
    unsigned long pressStartTime;
    unsigned long lastReleaseTime;

    bool waitingForSecondClick;
    bool longPressDetected;
    bool buttonDown;

    static constexpr unsigned long debounceDelay = 30;
    static constexpr unsigned long longPressDuration = 1000;
    static constexpr unsigned long doubleClickThreshold = 500;

    PressType detectedPress;
};

#endif
