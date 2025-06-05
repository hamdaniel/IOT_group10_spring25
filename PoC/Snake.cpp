#include "Snake.h"

Snake::Snake(LedMatrix* lm, UDRLInput* bs, Mp3Player* mp3) {
    snake_length = 1;
    snake_pos[0][0] = 8; // Starting position x
    snake_pos[0][1] = 8; // Starting position y
    snake_direction = 2; // Start moving right
    is_game_over = false;
    last_move_time = millis();

    led_matrix = lm;
    btns = bs;
    mp3_player = mp3;
    speed = 400; // Default speed
    max_length = 64; // Maximum length of the snake
    
    generate_food();
}

Snake::~Snake() {
    // Destructor logic if needed
}

void Snake::generate_food() {
    food_pos[0] = random(0, ROW_LEN);
    food_pos[1] = random(0, COL_LEN);
}

void Snake::send_food(){
    String food_msg = "F";
    food_msg += food_pos[0];
    food_msg += ",";
    food_msg += food_pos[1];
    //SerialBT.println(food_msg);
}

bool Snake::check_collision() {
    // Check collision with walls
    if (snake_pos[0][0] < 0 || snake_pos[0][0] >= ROW_LEN ||
        snake_pos[0][1] < 0 || snake_pos[0][1] >= COL_LEN) {
        is_game_over = true;
        return true; // Collision with wall
    }
    
    // Check collision with itself
    for (int i = 1; i < snake_length; i++) {
        if (snake_pos[0][0] == snake_pos[i][0] && snake_pos[0][1] == snake_pos[i][1]) {
            is_game_over = true;
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

void Snake::draw_board(){
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
    
    led_matrix->show();
}

void Snake::play() {
    if (is_game_over) {
        led_matrix->clearPixels();
        led_matrix->show();
        return; // Game over, do not continue
    }
    
    unsigned long current_time = millis();
    
    // Check if it's time to move the snake
    if (current_time - last_move_time >= speed) {
        last_move_time = current_time;
        update_snake_position(); // Update the snake's position

        if (check_collision()) {
            mp3_player->playFilename(0, 1); // Play game over sound
            return; // Stop the game if collision occurs
        }
        
        // Check if the snake has eaten the food
        if (snake_pos[0][0] == food_pos[0] && snake_pos[0][1] == food_pos[1]) {
            if (snake_length < max_length) {
                snake_length++;
            }
            generate_food(); // Generate new food position
            //send_food(); // Send food position to the app
        }
        
        Serial.println("Got input from buttons");
        draw_board(); // Draw the updated board
    }
    btns->readInput(); // Read button inputs
    change_direction(); // Change direction based on button input
}
