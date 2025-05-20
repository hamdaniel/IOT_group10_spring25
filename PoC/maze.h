#ifndef MAZE_H
#define MAZE_H


#include <stdbool.h>
#include <Arduino.h>
#include "ledMatrix.h"
#include "btnInput.h"

#define ROW_LEN 16
#define COL_LEN 16

class Maze {

  bool board[ROW_LEN * COL_LEN];
  int move_amounts[4];

  uint32_t* wall_colors;
  uint32_t* target_colors;
  uint32_t player_color;

  int player_pos;
  int target_pos;
  int dist;



  LedMatrix* led_matrix;
  UDRLInput* btns;


  void drawMaze();
  bool isValid(int new_loc);
  void movePlayer();
  bool isVisible(int pixel);
  int calcRGBVal(int c, int i);
  int colorIdx(int pixel);


  public:
  Maze(LedMatrix* lm, UDRLInput* bs, const String input, int d = 3);
  ~Maze();

  void play();



};

#endif //MAZE_H