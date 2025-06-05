#include "puzzleBox.h"


// C'tor , D'tor and Init Functions
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
	timer->start(secs);
}


// Puzzle Creation Functions
bool PuzzleBox::validGameName(String name)
{
  return (
  		  msg == "maze" ||
		  msg == "snake"
  		 );
}

void PuzzleBox::startPuzzle(String name)
{
    switch (name)
	{
	case "maze":
		curr_puzzle = createMaze();
		break;
	
	case "snake":
		// curr_puzzle = createSnake();
		break;
	default:
		break;
	}
}


Maze* PuzzleB::createMaze();
{
	
	String boardStr = readFromBT();
	String time = readFromBT();
	String dist = readFromBT();
	
	return new Maze(SerialBT, mp3, matrix, btns, boardStr, dist, time);
}


// Main Function
void PuzzleBox::play()
{
	if(curr_puzzle != nullptr) // There is a puzzle to play, play it
	{
		curr_puzzle->play();

		if(curr_puzzle->canDelete()) // Can delete puzzle
		{
			puzzle_status curr_status = curr_puzzle->getStatus();
			switch (curr_status)
			{
				case Puzzle::puzzle_status::win:
					puzzles_solved++;
					break;

				case Puzzle::puzzle_status::lose:
					strikes--;
					break;
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

	}

	else if(timer->timeIsUp() || strikes == 0) // Lose logic
	{

	}

}


//Helper Functions
String PuzzleBox::readFromBT()
{
	while(!SerialBT->isAvailable()) // BT buffer is still empty -> block
	{
		;
	}

	return SerialBT->readStringUntil('\n');
}


