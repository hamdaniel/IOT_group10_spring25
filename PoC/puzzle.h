#ifndef PUZZLE_H
#define PUZZLE_H

#include <stdbool.h>
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h> 
#include "sound.h"

class Puzzle {

	public:
		enum puzzle_status { not_finished, win, lose};
		
	protected:
		BluetoothSerial* serialBT;
		Mp3Player* mp3_player;
		puzzle_status status;
	
	public:

		Puzzle(BluetoothSerial* bt, Mp3Player* mp3) : serialBT(bt), mp3_player(mp3), status(not_finished) {};
		virtual ~Puzzle() {};

		virtual puzzle_status getStatus() const { return status; };
		virtual bool canDelete() const { return status != Puzzle::puzzle_status::not_finished; };

		virtual void play() = 0;
};



#endif //PUZZLE_H