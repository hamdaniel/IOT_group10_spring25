#include "maze.h"

const int target_r = 1;
const int target_g = 0;
const int target_b = 1;

const int wall_r = 1;
const int wall_g = 1;
const int wall_b = 1;

const int game_over_wall_r = 10;
const int game_over_wall_g = 0;
const int game_over_wall_b = 0;

const int player_r = 0;
const int player_g = 10;
const int player_b = 0;

Maze::Maze(LedMatrix* lm, UDRLInput* bs, BluetoothSerial* bt, Mp3Player* mp3, String input, String d)
{

	led_matrix = lm;
	btns = bs;
  serialBT = bt;
  mp3_player = mp3;
	dist = d[0]-'0';

	//init moves array
	move_amounts[0] = -ROW_LEN;
	move_amounts[1] = ROW_LEN;
  move_amounts[2] = 1;
  move_amounts[3] = -1;
	
  maze_ended = false;
	
	//init board & positions
	for (int i = 0; i < ROW_LEN * COL_LEN; i++) {
    switch (input[i])
    {
    case '0':
      board[i] = true;
      break;

    case '1':
      board[i] = false;
      break;
    
    case '2':
      board[i] = true;
      player_pos = i;
      break;
    
    case '3':
      board[i] = true;
      target_pos = i;
      break;

    default:
      break;
    }

  }

  //init color arrays
  wall_colors = new uint32_t[max(dist,1)];
  target_colors = new uint32_t[max(dist,1)];
  
  player_color = led_matrix->generateColor(player_r, player_g, player_b);
  game_over_wall_color = led_matrix->generateColor(game_over_wall_r, game_over_wall_g, game_over_wall_b);
  for(int i = 0; i < max(dist,1); i++)
  {
    wall_colors[i] = led_matrix->generateColor(calcRGBVal(wall_r, i), calcRGBVal(wall_g, i), calcRGBVal(wall_b, i));
    target_colors[i] = led_matrix->generateColor(calcRGBVal(target_r, i), calcRGBVal(target_g, i), calcRGBVal(target_b, i));
  }


}

Maze::~Maze()
{
  delete wall_colors;
  delete target_colors;
  
  led_matrix->clearPixels();
}

void Maze::drawMaze() {
  led_matrix->clearPixels();
  
  for(int i = 0; i < NUMPIXELS; i++) { 
    if(!board[i] && isVisible(i)) {
      led_matrix->lightPixel(i, wall_colors[colorIdx(i)]);
    }
  }
  if(isVisible(target_pos))
  {
    led_matrix->lightPixel(target_pos, target_colors[colorIdx(target_pos)]);
  }

  led_matrix->lightPixel(player_pos, player_color); 
  led_matrix->show();
  
}

bool Maze::isValid(int new_loc)
{
  int new_x = new_loc % COL_LEN;
  int new_y = new_loc / COL_LEN;

  return (new_x >=0 && new_x < COL_LEN && new_y >=0 && new_y < COL_LEN && board[new_loc]);
}

void Maze::movePlayer()
{
  for (int i = 0; i < 4; i++)
  {
    if(btns->moved(i))
    {
      int new_loc = player_pos + move_amounts[i];
      if(isValid(new_loc))
      {
        player_pos = new_loc;
        String msg = "000";
        msg[0] += player_pos / 100;
        msg[1] += (player_pos % 100) / 10;
        msg[2] += player_pos % 10;
        serialBT->println(msg);
      }
      return;
    }
  }
}

bool Maze::isVisible(int pixel)
{
  int pixel_x = pixel % COL_LEN;
  int pixel_y = pixel / COL_LEN;
  int player_x = player_pos % COL_LEN;
  int player_y = player_pos / COL_LEN;
  int abs_x = max(pixel_x - player_x, player_x - pixel_x);
  int abs_y = max(pixel_y - player_y, player_y - pixel_y);

  return (abs_x <= dist && abs_y <= dist);
  
}

int Maze::calcRGBVal(int c, int i)
{
  return min(c * ((max(dist, 1) + 1 - i) << (max(dist, 1) + 1 - i)) , 255);
}

int Maze::colorIdx(int pixel)
{
  int pixel_x = pixel % COL_LEN;
  int pixel_y = pixel / COL_LEN;
  int player_x = player_pos % COL_LEN;
  int player_y = player_pos / COL_LEN;
  int abs_x = max(pixel_x - player_x, player_x - pixel_x);
  int abs_y = max(pixel_y - player_y, player_y - pixel_y);

  return max(abs_x, abs_y) - 1;
}

void Maze::startEndAnimation(bool won_game)
{
  if(maze_ended)//already started end animation
  {
    return;
  }

  maze_ended = true;
  maze_end_time = millis();
  mp3_player->playFilename(MAZE_DIR, won_game ? MAZE_WIN : MAZE_LOSS);
  uint32_t color = won_game ? wall_colors[0] : game_over_wall_color;

  for(int i = 0; i < ROW_LEN * COL_LEN; i++)
  {
    if(!board[i])
    {
      led_matrix->lightPixel(i, color);
    }
  }

  led_matrix->lightPixel(target_pos,target_colors[0]);
  led_matrix->lightPixel(player_pos, player_color);
  led_matrix->show();
  

}


Maze::maze_status Maze::play(bool timer_over)
{
  unsigned long now = millis();
  if(maze_ended && now - maze_end_time > END_ANIMATION_LENGTH) //game ended and enough time passed, can return true and delete maze
  {
    led_matrix->clearPixels();//remove maze from matrix
    return can_delete;
  }

  if(timer_over) //time is up
  {
    startEndAnimation(false);
    return displaying_l_anim;
  }

  if(player_pos == target_pos) //finisehd maze
  {
    startEndAnimation(true);
    return displaying_w_anim;
  }
  
  //if got here maze is not over and time is not up, get input and move player

  //read input
  btns->readInput();
  
  //move player accordingly
  movePlayer();
  //draw the maze
  drawMaze();
  return ongoing;
}