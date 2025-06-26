#ifndef MINE_H
#define MINE_H

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

class Mine : public MatrixPuzzle {
private:
    // Game state
    std::vector<std::vector<int>> minefield; // 0: empty, 1: mine, 2: safe
    int player_x;
    int player_y;
    int score;
    int time_limit;

    // Game logic
    void generateMinefield(int rows, int cols, int mines);
    bool isValidMove(int x, int y);
    void updateScore(int x, int y);
    
    // Drawing
    void draw() override;
    void endAnimation() override;

#endif // MINE_H