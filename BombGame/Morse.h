#ifndef MORSE_H
#define MORSE_H

#include <stdbool.h>
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h> 

#include "ledElement.h"
#include "sound.h"
#include "puzzle.h"
#include "BigBtn.h"

#define MORSE_LED_PIN 5

class Morse : public Puzzle {
private:
    enum class MorseState {
        START_ANIMATION,
        SHOW_MORSE,
        WAIT_RESTART,
        END_ANIMATION,
        DONE
    };

    BigBtn* btn;
    String orig_word;
    char permutation[4];
    int target_val;

    MorseState state;
    String scrambled;
    int currentLetterIdx;
    String currentMorse;
    int currentSymbolIdx;
    unsigned long lastActionTime;
    unsigned long start_anim_start_time;
    bool symbolOn;

    void checkForInput();
    String getMorseCode(char c);

public:
    Morse(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, BigBtn* bb, String ow);
    ~Morse() override = default;

    void play() override;
    void endAnimation() override;
};

#endif // MORSE_H
