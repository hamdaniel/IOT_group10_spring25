#ifndef PUZZLE_BOX_H
#define PUZZLE_BOX_H

#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#define NEOPIXEL_PIN        13 // On Trinket or Gemma, suggest changing this to 1
#define MATRIX_NUM_PIXELS 256
#define STRIP_NUM_PIXELS 10
#define RING_NUM_PIXELS 16
#define TOTAL_PIXELS (MATRIX_NUM_PIXELS + STRIP_NUM_PIXELS + RING_NUM_PIXELS)

#include "ledMatrix.h"
#include "ledMatrix.h"

#include "UDRLInput.h"
#include "BigBtn.h"

#include "sound.h"
#include "Timer.h"
#include "puzzle.h" 
#include "matrixPuzzle.h"

#include "maze.h"    
#include "Snake.h"

#include "Morse.h"

#define NUM_STRIKES 3

class PuzzleBox {

	private:
		// PuzzleBox General Properties
		bool game_running;
		Puzzle* curr_puzzle;
		int puzzle_count;
		int puzzles_solved;
		int strikes;
		
		// Displays
		Adafruit_NeoPixel* pixels;
		LedMatrix* matrix;
		LedElement* strip;
		LedElement* ring;
		
		Timer* timer;
		// Input
		UDRLInput* mat_btns;
		BigBtn* morse_btn;

		// Audio
		Mp3Player* mp3;

		//BT init
		BluetoothSerial* SerialBT;

		
		// Puzzle creators
		Maze* createMaze();
		Snake* createSnake();


		Morse* createMorse();
		
		//Helper Functions
		String readFromBT();
		
		public:
		PuzzleBox(BluetoothSerial* bt, Adafruit_NeoPixel* px);
		~PuzzleBox();
		
		void startPuzzle(String name);
		bool validGameName(String name);
		void startGame(int num_puzzles);
		void cleanupGame();

		void play();
};

#endif // PUZZLE_BOX_H