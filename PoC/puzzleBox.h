#ifndef PUZZLE_BOX_H
#define PUZZLE_BOX_H

#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h>

#include "ledMatrix.h"
#include "btnInput.h"
#include "sound.h"
#include "Timer.h"
#include "puzzle.h" 
#include "matrixPuzzle.h"
#include "maze.h"    

#define NUM_STRIKES 3

class PuzzleBox {

	private:
		// PuzzleBox General Properties
		bool game_started;

		Puzzle* curr_puzzle;
		int puzzle_count;
		int puzzles_solved;
		int strikes;

		// Displays
		LedMatrix* matrix = nullptr;
		Timer* timer = nullptr;

		// Input
		UDRLInput* btns = nullptr;

		// Audio
		Mp3Player* mp3 = nullptr;

		//BT init
		BluetoothSerial* SerialBT = nullptr;

		// Puzzle creators
		Maze* createMaze();
		// Snake* createSnake();

		//Helper Functions
		String readFromBT();
	public:
		PuzzleBox(BluetoothSerial* bt);
		~PuzzleBox();

		void startPuzzle(String name);
		bool validGameName(String name);
		void startGame(int num_puzzles);

		void play();
};

#endif // PUZZLE_BOX_H