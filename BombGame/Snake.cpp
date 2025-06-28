#include "Snake.h"

Snake::Snake(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, LedMatrix* lm, UDRLInput* bs, int s, int m_l) : MatrixPuzzle(bt, mp3, r, lm, bs), speed(s), max_length(m_l) {
    snake_length = 1;
    snake_pos[0][0] = 8; // Starting position x
    snake_pos[0][1] = 8; // Starting position y
    snake_direction = 2; // Start moving right
    status = Puzzle::puzzle_status::not_finished;
    last_move_time = millis();

    
    generate_food();
    send_food();
}

Snake::~Snake() {
    
    led_matrix->clearPixels();
}

void Snake::generate_food() {
    food_pos[0] = random(0, ROW_LEN);
    food_pos[1] = random(0, COL_LEN);
}

void Snake::send_food(){
    int food = food_pos[1] + ROW_LEN * food_pos[0];
    char msg[4];
    sprintf(msg, "%03d", food);
    serialBT->println(msg);
    Serial.println("Sending food position: " + String(msg));
}

bool Snake::check_collision() {
    // Check collision with walls
    if (snake_pos[0][0] < 0 || snake_pos[0][0] >= ROW_LEN ||
        snake_pos[0][1] < 0 || snake_pos[0][1] >= COL_LEN) {
        return true; // Collision with wall
    }
    
    // Check collision with itself
    for (int i = 1; i < snake_length; i++) {
        if (snake_pos[0][0] == snake_pos[i][0] && snake_pos[0][1] == snake_pos[i][1]) {
            return true; // Collision with itself
        }
    }
    return false; // No collision
}

void Snake::update_snake_position() {
    // Move the snake's body
    for (int i = snake_length - 1; i > 0; i--) {
        snake_pos[i][0] = snake_pos[i - 1][0];
        snake_pos[i][1] = snake_pos[i - 1][1];
    }
    
    // Update the head position based on the direction
    switch (snake_direction) {
        case 0: // Up
            snake_pos[0][0]--;
            break;
        case 1: // Down
            snake_pos[0][0]++;
            break;
        case 2: // Right
            snake_pos[0][1]++;
            break;
        case 3: // Left
            snake_pos[0][1]--;
            break;
    }
}

void Snake::change_direction() {
    for(int i = 0; i < 4; i++) {
        if (btns->moved(i)) { // Prevent reversing direction
            if ((snake_direction == 0 && i == 1) || // Up to Down
                (snake_direction == 1 && i == 0) || // Down to Up
                (snake_direction == 2 && i == 3) || // Right to Left
                (snake_direction == 3 && i == 2)) { // Left to Right
                return; // Do not change direction if it's a reverse
            }
            snake_direction = i;
            break;
        }
    }
}

int Snake::calculate_position(int x, int y){
    return x * COL_LEN + y;
}

void Snake::draw(){

    led_matrix->clearPixels();
    
    // Draw the snake
    int pos = calculate_position(snake_pos[0][0], snake_pos[0][1]);
    uint32_t blue = led_matrix->generateColor(0, 0, 10); // Blue color for the head
    led_matrix->lightPixel(pos, blue); // Blue color for the head

    for (int i = 1; i < snake_length; i++) {
        pos = calculate_position(snake_pos[i][0], snake_pos[i][1]);
        uint32_t green = led_matrix->generateColor(0, 10, 0); // Green color for the snake
        led_matrix->lightPixel(pos, green); // Green color for the snake
      
    }
    // Draw the food
    pos = calculate_position(food_pos[0], food_pos[1]);
    uint32_t red = led_matrix->generateColor(10, 0, 0);
    led_matrix->lightPixel(pos, red); // Red color for the food

    ring->clearPixels();
    double fraction = (double)(snake_length - 1) / (double)(max_length - 1);
    ring->lightFraction(fraction,ring_color);

}


void Snake::endAnimation()
{
  if(end_anim_start_time != 0) // End animation already started
  {
    if(canDelete()) // finished end animation, clear board
    {
      led_matrix->clearPixels();
    }
    return;
  }
  
  // TODO: add something visual

  end_anim_start_time = millis();
  mp3_player->playFilename(GAME_WIN_LOSE_SOUND_DIR, status == Puzzle::puzzle_status::win ? GAME_WIN_SOUND : GAME_LOSE_SOUND);
}


void Snake::play()
{
  switch (status)
    {
      case Puzzle::puzzle_status::not_finished:
        {
          unsigned long current_time = millis();
        
          // Check if it's time to move the snake
          if (current_time - last_move_time >= speed) {
              last_move_time = current_time;
              update_snake_position(); // Update the snake's position

              if (check_collision()) {
                status = Puzzle::puzzle_status::lose;
                  return; // Stop the game if collision occurs
              }
              
              // Check if the snake has eaten the food
              if (snake_pos[0][0] == food_pos[0] && snake_pos[0][1] == food_pos[1]) {
                if (snake_length < max_length) {
                    snake_length++;
                    mp3_player->playFilename(SNAKE_SOUND_DIR, EAT_SOUND); // Play eating sound
                }

                else { // reached max length
                    status = Puzzle::puzzle_status::win;   
                }
                generate_food(); // Generate new food position
                send_food(); // Send food position to the app
              }
              
            //   Serial.println("Got input from buttons");
          }
          btns->readInput(); // Read button inputs
          change_direction(); // Change direction based on button input
          draw(); // Draw the updated board
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
