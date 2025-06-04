#include "BluetoothSerial.h"
#include <stdbool.h>

#include <Adafruit_NeoPixel.h>



#include "ledMatrix.h"
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

//BT init
BluetoothSerial* SerialBT = nullptr;




void setup() {

  Serial.begin(115200);              // USB serial monitor
  SerialBT = new BluetoothSerial();
  SerialBT->begin("ESP32_BT_Server"); // Bluetooth name
  while (!Serial) {
	  ; // Wait for serial & serialBT port to be ready
	}
  
  matrix = new LedMatrix();
  btns = new UDRLInput();
  timer = new Timer();
  mp3 = new Mp3Player();
  
  Serial.println("finished setup");
}


void loop() {
  //read from bluetooth
  if (SerialBT->available()) {
    String boardStr = SerialBT->readStringUntil('\n');
    String time = SerialBT->readStringUntil('\n');
    String dist = SerialBT->readStringUntil('\n');
    Serial.println("got dist: ");
    Serial.println(dist);
    
    maze = new Maze(matrix, btns, SerialBT, mp3, boardStr, dist);

    matrix->stopIdleAnimation();
    timer->start(time);
  }

  // //there is a maze
  if(maze != nullptr)
  {
    Maze::maze_status status = maze->play(timer->timeIsUp());
    if(status == Maze::displaying_w_anim)
    {
      timer->reset();//stop timer
    }

    if(status == Maze::can_delete) //if true then finished, can delete maze
    {
      delete maze;
      maze = nullptr;
      Serial.println("deleted maze!");
    }
    
  }
  else//no maze, play animation
  {
    // Serial.println("no maze, playing idle animation");
    matrix->startIdleAnimation();
  }

  //update matrix and timer
  matrix->updateIdleAnimation();
  timer->update();

}