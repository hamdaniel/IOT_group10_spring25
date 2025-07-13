#include "BigBtn.h"

const BigBtn::PressType BigBtn::PressTypes[3] = {
    BigBtn::SINGLE,
    BigBtn::DOUBLE,
    BigBtn::LONG
};

BigBtn::BigBtn()
    : lastPhysicalState(HIGH),
      stableState(HIGH),
      lastDebounceTime(0),
      pressStartTime(0),
      lastReleaseTime(0),
      waitingForSecondClick(false),
      longPressDetected(false),
      buttonDown(false),
      detectedPress(NONE)
{
    pinMode(BIGBTN_PIN, INPUT_PULLUP); // Assumes active-low button
}

void BigBtn::update() {
    bool reading = digitalRead(BIGBTN_PIN);
    unsigned long now = millis();
    detectedPress = NONE;

    // Debounce
    if (reading != lastPhysicalState) {
        lastDebounceTime = now;
    }

    if ((now - lastDebounceTime) > debounceDelay) {
        if (reading != stableState) {
            stableState = reading;

            if (stableState == LOW) {
                pressStartTime = now;
                longPressDetected = false;
                buttonDown = true;
            } else {
                buttonDown = false;

                if (!longPressDetected) {
                    if (waitingForSecondClick && (now - lastReleaseTime <= doubleClickThreshold)) {
                        detectedPress = DOUBLE;
                        waitingForSecondClick = false;
                    } else {
                        waitingForSecondClick = true;
                        lastReleaseTime = now;
                    }
                }

                longPressDetected = false;
            }
        }
    }

    lastPhysicalState = reading;

    // Long press detection
    if (buttonDown && !longPressDetected && (now - pressStartTime >= longPressDuration)) {
        detectedPress = LONG;
        longPressDetected = true;
        waitingForSecondClick = false;
    }

    // Single press timeout
    if (!buttonDown && waitingForSecondClick && (now - lastReleaseTime > doubleClickThreshold)) {
        detectedPress = SINGLE;
        waitingForSecondClick = false;
    }
}

BigBtn::PressType BigBtn::getPressType() {
    return detectedPress;
}
