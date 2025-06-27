#ifndef BIGBTN_H
#define BIGBTN_H

#include <Arduino.h>

#define BIGBTN_PIN 27

class BigBtn {
private:
    uint8_t buttonPin;
    
    // Button states
    bool prevState;
    bool currState;

    // Timing
    unsigned long pressStartTime;
    unsigned long lastReleaseTime;

    // Settings (adjustable)
    const unsigned long longPressDuration = 1000;  // ms
    const unsigned long doublePressInterval = 400; // ms max between two presses
    
    // Event flags
    bool singlePressDetected;
    bool doublePressDetected;
    bool longPressDetected;

    // Helper
    bool waitingForSecondPress;

public:
    BigBtn();
    ~BigBtn() = default;

    void readInput();
    
    bool isSinglePress();    // Returns true once when a single press is detected
    bool isDoublePress();    // Returns true once when a double press is detected
    bool isLongPress();      // Returns true while button held long
    
    // Optional: reset events manually
    void resetEvents();
};

#endif // BIGBTN_H
