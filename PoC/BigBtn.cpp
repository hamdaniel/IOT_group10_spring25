#include "BigBtn.h"

BigBtn::BigBtn() :
    buttonPin(BIGBTN_PIN),
    prevState(HIGH),
    currState(HIGH),
    pressStartTime(0),
    lastReleaseTime(0),
    singlePressDetected(false),
    doublePressDetected(false),
    longPressDetected(false),
    waitingForSecondPress(false)
{
    pinMode(buttonPin, INPUT_PULLUP);
}

void BigBtn::readInput()
{
    prevState = currState;
    currState = digitalRead(buttonPin);

    unsigned long now = millis();

    // Detect button press (transition HIGH->LOW)
    if (prevState == HIGH && currState == LOW) {
        pressStartTime = now;
        longPressDetected = false;  // Reset long press flag on new press
    }

    // Detect button release (LOW->HIGH)
    if (prevState == LOW && currState == HIGH) {
        unsigned long pressDuration = now - pressStartTime;

        // Check long press
        if (pressDuration >= longPressDuration) {
            // Long press just ended - keep flag false because isLongPress() returns true only while held
            longPressDetected = false;
            singlePressDetected = false;
            doublePressDetected = false;
            waitingForSecondPress = false;
            lastReleaseTime = 0;
            return;
        }

        // Short press released - detect single or double press
        if (waitingForSecondPress) {
            // If second press within double press interval
            if (now - lastReleaseTime <= doublePressInterval) {
                doublePressDetected = true;
                singlePressDetected = false;
                waitingForSecondPress = false;
            } else {
                // Too late - treat previous as single press, start over
                singlePressDetected = true;
                doublePressDetected = false;
                lastReleaseTime = now;
            }
        } else {
            // First press released - wait for second press
            waitingForSecondPress = true;
            lastReleaseTime = now;
            singlePressDetected = false;
            doublePressDetected = false;
        }
    }

    // Check if double press window expired without second press
    if (waitingForSecondPress && (now - lastReleaseTime > doublePressInterval)) {
        singlePressDetected = true;
        doublePressDetected = false;
        waitingForSecondPress = false;
    }

    // Check for long press (button held down long enough)
    if (currState == LOW && !longPressDetected && (now - pressStartTime >= longPressDuration)) {
        longPressDetected = true;
        singlePressDetected = false;
        doublePressDetected = false;
        waitingForSecondPress = false;
    }
}

bool BigBtn::isSinglePress()
{
    if (singlePressDetected) {
        singlePressDetected = false; // clear event after reporting
        return true;
    }
    return false;
}

bool BigBtn::isDoublePress()
{
    if (doublePressDetected) {
        doublePressDetected = false; // clear event after reporting
        return true;
    }
    return false;
}

bool BigBtn::isLongPress()
{
    // Return true while button is held longer than longPressDuration
    return longPressDetected;
}

void BigBtn::resetEvents()
{
    singlePressDetected = false;
    doublePressDetected = false;
    longPressDetected = false;
    waitingForSecondPress = false;
}
