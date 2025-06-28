#include "Snake.h"

Snake::Snake(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, LedMatrix* lm, UDRLInput* bs, int s, int m_l) : MatrixPuzzle(bt, mp3, r, lm, bs),
                                                                                                                 speed(s), max_length(m_l),
                                                                                                                 eaten(false), direction_changed(false),
                                                                                                                 segments_to_draw(1){
    snake_length = 1;
    snake_pos[0][0] = 8; // Starting position x
    snake_pos[0][1] = 8; // Starting position y
    snake_direction = 2; // Start moving right
    status = Puzzle::puzzle_status::not_finished;
    last_move_time = millis();

    blink_state = true;      // toggles between visible/invisible
    last_blink_time = 0;     // tracks last toggle time
    BLINK_INTERVAL = 250;    // blink every 250ms
    
    generate_food();
    send_food();
}

Snake::~Snake() {
    
    led_matrix->clearPixels();
}

void Snake::generate_food() {
    while (true) {
        int x = random(0, ROW_LEN);
        int y = random(0, COL_LEN);

        bool is_on_snake = false;
        for (int i = 0; i < snake_length; i++) {
            if (snake_pos[i][0] == x && snake_pos[i][1] == y) {
                is_on_snake = true;
                break;
            }
        }

        if (!is_on_snake) {
            food_pos[0] = x;
            food_pos[1] = y;
            return;
        }

        // If position was occupied, try again
    }
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
    if (direction_changed) return; // prevent multiple changes between moves

    for(int i = 0; i < 4; i++) {
        if (btns->moved(i)) {
            if ((snake_direction == 0 && i == 1) || // Up to Down
                (snake_direction == 1 && i == 0) || // Down to Up
                (snake_direction == 2 && i == 3) || // Right to Left
                (snake_direction == 3 && i == 2)) { // Left to Right
                return; // Invalid reverse direction
            }

            snake_direction = i;
            direction_changed = true;
            break;
        }
    }
}

int Snake::calculate_position(int x, int y){
    return x * COL_LEN + y;
}

void Snake::draw() {
    led_matrix->clearPixels();

    // Always draw the head in blue
    if (segments_to_draw > 0) {
        int pos = calculate_position(snake_pos[0][0], snake_pos[0][1]);
        uint32_t head_color = led_matrix->generateColor(0, 0, 10); // Always blue
        led_matrix->lightPixel(pos, head_color);
    }

    // Draw body (blink red during lose animation)
    for (int i = 1; i < segments_to_draw; i++) {
        if (status == Puzzle::puzzle_status::lose_anim && !blink_state)
            continue; // Skip blinking off

        int pos = calculate_position(snake_pos[i][0], snake_pos[i][1]);
        uint32_t body_color =
            (status == Puzzle::puzzle_status::lose_anim)
            ? led_matrix->generateColor(10, 0, 0) // Red for losing
            : led_matrix->generateColor(0, 10, 0); // Green for normal
        led_matrix->lightPixel(pos, body_color);
    }

    if (status != Puzzle::puzzle_status::not_finished)
        return;

    // Gameplay progress ring
    ring->clearPixels();
    double fraction = (double)(snake_length - 1) / (double)(max_length - 1);
    ring->lightFraction(fraction, ring_color);
}

void Snake::endAnimation()
{
    if (end_anim_start_time != 0) // Already started
    {
        unsigned long elapsed = millis() - end_anim_start_time;
        int segments = snake_length;
        int interval = GAME_END_ANIMATION_LENGTH / segments;

        // Handle blinking for lose animation
        if (status == Puzzle::puzzle_status::lose_anim) {
            if (millis() - last_blink_time > BLINK_INTERVAL) {
                blink_state = !blink_state;
                last_blink_time = millis();
            }
        }

        // Shrink the snake over time
        int visible = std::max(0, (int)(segments - (elapsed / interval)));
        segments_to_draw = visible;

        if (canDelete()) {
            status = (status == Puzzle::puzzle_status::win_anim ? Puzzle::puzzle_status::win : Puzzle::puzzle_status::lose);
        }
        return;
    }

    end_anim_start_time = millis();
    last_blink_time = millis();
    blink_state = true;  // start visible
    mp3_player->playFilename(GAME_WIN_LOSE_SOUND_DIR,
                             status == Puzzle::puzzle_status::win_anim ? GAME_WIN_SOUND : GAME_LOSE_SOUND);
    ring->lightSolid((status == Puzzle::puzzle_status::win_anim) ? ring_win_color : ring_lose_color);
}
void Snake::get_next_head_pos(int& next_x, int& next_y) const{
    next_x = snake_pos[0][0];
    next_y = snake_pos[0][1];
    switch (snake_direction) {
        case 0: next_x--; break; // Up
        case 1: next_x++; break; // Down
        case 2: next_y++; break; // Right
        case 3: next_y--; break; // Left
    }
}

void Snake::play()
{
    switch (status)
    {
        case Puzzle::puzzle_status::not_finished:
        {
            segments_to_draw = snake_length;
            unsigned long current_time = millis();

            // Read input and potentially change direction before movement
            btns->readInput();
            change_direction();

            // Check for food consumption (before move)
            if (snake_pos[0][0] == food_pos[0] && snake_pos[0][1] == food_pos[1] && !eaten)
            {
                if (snake_length < max_length)
                {
                    // Grow snake by duplicating last segment
                    snake_pos[snake_length][0] = snake_pos[snake_length - 1][0];
                    snake_pos[snake_length][1] = snake_pos[snake_length - 1][1];

                    snake_length++;
                    segments_to_draw++;
                    mp3_player->playFilename(SNAKE_SOUND_DIR, EAT_SOUND);
                    eaten = true;
                    generate_food();
                    send_food();
                }
                else
                {
                    status = Puzzle::puzzle_status::win_anim;
                }
            }

            // Only move if enough time has passed
            if (current_time - last_move_time >= speed)
            {
                last_move_time = current_time;

                // Determine next position (after possible input)
                int next_x, next_y;
                get_next_head_pos(next_x, next_y);

                // Wall collision check
                if (next_x < 0 || next_x >= ROW_LEN || next_y < 0 || next_y >= COL_LEN)
                {
                    status = Puzzle::puzzle_status::lose_anim;
                    return;
                }

                // Self collision check
                for (int i = 0; i < snake_length; i++)
                {
                    if (snake_pos[i][0] == next_x && snake_pos[i][1] == next_y)
                    {
                        status = Puzzle::puzzle_status::lose_anim;
                        return;
                    }
                }

                // Move snake
                update_snake_position();
                eaten = false;
                direction_changed = false;
            }
            draw();
            break;
        }

        case Puzzle::puzzle_status::win_anim:
        {
            endAnimation();
            draw();
            break;
        }

        case Puzzle::puzzle_status::lose_anim:
        {
            endAnimation();
            draw();
            break;
        }

        default:
            break;
    }
}
