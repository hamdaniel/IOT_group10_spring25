#ifndef MATRIX_PUZZLE_H
#define MATRIX_PUZZLE_H

#include <stdbool.h>
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h> 

#include "ledMatrix.h"
#include "btnInput.h"
#include "sound.h"
#include "puzzle.h"


#define END_ANIMATION_LENGTH 6000


class MatrixPuzzle : public Puzzle {
	
	protected:
		unsigned long end_anim_start_time; // In millis
		LedMatrix* led_matrix;
		UDRLInput* btns;

		virtual void draw() = 0;
		virtual void endAnimation() = 0;

	public:
		MatrixPuzzle(BluetoothSerial* bt, Mp3Player* mp3, LedMatrix* mat, UDRLInput* bs) : Puzzle(bt, mp3), led_matrix(mat), btns(bs), end_anim_start_time(0) {};
		~MatrixPuzzle() override {};

		virtual puzzle_status getStatus() const override { return (canDelete() ? status : Puzzle::puzzle_status::not_finished); };
		virtual bool canDelete() const override { return ((end_anim_start_time != 0) && 
														  (millis() - end_anim_start_time > END_ANIMATION_LENGTH)); };

};



#endif //MATRIX_PUZZLE_H