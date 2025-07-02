#include "Symbols.h"
//ADD the functionallity of time to solve
Symbol::Symbol(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, LedMatrix* lm, UDRLInput* bs, String * input, int numSymbols, int timeToFinish) 
    : MatrixPuzzle(bt, mp3, r, lm, bs), current_symbol(0), max_symbol(numSymbols), color(0) {
    last_change_time = millis();
    time_to_finish = timeToFinish;
    time_started = millis(); // Record the start time of the puzzle
    // Initialize the symbol positions
    init_symbol_positions(input);
}

void Symbol::init_symbol_positions(String* input) {
    // Initialize the symbol position array
    for(int j=0; j<3; j++){
        if(input[j].length() != ROW_LEN * COL_LEN) {
            Serial.println("Last Symbol");
            return;
        }
        Serial.println("Symbol " + String(j) + ": " + input[j]);
        for (int i = 0; i < ROW_LEN * COL_LEN; i++) {
            switch (input[j][i]){
                case '0':{
                    Symbols[j][i] = false;
                    break;
                }
                case '1':{
                    Symbols[j][i] = true;
                    break;
                }
                default:{
                    break;
                }
            }
        }
    }
    
}

void Symbol::draw() {
    //Draw the fraction in the ring:
    int time_elapsed = millis() - time_started;
    double frac = (double)time_elapsed / time_to_finish;
    frac = 1-frac;
    ring->clearPixels(); // Clear the ring before lighting it up
    switch(current_symbol){
        case 0: // Blue
            ring->lightFraction(frac, led_matrix->generateColor(0, 0, 10)); // Blue color for the ring
            break;
        case 1: // Purple
            ring->lightFraction(frac, led_matrix->generateColor(10, 0, 10)); 
            break;
        case 2: // Orange
            ring->lightFraction(frac, led_matrix->generateColor(0, 10, 10)); 
            break;
    }
    led_matrix->clearPixels(); // Clear the matrix before lighting it up
    for (int i = 0; i < ROW_LEN * COL_LEN; i++) {
        if (!Symbols[current_symbol][i]) {
            continue; // Skip if the pixel is not part of the current symbol
        }
        switch(color) {
            case 0: // Blue
                led_matrix->lightPixel(i, led_matrix->generateColor(0, 0, 10));
                break;
            case 1: // Green
                led_matrix->lightPixel(i, led_matrix->generateColor(0, 10, 0));
                break;
            case 2: // Red
                led_matrix->lightPixel(i, led_matrix->generateColor(10, 0, 0));
                break;
        }
    }
}

void Symbol::change_color() {
    color = (color + 1) % 3; // Cycle through colors: 0 -> 1 -> 2 -> 0
}

Symbol::~Symbol() {
    // Destructor logic if needed
}

void Symbol::Proceed(int isNext) {
    Serial.println("Proceeding...");
    if (isNext==1) {
        //print values of current_symbol and max_symbol
        Serial.println("Current symbol: " + String(current_symbol) + ", Max symbol: " + String(max_symbol));
        if(current_symbol < max_symbol-1) {
            current_symbol +=1;
            Serial.println("Current symbol: " + String(current_symbol) + ", Max symbol: " + String(max_symbol));
        }
        else {
            //Won:
            status = Puzzle::puzzle_status::win_anim;
        }
    }
    else {
        Serial.println("Failed to proceed, current symbol: " + String(current_symbol) + ", Max symbol: " + String(max_symbol));
        //Lost:
        status = Puzzle::puzzle_status::lose_anim;
    }
    
}

void Symbol::endAnimation() {
    if(end_anim_start_time != 0) // End animation already started
    {
        if(canDelete()) // finished end animation, change status and pbox will delete
            status = (status == puzzle_status::win_anim ? puzzle_status::win : puzzle_status::lose);
        
        return;
    }
    end_anim_start_time = millis(); // Start the end animation
    ring->lightSolid((status == Puzzle::puzzle_status::win_anim) ? ring_win_color : ring_lose_color);
    mp3_player->playFilename(GAME_WIN_LOSE_SOUND_DIR, status == Puzzle::puzzle_status::win_anim ? GAME_WIN_SOUND : GAME_LOSE_SOUND);
    // Show the symbols on the matrix
    // Light the matrix with the right color, if won, light the current symbol with the winning color, else light it with the losing color
    uintptr_t color = (status == Puzzle::puzzle_status::win_anim) ? led_matrix->generateColor(0, 10, 0) : led_matrix->generateColor(10, 0, 0);
    for (int i = 0; i < ROW_LEN * COL_LEN; i++) {
        if (Symbols[current_symbol][i]) {
            led_matrix->lightPixel(i, color); // Light the current symbol with the right color
        }
    }


}

void Symbol::play() {
    // Main loop for the puzzle
    if (getStatus() == Puzzle::puzzle_status::not_finished) {
        // Change color every 10 seconds
        if (millis() - last_change_time > 10000) {
            change_color();
            last_change_time = millis();
        }
        if (millis() - time_started > time_to_finish) {
            // Time is up, lose the puzzle
            status = Puzzle::puzzle_status::lose_anim;
        }
        Serial.println("Playing Symbol puzzle, symbol num " + String(current_symbol));
        draw();
    }
    // Handle end animation
    if (getStatus() == Puzzle::puzzle_status::win_anim || getStatus() == Puzzle::puzzle_status::lose_anim) {
        Serial.println("Ending animation for status: " + String(getStatus()));
        endAnimation();
    }
}