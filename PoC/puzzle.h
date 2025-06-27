#ifndef PUZZLE_H
#define PUZZLE_H

#include <stdbool.h>
#include <Arduino.h>
#include <stdlib.h> 
#include "BluetoothSerial.h"

#include "sound.h"
#include "ledElement.h"

#define WIN_LOSE_SOUND_DIR 1
#define WIN_SOUND 1
#define LOSS_SOUND 2

class Puzzle {

	public:
		enum puzzle_status { not_finished, win, lose};
		
	protected:
		BluetoothSerial* serialBT;
		Mp3Player* mp3_player;

		LedElement* ring;
		uint32_t ring_color;
		uint32_t ring_win_color;

		puzzle_status status;
		//LedStrip* led_strip; // All puzzles will have a led strip for displaying progress or attempts/time left
	
	public:

		Puzzle(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r) : serialBT(bt), mp3_player(mp3), ring(r),
																	ring_color(r->generateColor(0,0,10)), ring_win_color(r->generateColor(0,10,0)),
																	status(not_finished) {Serial.printf("[Puzzle] r: %p, ring: %p\n", r, ring);};
		virtual ~Puzzle() {};

		virtual puzzle_status getStatus() const { return status; };
		virtual bool canDelete() const { return status != Puzzle::puzzle_status::not_finished; };

		virtual void play() = 0;
};



#endif //PUZZLE_H