#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h> 

#include "ledMatrix.h"
#include "btnInput.h"
#include "sound.h"
#include "matrixPuzzle.h"

class Maze : public MatrixPuzzle {

  private:
      // Board and Movement
      bool board[ROW_LEN * COL_LEN];
      int move_amounts[4];

      UDRLInput* btns;

      int player_pos;
      int target_pos;

      // Visuals
      uint32_t* wall_colors;
      uint32_t* target_colors;

      uint32_t game_over_wall_color;
      uint32_t player_color;

      int dist;

      // Time
      unsigned long maze_start_time;
      unsigned long duration;

      //Movement Functions
      bool isValid(int new_loc);
      void movePlayer();

      // Visual Functions
      bool isVisible(int pixel);
      int colorIdx(int pixel);
      void endAnimation() override;
      void draw() override;
      int calcRGBVal(int c, int i) const;

  public:
      Maze(BluetoothSerial* bt, Mp3Player* mp3, LedMatrix* lm, UDRLInput* bs, String input, int d, int t);
      ~Maze() override;

      void play() override;
};

#endif // MAZE_H
