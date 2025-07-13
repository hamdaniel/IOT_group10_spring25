#ifndef SUYMBOLS_H
#define SUYMBOLS_H

#include <stdbool.h>
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h> 

#include "ledMatrix.h"
#include "ledElement.h"
#include "UDRLInput.h"
#include "sound.h"
#include "matrixPuzzle.h"

#define ROW_LEN 16
#define COL_LEN 16



class Symbol : public MatrixPuzzle {
    private:
        bool per_Symbols[3][256];
        bool orig_Symbols[3][256];
        int color; // 0: blue, 1: green, 2: red
        unsigned long last_change_time; // Time of the last color change
        int current_symbol; // Current symbol index
        int max_symbol;
        int time_to_finish; // Time to finish the puzzle in milliseconds
        int time_started;

        void draw() override;
        void init_symbol_positions(String * per_input, String* input);
        void change_color();
        void endAnimation() override;

    public:
        Symbol(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, LedMatrix* lm, UDRLInput* bs, String * per_input, String * input, int numper_Symbols, int timeToFinish);
        ~Symbol();
        void Proceed(int isNext);
        void play() override;
};

#endif // per_Symbols_H