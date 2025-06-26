#ifndef SUYMBOLS_H
#define SUYMBOLS_H

#include <stdbool.h>
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h> 

#include "ledMatrix.h"
#include "btnInput.h"
#include "sound.h"
#include "matrixPuzzle.h"

#define ROW_LEN 16
#define COL_LEN 16

/*
Communication with the application:
1. App sends the symbol to be displayed.
2. ESP listens for both success and quitting messages from the app:
    - For the coding for new symbol, parse it and initialize the symbol positions.
*/
class symbol : public MatrixPuzzle {
    private:
        bool symbol_pos[256]; // 1D array to store the position of each pixel in the symbol
        int color; // 0: blue, 1: green, 2: red
        unsigned long last_change_time; // Time of the last color change

        void draw() override;
        void change_color();

    public:
        symbol(BluetoothSerial* bt, Mp3Player* mp3, LedMatrix* lm, UDRLInput* bs);
        ~symbol();
        void update_pazzle_data(string input) override;
        void play() override;
}

#endif // SYMBOLS_H