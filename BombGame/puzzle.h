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
#define END_ANIMATION_LENGTH 6000

class Puzzle {

	public:
		enum puzzle_status { not_finished, win, lose};
		
	protected:
		BluetoothSerial* serialBT;
		Mp3Player* mp3_player;
		
		LedElement* ring;
		uint32_t ring_color;
		uint32_t ring_win_color;
		uint32_t ring_lose_color;
		
		puzzle_status status;
	
		unsigned long end_anim_start_time; // In millis
	
	public:

		Puzzle(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r) : serialBT(bt), mp3_player(mp3), ring(r),
																	ring_color(r->generateColor(0,0,10)), ring_win_color(r->generateColor(0,10,0)),
																	ring_lose_color(r->generateColor(10,0,0)), status(not_finished), end_anim_start_time(0) {};
		virtual ~Puzzle() {};

		puzzle_status getStatus() const { return (canDelete() ? status : Puzzle::puzzle_status::not_finished); };
		bool canDelete() const { return ((end_anim_start_time != 0) && 
														  (millis() - end_anim_start_time > END_ANIMATION_LENGTH)); };

		virtual void play() = 0;

		virtual void endAnimation() = 0;
};



#endif // PUZZLE_H