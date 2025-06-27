#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h> 

#include "ledMatrix.h"
#include "ledElement.h"
#include "btnInput.h"
#include "sound.h"
#include "matrixPuzzle.h"

#define ROW_LEN 16
#define COL_LEN 16

class Maze : public MatrixPuzzle {

  private:
      // Board and Movement
      bool board[ROW_LEN * COL_LEN];
      int move_amounts[4];

      int player_pos;
      int target_pos;

      uint32_t* wall_colors;
      uint32_t* target_colors;

      uint32_t game_over_color;
      uint32_t player_color;
      
      int dist;

      // Time
      unsigned long maze_start_time; // In millis
      unsigned long time; // In millis

      //Movement Functions
      bool isValid(int new_loc);
      void movePlayer();

      // Visual Functions
      bool isVisible(int pixel);
      int colorIdx(int pixel);
      void endAnimation() override;
      void draw() override;
      int genRGBVal(int c, int i) const;

  public:
      Maze(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, LedMatrix* lm, UDRLInput* bs, String input, int d, int t);
      ~Maze() override;

      void play() override;
};

#endif // MAZE_H
