#include "puzzleBox.h"

// // When setting up the NeoPixel library, we tell it how many pixels,
// // and which pin to use to send signals. Note that for older NeoPixel
// // strips you might need to change the third parameter -- see the
// // strandtest example for more information on possible values.



PuzzleBox* pbox = nullptr;
BluetoothSerial* SerialBT = nullptr;

void setup() {

  Serial.begin(115200);              // USB serial monitor
  SerialBT = new BluetoothSerial();
	SerialBT->begin("ESP32_BT_Server"); // Bluetooth name
  pbox = new PuzzleBox(SerialBT);
  Serial.println("finished setup");
}


void loop() {
  //read from bluetooth
  if(SerialBT->available()) // Start PuzzleBox || Create New Puzzle || or End PuzzleBox
  {
      String msg = SerialBT->readStringUntil('\n');
      if (msg.toInt() > 0) { // number -> Start PuzzleBox
        int num_games = msg.toInt();
        pbox->startGame(num_games);
      } 

      else if (msg == "exit") { // "exit" -> End PuzzleBox
        // Handle exit logic
      }

      else if (pbox->validGameName(msg)) { // name -> Create New Puzzle
        pbox->startPuzzle(msg);
      }
      
  }
  pbox->play(); // Play the curent puzzle
}