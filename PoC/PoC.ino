#include "BluetoothSerial.h"
#include <stdbool.h>

#include <Adafruit_NeoPixel.h>



#include "ledMatrix.h"
#include "Snake.h"
#include "btnInput.h"
#include "maze.h"
#include "Timer.h"
#include "sound.h"

// // When setting up the NeoPixel library, we tell it how many pixels,
// // and which pin to use to send signals. Note that for older NeoPixel
// // strips you might need to change the third parameter -- see the
// // strandtest example for more information on possible values.

// //Matrix init


LedMatrix* matrix = nullptr;
UDRLInput* btns = nullptr;
Maze* maze = nullptr;
Timer* timer = nullptr;
Mp3Player* mp3 = nullptr;

Snake* snake = nullptr;

//BT init
BluetoothSerial* SerialBT = nullptr;


/*
First message in the setup - number of games
inside the loop:
  each message: 
  1. name of the game 
  2. parameters for each game (e.g. time for timer, distance for maze)
*/

void setup() {

  Serial.begin(115200);              // USB serial monitor
  // SerialBT = new BluetoothSerial();
  // SerialBT->begin("ESP32_BT_Server"); // Bluetooth name
  // while (!Serial) {
	//   ; // Wait for serial & serialBT port to be ready
	// }
  
  matrix = new LedMatrix();
  btns = new UDRLInput();
  //timer = new Timer();
  mp3 = new Mp3Player();
  snake = new Snake(matrix, btns, mp3);
  Serial.println("finished setup");

  // Gets the number of games from the app
  // if (SerialBT->available()) {
  //   String numGames = SerialBT->readStringUntil('\n');
  //   Serial.println("Number of games: " + numGames);
  // }
}
// unsigned long lastMatrixMillis = 0;
// unsigned long lastTimerMillis  = 0;

void loop() {
  //read from bluetooth
  // if (SerialBT->available()) {
  //   String boardStr = SerialBT->readStringUntil('\n');
  //   String time = SerialBT->readStringUntil('\n');
  //   String dist = SerialBT->readStringUntil('\n');
  //   Serial.println("got dist: ");
  //   Serial.println(dist);
    
  //   maze = new Maze(matrix, btns, SerialBT, mp3, boardStr, dist);

  matrix->stopIdleAnimation();
  if(snake != nullptr)
  {
    snake->play();
  }
  else//no snake, play animation
  {
    matrix->startIdleAnimation();
  }
}