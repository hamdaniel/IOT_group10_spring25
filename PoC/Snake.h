#ifndef SNAKE_H
#define SNAKE_H


#include <Arduino.h>


#include "ledMatrix.h"
#include "btnInput.h"
#include "sound.h"

#define ROW_LEN 16
#define COL_LEN 16

/*
Communication with the application:
1. App sends the maximum length of the snake and speed of the snake.
2. ESP sends the location of the food (every time it changes) in the format "F,x,y" where x and y are the coordinates of the food.
*/ 

class Snake {
    int snake_length;
    int snake_pos[64][2];
    int snake_direction; // 0: up, 1: down, 2: right, 3: left; the direction can be changed using buttons
    int food_pos[2];

    int speed; 
    int max_length;

    unsigned long last_move_time;
    bool is_game_over;

    LedMatrix* led_matrix;
    UDRLInput* btns;
    //BluetoothSerial* serialBT;
    Mp3Player* mp3_player;

    void generate_food();
    void send_food();
    bool check_collision();
    void update_snake_position();
    void change_direction();
    void draw_board();
    int calculate_position(int x, int y);
    public:
    Snake(LedMatrix* lm, UDRLInput* bs, Mp3Player* mp3);
    ~Snake();
    void play();


};

#endif // SNAKE_H