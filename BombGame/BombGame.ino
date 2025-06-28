#include "puzzleBox.h"

// // When setting up the NeoPixel library, we tell it how many pixels,
// // and which pin to use to send signals. Note that for older NeoPixel
// // strips you might need to change the third parameter -- see the
// // strandtest example for more information on possible values.

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  		clock_prescale_set(clock_div_1);
#endif
	

PuzzleBox* pbox = nullptr;
BluetoothSerial* SerialBT = nullptr;
Adafruit_NeoPixel* pixels = nullptr;

bool isNumber(const String& str) {
  if (str.length() == 0) return false;
  for (unsigned int i = 0; i < str.length(); i++) {
    if (!isDigit(str[i])) return false;
  }
  return true;
}

void setup() {

  Serial.begin(115200);              // USB serial monitor
  SerialBT = new BluetoothSerial();
	SerialBT->begin("ESP32_BT_Server"); // Bluetooth name

  pixels = new Adafruit_NeoPixel(TOTAL_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
	pixels->begin();

  pbox = new PuzzleBox(SerialBT, pixels);
  Serial.println("finished setup");
}


void loop() {
  //read from bluetooth
  if(SerialBT->available()) // Start PuzzleBox || Create New Puzzle || or End PuzzleBox
  {
      String msg = SerialBT->readStringUntil('\n');
      if (isNumber(msg) && msg.toInt() > 0) {
        int num_games = msg.toInt();
        Serial.println("Starting with " + String(num_games) + " games.");
        pbox->startGame(num_games);
      }

      else if (msg == "exit") { // "exit" -> End PuzzleBox
        pbox->cleanupGame();
      }

      else if (pbox->validGameName(msg)) { // name -> Create New Puzzle
        Serial.println("Starting puzzle: " + msg);
        pbox->startPuzzle(msg);
      }
      
  }
  pbox->play(); // Play the curent puzzle
}