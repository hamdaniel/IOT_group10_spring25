#ifndef SNAKE_H
#define SNAKE_H


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

#define SNAKE_SOUND_DIR 2
#define EAT_SOUND 1

/*
Communication with the application:
1. App sends the maximum length of the snake and speed of the snake.
2. ESP sends the location of the food (every time it changes) in the format "F,x,y" where x and y are the coordinates of the food.
*/ 

class Snake : public MatrixPuzzle {

    private:
        int snake_length;
        int snake_pos[64][2];
        int snake_direction; // 0: up, 1: down, 2: right, 3: left; the direction can be changed using buttons
        int food_pos[2];

        int speed; 
        int max_length;

        unsigned long last_move_time;
        bool is_game_over;

        // Game Logic
        void generate_food();
        void send_food();
        
        // Position & Movement
        bool check_collision();
        void update_snake_position();
        void change_direction();
        int calculate_position(int x, int y);

        // Drawing
        void draw() override;
        void endAnimation() override;

    public:
        Snake(BluetoothSerial* bt, Mp3Player* mp3, LedMatrix* lm, UDRLInput* bs, int s, int m_l);
        ~Snake();

        void play() override;


};

#endif // SNAKE_H