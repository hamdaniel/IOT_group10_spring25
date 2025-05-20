#include "BluetoothSerial.h"
#include <stdbool.h>

#include <Adafruit_NeoPixel.h>



#include "ledMatrix.h"
#include "btnInput.h"
#include "maze.h"

int moves[4]; //UDRL

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.

//Matrix init


LedMatrix* matrix = nullptr;
UDRLInput* btns = nullptr;
Maze* maze = nullptr;

//BT init
BluetoothSerial SerialBT;


void sendLocations()
{
  // SerialBT.println(m.player);
  // SerialBT.println(m.target);
}



void setup() {


  Serial.begin(115200);              // USB serial monitor
  SerialBT.begin("ESP32_BT_Server"); // Bluetooth name
  
  matrix = new LedMatrix();
  btns = new UDRLInput();

  matrix->initMatrix();
  btns->initInput();
  
  Serial.println("finished setup");
}


void loop() {
  //read from bluetooth
  if (SerialBT.available()) {
    if(maze != nullptr)
    {
      delete maze;
    }
    String input =  SerialBT.readStringUntil('\n');
    maze = new Maze(matrix, btns, input, 3);
  }
  if(maze != nullptr)
  {
    maze->play();
  }
  

  //sendLocations();
}