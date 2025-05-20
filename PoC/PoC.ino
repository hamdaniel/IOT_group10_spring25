#include "BluetoothSerial.h"
#include <stdbool.h>

#include <Adafruit_NeoPixel.h>



#include "ledMatrix.h"
#include "btnInput.h"
#include "maze.h"


// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.

//Matrix init


LedMatrix* matrix = nullptr;
UDRLInput* btns = nullptr;
Maze* maze = nullptr;

//BT init
BluetoothSerial* SerialBT = nullptr;




void setup() {

  SerialBT = new BluetoothSerial();
  Serial.begin(115200);              // USB serial monitor
  SerialBT->begin("ESP32_BT_Server"); // Bluetooth name
  
  matrix = new LedMatrix();
  btns = new UDRLInput();
  
  Serial.println("finished setup");
}


void loop() {
  //read from bluetooth
  if (SerialBT->available()) {
    String boardStr =  SerialBT->readStringUntil('\n');
    String time = SerialBT->readStringUntil('\n');
    String dist = SerialBT->readStringUntil('\n');
    Serial.print("got dist: ");
    Serial.println(dist);
    maze = new Maze(matrix, btns, SerialBT, boardStr, dist);
  }
  if(maze != nullptr)
  {
    if(maze->play()) //finished
    {
      delete maze;
      maze = nullptr;
      Serial.println("deleted maze!");
    }
  }
  else
  {
    matrix->startIdleAnimation();
  }
  matrix->updateAnimation();

}