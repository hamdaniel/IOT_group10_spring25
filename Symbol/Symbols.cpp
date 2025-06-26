#include "Symbols.h"

symbol::symbol(BluetoothSerial* bt, Mp3Player* mp3, LedMatrix* lm, UDRLInput* bs, string input)
    : MatrixPuzzle(bt, mp3, lm, bs) {
    init_symbol_positions(input);
    color = 0; // Start with blue
    last_change_time = millis(); // Initialize the last change time
    
}

void symbol::init_symbol_positions(string input) {
    // Initialize the symbol position array
    for (int i = 0; i < ROW_LEN * COL_LEN; i++) {
        switch (input[i]){
            case '0':{
                symbol_pos[i] = false;
                break;
            }
            case '1':{
                symbol_pos[i] = true;
                break;
            }
            default:{
                break;
            }
        }
    }
}

void draw() {
    led_matrix->clearPixels();
    for (int i = 0; i < 64; i++) {
        if (symbol_pos[i]) {
            if(color==0){
                led_matrix->lightPixel(i, led_matrix->generateColor(0, 0, 10)); // Blue color for the symbol
            } else if(color==1){
                led_matrix->lightPixel(i, led_matrix->generateColor(0, 10, 0)); // Green color for the symbol
            } else if(color==2){
                led_matrix->lightPixel(i, led_matrix->generateColor(10, 0, 0)); // Red color for the symbol
            }
        }
    }
    led_matrix->show();
}

void symbol::change_color() {
    color = (color + 1) % 3; // Cycle through colors: 0 -> 1 -> 2 -> 0
}

symbol::~symbol() {
    // Destructor logic if needed
}

void symbol::update_pazzle_data(string data) {
    init_symbol_positions(data);
}

void symbol::play() {
    // Main loop for the puzzle
    while (getStatus() == Puzzle::puzzle_status::not_finished) {
        // Change color every 10 seconds
        if (millis() - last_change_time > 10000) {
            change_color();
            last_change_time = millis();
        }

        // Draw the current state of the symbol
        draw();
    }
}