#include "puzzleBox.h"


// C'tor , D'tor, Start and End Functions
PuzzleBox::PuzzleBox(BluetoothSerial* bt, Adafruit_NeoPixel* px) : game_running(false),
											curr_puzzle(nullptr), puzzle_count(0),
											puzzles_solved(0), strikes(0), pixels(px),
											matrix(nullptr), strip(nullptr), ring(nullptr), timer(nullptr),
											btns(nullptr), mp3(nullptr), SerialBT(bt)
											
{	
	
	matrix = new LedMatrix(pixels, 0, MATRIX_NUM_PIXELS);
	strip = new LedElement(pixels, MATRIX_NUM_PIXELS, STRIP_NUM_PIXELS);
	ring = new LedElement(pixels, MATRIX_NUM_PIXELS + STRIP_NUM_PIXELS, RING_NUM_PIXELS);
	
	matrix->clearPixels();
	strip->clearPixels();
	ring->clearPixels();
	
	strip->lightSolid(strip->generateColor(5,5,5));
	
	pixels->show();

	btns = new UDRLInput();
	timer = new Timer();
	mp3 = new Mp3Player();

	mp3->setVolume(10);
}

PuzzleBox::~PuzzleBox()
{
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
	delete btns;
	delete timer;
	delete mp3;
}

void PuzzleBox::startGame(int num_puzzles) {
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
		  name == "snake"
  		 );
}

void PuzzleBox::startPuzzle(String name)
{
	if (name == "maze") {
		curr_puzzle = createMaze();
	}
	else if (name == "snake") {
		curr_puzzle = createSnake();
	}
	else {
		// Handle unknown puzzle name
	}
}

void PuzzleBox::cleanupGame()
{
	Serial.println("Stopping puzzle box");

	// PuzzleBox Data
	game_running = false;
	puzzle_count = 0;
	puzzles_solved = 0;
	strikes = 0;
	if(curr_puzzle != nullptr)
		delete curr_puzzle;
	curr_puzzle = nullptr;
	
	// Visuals
	timer->reset();
	matrix->clearPixels();
	strip->clearPixels();
	ring->clearPixels();
	pixels->show();
}

Maze* PuzzleBox::createMaze()
{
	
	String boardStr = readFromBT();
	String time = readFromBT();
	String dist = readFromBT();
	Serial.println("Creating Maze with board: " + boardStr + ", time: " + time + ", distance: " + dist);
	return new Maze(SerialBT, mp3, ring, matrix, btns, boardStr, dist.toInt(), time.toInt() * 1000);
}

Snake* PuzzleBox::createSnake()
{
	String max_len = readFromBT();
	String speed = readFromBT();
	Serial.println("Creating Snake with target length: " + max_len + ", speed: " + speed);
	return new Snake(SerialBT, mp3, ring, matrix, btns, (int)(1000 / speed.toFloat()), max_len.toInt());
}
// Main Function
void PuzzleBox::play()
{
	if(!game_running) // No puzzle box & finished final visuals, nothing to do here
	{
		return;
	}

	if(timer->finished()) // Time is up and finished blinking. Can clear all displays and set running to false
	{
		cleanupGame();
	}
	if(curr_puzzle != nullptr) // There is a puzzle to play, play it
	{
		if (!timer->timeIsUp()) 
        	curr_puzzle->play();

		if(curr_puzzle->canDelete()) // Can delete puzzle
		{
			Puzzle::puzzle_status curr_status = curr_puzzle->getStatus();
			switch (curr_status)
			{
				case Puzzle::puzzle_status::win:
				{
					puzzles_solved++;
					SerialBT->println("game_over_w");
					break;
				}

				case Puzzle::puzzle_status::lose:
				{
					strikes--;
					SerialBT->println("game_over_l");
					break;
				}

				default:
					break;
			}
			ring->clearPixels();
			matrix->clearPixels();
			delete curr_puzzle;
			curr_puzzle = nullptr;
		}
	}

	
	pixels->show();	

	if(puzzles_solved == puzzle_count) // Win logic
	{
		Serial.println("Won Game!");
		if(curr_puzzle != nullptr)
			delete curr_puzzle;
		curr_puzzle = nullptr;
	}

	else if(timer->timeIsUp() || strikes == 0) // Lose logic
	{
		Serial.println("Lost Game!");
		if(curr_puzzle != nullptr)
			delete curr_puzzle;
		curr_puzzle = nullptr;
	}

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


