#ifndef MAZE_H
#define MAZE_H


#include <stdbool.h>
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h> 

#include "ledMatrix.h"
#include "btnInput.h"
#include "sound.h"

#define ROW_LEN 16
#define COL_LEN 16
#define END_ANIMATION_LENGTH 6000

class Maze {

  bool board[ROW_LEN * COL_LEN];
  int move_amounts[4];

  uint32_t* wall_colors;
  uint32_t* target_colors;

  uint32_t game_over_wall_color;
  uint32_t player_color;

  int player_pos;
  int target_pos;
  int dist;

  unsigned long maze_end_time;
  bool maze_ended;



  LedMatrix* led_matrix;
  UDRLInput* btns;
  BluetoothSerial* serialBT;
  Mp3Player* mp3_player;

  void drawMaze();
  bool isValid(int new_loc);
  void movePlayer();
  bool isVisible(int pixel);
  int calcRGBVal(int c, int i);
  int colorIdx(int pixel);

  void startEndAnimation(bool won_game);

  public:

  enum maze_status {ongoing, displaying_w_anim, displaying_l_anim, can_delete};
  Maze(LedMatrix* lm, UDRLInput* bs, BluetoothSerial* bt, Mp3Player* mp3, String input, String d);
  ~Maze();

  maze_status play(bool is_over);//true if over



};

#endif //MAZE_H