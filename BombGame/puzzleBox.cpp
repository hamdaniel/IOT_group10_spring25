#include "puzzleBox.h"


// C'tor , D'tor, Start and End Functions
PuzzleBox::PuzzleBox(BluetoothSerial* bt, Adafruit_NeoPixel* px) : game_running(false),
											curr_puzzle(nullptr), puzzle_count(0),
											puzzles_solved(0), strikes(0), pixels(px),
											matrix(nullptr), strip(nullptr), ring(nullptr), end_time(0),
											win_color(px->Color(0,10,0)), lose_color(px->Color(10,0,0)),
											timer(nullptr),  mat_btns(nullptr), morse_btn(nullptr),
											outputExpander(nullptr), inputExpander(nullptr),
											mp3(nullptr), SerialBT(bt)
											
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
	mp3 = new Mp3Player();
	mp3->setVolume(20);

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
		  name == "wires"
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
		Serial.print("winner ");
		if(curr_puzzle != nullptr)
			delete curr_puzzle;
		curr_puzzle = nullptr;
	}

	else if(timer->timeIsUp() || strikes == 0) // Lose logic. Need to send game_over_l only if time is up
	{
		Serial.print("loser ");
		if(curr_puzzle != nullptr) // If there is a puzzle, also seeing the puzzle in the app. send message 
		{
			SerialBT->println("game_over_l");
			delete curr_puzzle;

		}
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
				if(timer->timeIsUp())
					SerialBT->println("bomb_over_l_time");
				else
				{
					SerialBT->println("game_over_l");
					SerialBT->println("bomb_over_l_lives");
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
void PuzzleBox::play()
{
	
	if(!game_running) // No puzzle box & finished final visuals, nothing to do here
	{
		return;
	}
		
	if(isOver())
	{
		pixels->show();
		return;
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


