#include "puzzleBox.h"


// C'tor , D'tor, Start and End Functions
PuzzleBox::PuzzleBox(BluetoothSerial* bt) : game_started(false), curr_puzzle(nullptr),
						 puzzle_count(0), puzzles_solved(0), strikes(0), matrix(nullptr), timer(nullptr),
						 btns(nullptr), mp3(nullptr), SerialBT(bt)
{	
	matrix = new LedMatrix();
	btns = new UDRLInput();
	timer = new Timer();
	mp3 = new Mp3Player();
}

PuzzleBox::~PuzzleBox()
{
	if(curr_puzzle != nullptr)
	{
		delete curr_puzzle;
	}

	matrix->clearPixels();
	timer->reset();

	delete matrix;
	delete btns;
	delete timer;
	delete mp3;
}

void PuzzleBox::startGame(int num_puzzles) {
	game_started = true;
	puzzle_count = num_puzzles;
	puzzles_solved = 0;
	strikes = NUM_STRIKES;

	String secs = readFromBT();
	Serial.println("Starting timer with: " + secs);
	timer->start(secs.toInt());
}

void PuzzleBox::endGame() {
	game_started = false;
	puzzle_count = 0;
	puzzles_solved = 0;
	strikes = 0;
	delete curr_puzzle;
	curr_puzzle = nullptr;
	timer->reset();
	Serial.println("Stopping puzzle box");
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
		// curr_puzzle = createSnake();
	}
	else {
		// Handle unknown puzzle name
	}
}


Maze* PuzzleBox::createMaze()
{
	
	String boardStr = readFromBT();
	String time = readFromBT();
	String dist = readFromBT();
	Serial.println("Creating Maze with board: " + boardStr + ", time: " + time + ", distance: " + dist);
	return new Maze(SerialBT, mp3, matrix, btns, boardStr, dist.toInt(), time.toInt() * 1000);
}


// Main Function
void PuzzleBox::play()
{
	if(!game_started)
	{
		return;
	}

	if(curr_puzzle != nullptr) // There is a puzzle to play, play it
	{
		curr_puzzle->play();

		if(curr_puzzle->canDelete()) // Can delete puzzle
		{
			Puzzle::puzzle_status curr_status = curr_puzzle->getStatus();
			switch (curr_status)
			{
				case Puzzle::puzzle_status::win:
				{
					puzzles_solved++;
					break;
				}

				case Puzzle::puzzle_status::lose:
				{
					strikes--;
					break;
				}

				default:
					break;
			}
			delete curr_puzzle;
			curr_puzzle = nullptr;
		}
	}

	timer->update(); //Update the timer

	if(puzzles_solved == puzzle_count) // Win logic
	{

		endGame();
	}

	else if(timer->timeIsUp() || strikes == 0) // Lose logic
	{

		endGame();
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


