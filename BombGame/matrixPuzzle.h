#ifndef MATRIX_PUZZLE_H
#define MATRIX_PUZZLE_H

#include <stdbool.h>
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h> 

#include "ledMatrix.h"
#include "ledElement.h"
#include "UDRLInput.h"
#include "sound.h"
#include "puzzle.h"


class MatrixPuzzle : public Puzzle {
	
	protected:
		
		LedMatrix* led_matrix;
		UDRLInput* btns;

		virtual void draw() = 0;

	public:
		MatrixPuzzle(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, LedMatrix* mat, UDRLInput* bs) : Puzzle(bt, mp3, r), led_matrix(mat),
																										  btns(bs) {};
		~MatrixPuzzle() override {};


};



#endif // MATRIX_PUZZLE_H