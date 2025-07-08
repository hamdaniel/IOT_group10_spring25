#include "Symbols.h"
//ADD the functionallity of time to solve
Symbol::Symbol(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, LedMatrix* lm, UDRLInput* bs, String * per_input, String * input, int number_Symbols, int timeToFinish) 
    : MatrixPuzzle(bt, mp3, r, lm, bs), current_symbol(0), max_symbol(number_Symbols), color(0) {
    last_change_time = millis();
    time_to_finish = timeToFinish;
    time_started = millis(); // Record the start time of the puzzle
    // Initialize the symbol positions
    init_symbol_positions(per_input, input);
}

void Symbol::init_symbol_positions(String* per_input, String* input) {
    // Array of pointers to your bool arrays
    bool (*symbols_arr[2])[256] = { per_Symbols, orig_Symbols };
    String* inputs[2] = { per_input, input };

    for (int arr_idx = 0; arr_idx < 2; arr_idx++) {
        for (int j = 0; j < max_symbol; j++) {
            for (int i = 0; i < ROW_LEN * COL_LEN; i++) {
                switch (inputs[arr_idx][j][i]) {
                    case '0':
                        symbols_arr[arr_idx][j][i] = false;
                        break;
                    case '1':
                        symbols_arr[arr_idx][j][i] = true;
                        break;
                    default:
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
        if (!per_Symbols[current_symbol][i]) {
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
    if (isNext==1) {
        //print values of current_symbol and max_symbol
        if(current_symbol < max_symbol-1) {
            current_symbol +=1;
        }
        else {
            //Won:
            status = Puzzle::puzzle_status::win_anim;
        }
    }
    else {
        //Lost:
        status = Puzzle::puzzle_status::lose_anim;
    }
    
}

void Symbol::endAnimation() {
    if(end_anim_start_time != 0) // End animation already started
    {
        if(canDelete()){
            status = (status == puzzle_status::win_anim ? puzzle_status::win : puzzle_status::lose);
            return;
        } // finished end animation, change status and pbox will delete
        
        unsigned long elapsed = millis() - end_anim_start_time;
        unsigned long anim_duration = GAME_END_ANIMATION_LENGTH; // total animation time
        int symbol_count = max_symbol;
        unsigned long symbol_time = anim_duration / symbol_count;
        int symbol_idx = (int)(elapsed / symbol_time);

        uintptr_t color = (status == Puzzle::puzzle_status::win_anim) ? led_matrix->generateColor(0, 10, 0) : led_matrix->generateColor(10, 0, 0);
        led_matrix->clearPixels();
        
        for (int j = 0; j < ROW_LEN * COL_LEN; j++) {
            if (orig_Symbols[symbol_idx][j]) {
                led_matrix->lightPixel(j, color);
            }
        }
        return;
    }
    end_anim_start_time = millis(); // Start the end animation
    ring->lightSolid((status == Puzzle::puzzle_status::win_anim) ? ring_win_color : ring_lose_color);
    mp3_player->playFilename(GAME_WIN_LOSE_SOUND_DIR, status == Puzzle::puzzle_status::win_anim ? GAME_WIN_SOUND : GAME_LOSE_SOUND);

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
        draw();
    }
    // Handle end animation
    if (getStatus() == Puzzle::puzzle_status::win_anim || getStatus() == Puzzle::puzzle_status::lose_anim) {
        endAnimation();
    }
}