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


int Maze::calcRGBVal(int c, int i) const
{
  return min(c * ((max(dist, 1) + 1 - i) << (max(dist, 1) + 1 - i)) , 255);
}

Maze::Maze(BluetoothSerial* bt, Mp3Player* mp3, LedMatrix* lm, UDRLInput* bs, String input, int d, int t) :
      MatrixPuzzle(bt, mp3, lm), btns(bs), dist(d), time(t)
{

	//init moves array
	move_amounts[0] = -ROW_LEN;
	move_amounts[1] = ROW_LEN;
  move_amounts[2] = 1;
  move_amounts[3] = -1;
	
	//init board & positions
	for (int i = 0; i < ROW_LEN * COL_LEN; i++) {
    switch (input[i])
    {
      case '0':
      {
        board[i] = true;
        break;
      }

      case '1':
      {
        board[i] = false;
        break;
      }
      
      case '2':
      {
        board[i] = true;
        player_pos = i;
        break;
      }
      
      case '3':
      {
        board[i] = true;
        target_pos = i;
        break;
      }

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

  maze_start_time = millis();

}

Maze::~Maze()
{
  delete[] wall_colors;
  delete[] target_colors;
  
  led_matrix->clearPixels();
}

void Maze::draw() {
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
        char msg[4];
        sprintf(msg, "%03d", player_pos);
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

void Maze::endAnimation()
{
  if(end_anim_start_time != 0) // End animation already started
  {
    if(canDelete()) // finished end animation, clear board
    {
      led_matrix->clearPixels();
    }
    return;
  }
  
  end_anim_start_time = millis();
  mp3_player->playFilename(MAZE_DIR, status == Puzzle::puzzle_status::win ? MAZE_WIN : MAZE_LOSS);
  uint32_t color = (status == Puzzle::puzzle_status::win) ? wall_colors[0] : game_over_wall_color;

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

void Maze::play()
{
  switch (status)
  {
    case Puzzle::puzzle_status::not_finished:
    {
      btns->readInput(); // Update button state
      movePlayer(); // Move player accordingly

      draw(); // Draw the maze

      // Check if need to update status
      unsigned long now = millis(); 

      if(player_pos == target_pos && now - maze_start_time < time ) // Reached target in time
      {
        status = Puzzle::puzzle_status::win;
        endAnimation();
      }
      
      else if(now - maze_start_time >= time) // Time is up
      {
        status = Puzzle::puzzle_status::lose;
        endAnimation();
      }
      break;
    }
          
    case Puzzle::puzzle_status::win:
    {
      endAnimation();
      break;
    }

    case Puzzle::puzzle_status::lose:
    {
      endAnimation();
      break;
    }

    default:
      break;
  }

}