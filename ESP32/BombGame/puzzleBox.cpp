#include "puzzleBox.h"


// C'tor , D'tor, Start and End Functions
PuzzleBox::PuzzleBox(BluetoothSerial* bt, Adafruit_NeoPixel* px, Mp3Player* m) :
    game_running(false),
    curr_puzzle(nullptr), puzzle_count(0),
    puzzles_solved(0), strikes(0),
    pixels(px),
    matrix(nullptr), strip(nullptr), ring(nullptr),
    end_time(0),
    win_color(px->Color(0, 10, 0)), lose_color(px->Color(10, 0, 0)),
    timer(nullptr),
    mat_btns(nullptr), morse_btn(nullptr),
    outputExpander(nullptr), inputExpander(nullptr),
    mp3(m), SerialBT(bt),

    // === Idle Animation Initialization ===
    last_ring_update(0),
    last_glow_update(0),
    last_matrix_update(0),
    ring_offset(0),
    glow_phase(0),
    increasing(true),
    idle_anim_running(false)
{
    pinMode(MORSE_LED_PIN, OUTPUT);
    digitalWrite(MORSE_LED_PIN, LOW);

    matrix = new LedMatrix(pixels, 0, MATRIX_NUM_PIXELS);
    strip = new LedElement(pixels, MATRIX_NUM_PIXELS, STRIP_NUM_PIXELS);
    ring = new LedElement(pixels, MATRIX_NUM_PIXELS + STRIP_NUM_PIXELS, RING_NUM_PIXELS);

    matrix->clearPixels();
    strip->clearPixels();
    ring->clearPixels();

    outputExpander = new PCF8574(0x20);
    inputExpander = new PCF8574(0x21);
    Wire.begin(SDA_PIN, SCL_PIN);
    outputExpander->begin();
    inputExpander->begin();
    pinMode(WIRE_BTN_PIN, INPUT_PULLUP);

    pixels->show();

    mat_btns = new UDRLInput();
    morse_btn = new BigBtn();
    timer = new Timer();


    Serial.println("PuzzleBox C'tor complete!");
}

PuzzleBox::~PuzzleBox()
{
	Serial.println("PuzzleBox D'tor!");
	if(curr_puzzle != nullptr)
	{
		delete curr_puzzle;
	}

	matrix->clearPixels();
	strip->clearPixels();
	ring->clearPixels();
	pixels->show();
	timer->reset();

	delete matrix;
	delete strip;
	delete ring;
	delete mat_btns;
	delete morse_btn;
	delete timer;
	delete mp3;
}

void PuzzleBox::startGame(int num_puzzles) {
	Serial.println("Starting PuzzleBox, cleaning up");
	// First, clear up any previous game
	cleanupGame();

	game_running = true;
	puzzle_count = num_puzzles;
	puzzles_solved = 0;
	strikes = NUM_STRIKES;

	String secs = readFromBT();
	Serial.println("Starting timer with: " + secs);

	matrix->clearPixels();
	strip->clearPixels();
	ring->clearPixels();
	pixels->show();

	//starting timer on separate core logic
    timer->start(secs.toInt());
}

// Puzzle Creation Functions
bool PuzzleBox::validGameName(String name)
{
  return (
  		  name == "maze" ||
		  name == "snake" || 
		  name == "morse" ||
		  name == "wires" ||
		  name == "symbol"
  		 );
}

void PuzzleBox::startPuzzle(String name)
{
	Serial.println("Puzzle to create: " + name);
	if(name == "maze") {
		curr_puzzle = createMaze();
	}
	else if(name == "snake") {
		curr_puzzle = createSnake();
	}
	else if(name == "morse") {
		curr_puzzle = createMorse();
	}
	else if(name == "wires") {
		curr_puzzle = createWires();
	}
	else if(name == "symbol") {
		curr_puzzle = handleSymbol();
	}
	else {
		Serial.println("Invalid puzzle name: " + name);
		return;
	}
	
}

void PuzzleBox::cleanupGame()
{

	Serial.println("Cleaning up");

	// PuzzleBox Data
	game_running = false;
	puzzle_count = 0;
	puzzles_solved = 0;
	strikes = 0;
	if(curr_puzzle != nullptr)
		delete curr_puzzle;
	curr_puzzle = nullptr;
	end_time = 0;
		
	// Visuals
	digitalWrite(MORSE_LED_PIN, LOW);
	
	timer->reset();
	matrix->clearPixels();
	strip->clearPixels();
	ring->clearPixels();
	pixels->show();
	Serial.println("Finished Cleanup");

}

Maze* PuzzleBox::createMaze()
{
	
	String boardStr = readFromBT();
	String time = readFromBT();
	String dist = readFromBT();
	Serial.println("Creating Maze with board: " + boardStr + ", time: " + time + ", distance: " + dist);
	return new Maze(SerialBT, mp3, ring, matrix, mat_btns, boardStr, dist.toInt(), time.toInt() * 1000);
}

Snake* PuzzleBox::createSnake()
{
	String max_len = readFromBT();
	String speed = readFromBT();
	Serial.println("Creating Snake with target length: " + max_len + ", speed: " + speed);
	return new Snake(SerialBT, mp3, ring, matrix, mat_btns, (int)(750 / speed.toFloat()), max_len.toInt());
}


Morse* PuzzleBox::createMorse()
{
	String word = readFromBT();
	Serial.println("Creating Morse with word: " + word);
	return new Morse(SerialBT,mp3,ring,morse_btn,word);
}

Symbol* PuzzleBox::handleSymbol()
{
	Symbol* per_Symbols = nullptr;
	Symbol* orig_Symbols = nullptr;
	Symbol *symbol = nullptr;
	String type = readFromBT();
	if (type=="init"){
		//time, amount of symbols, and the permutation of the symbols 
		String time = readFromBT(); //time for solving
		//convert time to int
		int time_int = time.toInt() * 1000; // convert to milliseconds
		String num_symbols = readFromBT();
		int num_symbols_int = num_symbols.toInt();
	
		String orig_Symbols[3] = {"0", "0", "0"}; 
		String per_Symbols[3] = {"0", "0", "0"};
		for (int i=0; i < num_symbols_int; i++)
		{
			orig_Symbols[i] = readFromBT();

			per_Symbols[i] = readFromBT();
		}
		return new Symbol(SerialBT, mp3, ring, matrix, mat_btns, per_Symbols, orig_Symbols, num_symbols_int, time_int);
	}
	else if (type=="pass"){
		symbol = static_cast<Symbol*>(curr_puzzle);
		symbol->Proceed(1); // Proceed to next symbol
		
	}
	else if (type=="fail"){
		symbol = static_cast<Symbol*>(curr_puzzle);
		symbol->Proceed(0); // Proceed to next symbol
	}
	
	return symbol; // Return the current puzzle, which is a Symbol puzzle
}

Wires* PuzzleBox::createWires()
{
	int num_wires = readFromBT().toInt();
	int attempts = readFromBT().toInt();
	Serial.printf("Creating Wires with: %d wires and %d attempts\n",num_wires, attempts);
	return new Wires(SerialBT, mp3, ring, inputExpander, outputExpander, num_wires, attempts);
	
}

bool PuzzleBox::isOver()
{
	if(puzzles_solved == puzzle_count) // Win logic. No need to send game_over_w, already sent before
	{
		if(curr_puzzle != nullptr)
			delete curr_puzzle;
		curr_puzzle = nullptr;
	}

	else if(timer->timeIsUp() || strikes == 0) // Lose logic. Need to send game_over_l only if time is up
	{
		if(curr_puzzle != nullptr) // If there is a puzzle, delete it
			delete curr_puzzle;

		curr_puzzle = nullptr;
	}
	else
		return false; // If didn't win or lose, no need to do any end of pbox logic


	bool won = (puzzles_solved == puzzle_count);
	if(end_time != 0)
	{
		if(canCleanup())
		{
			//Send pbox end message to app and clean up pbox data
			if(won)
				SerialBT->println("bomb_over_w");
			else
			{
				if(strikes == 0)
				{
					SerialBT->println("game_over_l");
					delay(250);
					SerialBT->println("bomb_over_l_lives");
				}
				else
				{
					SerialBT->println("game_over_l");
					delay(250);
					SerialBT->println("bomb_over_l_time");
				}
			}
			
			cleanupGame();
		}
	}
	else
	{
		end_time = millis();
		
		// Play correct sound
		mp3->playFilename(PBOX_WIN_LOSE_SOUND_DIR, won ? PBOX_WIN_SOUND : PBOX_LOSE_SOUND);
		
		// Light all LED elements in correct color
		ring->lightSolid(won ? win_color : lose_color);
		strip->lightSolid(won ? win_color : lose_color);
		matrix->lightSolid(won ? win_color : lose_color);
		
		// Change what the timer shows
		timer->interruptTimer(won);

		digitalWrite(MORSE_LED_PIN, HIGH);
		
	}
	return true;
}
	

bool PuzzleBox::canCleanup() const
{
	return (puzzles_solved != puzzle_count && timer->finished()) ||
		   (puzzles_solved == puzzle_count && ((end_time != 0) && (millis() - end_time > PBOX_END_ANIMATION_LENGTH)));
}

// Main Function
bool PuzzleBox::play()
{
	
	if(!game_running) // No puzzle box & finished final visuals, nothing to do here
		return false;
	
		
	if(isOver())
	{
		pixels->show();
		return true; // Game is over but end animation is not
	}

	if(curr_puzzle != nullptr) // There is a puzzle to play, play it
	{ 
		curr_puzzle->play();
		
		Puzzle::puzzle_status curr_status = curr_puzzle->getStatus();
		switch (curr_status)
		{
			case Puzzle::puzzle_status::win:
			{
				puzzles_solved++;
				SerialBT->println("game_over_w");
				timer->resume();
				ring->clearPixels();
				matrix->clearPixels();
				delete curr_puzzle;
				curr_puzzle = nullptr;
				break;
			}

			case Puzzle::puzzle_status::lose:
			{
				strikes--;
				if(strikes)
					SerialBT->println("game_over_l");
				timer->resume();
				ring->clearPixels();
				matrix->clearPixels();
				if(curr_puzzle != nullptr)
				delete curr_puzzle;
				curr_puzzle = nullptr;
				break;
			}

			case Puzzle::puzzle_status::win_anim:
			{
				timer->pause();
			}
			
			case Puzzle::puzzle_status::lose_anim:
			{
				timer->pause();
			}

			default:
				break;
		}

	}
	if(end_time == 0)
	{
		//light strip according to puzzles solved and strikes recieved
		for(int i = 0; i < puzzles_solved; i++)
			strip->lightPixel(i, win_color);
		for(int i = puzzles_solved; i < puzzle_count; i++)
			strip->lightPixel(i, win_color + 2 * lose_color);
		
		for(int i = 1; i <= NUM_STRIKES - strikes; i++)
			strip->lightPixel(-i, lose_color);
	}
	
	pixels->show();	

	return true;
}


void PuzzleBox::stepIdleAnimation() {
    unsigned long current_time = millis();

    // === INITIALIZATION ===
    if (!idle_anim_running) {
        idle_anim_running = true;

        ring_offset = 0;
        glow_phase = 0;
        increasing = true;

        last_ring_update = current_time - 350;
        last_glow_update = current_time - 50;
        last_matrix_update = current_time;

        for (int i = 0; i < 16; i++) {
            idle_column_heights[i] = random(16);                     // Random start row
            idle_column_intervals[i] = random(200, 700);              // Random per-column speed
            idle_column_last_update[i] = current_time - random(150);  // Desync start times
			tail_sizes[i] = random(2,6);
        }

        strip->clearPixels();
        ring->clearPixels();
        matrix->clearPixels();
    }

    // === RING ANIMATION ===
	if (current_time - last_ring_update > 350) {
		last_ring_update = current_time;

		ring->clearPixels();

		for (int i = 0; i < 5; i++) {
			int led = (ring_offset + i) % RING_NUM_PIXELS;
			uint8_t brightness;
			switch (i) {
				case 0: brightness = 10; break;
				case 1: brightness = 20; break;
				case 2: brightness = 30; break;
				case 3: brightness = 40;  break;
				case 4: brightness = 50;  break;
				default: brightness = 0; break;
			}
			ring->lightPixel(led, ring->generateColor(0, 0, brightness));
		}

		ring_offset = (ring_offset + 1) % RING_NUM_PIXELS;
	}

    // === STRIP GLOW ===
    if (current_time - last_glow_update > 50) {
        last_glow_update = current_time;

        glow_phase += (increasing ? 5 : -5);
        if (glow_phase >= 255) increasing = false;
        else if (glow_phase <= 0) increasing = true;

        strip->clearPixels();
		strip->lightSolid(strip->generateColor(0, 0, glow_phase));
    }

    // === MATRIX RAIN ===
	matrix->clearPixels();
    for (int col = 0; col < 16; col++) {
		int head_y = idle_column_heights[col];
		matrix->lightPixel(col + head_y * 16, matrix->generateColor(0, 0, 10 * tail_sizes[col]));
		for(int tail_frag = 1; tail_frag <= tail_sizes[col]; tail_frag++)
		{
			int tail_y = (head_y - tail_frag + 16) % 16;
			matrix->lightPixel(col + tail_y * 16, matrix->generateColor(0, 0, 10 * (tail_sizes[col] - tail_frag)));
		}

		if (current_time - idle_column_last_update[col] >= idle_column_intervals[col])
		{
			idle_column_last_update[col] = current_time;
			idle_column_heights[col] = (head_y + 1) % 16;
		}
		
	}

    pixels->show();
}

void PuzzleBox::endIdleAnimation() {
	idle_anim_running = false;
}


//Helper Functions
String PuzzleBox::readFromBT()
{
	while(!SerialBT->available()) // BT buffer is still empty -> block
	{
		;
	}

	return SerialBT->readStringUntil('\n');
}


