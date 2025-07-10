#include "puzzleBox.h"

// // When setting up the NeoPixel library, we tell it how many pixels,
// // and which pin to use to send signals. Note that for older NeoPixel
// // strips you might need to change the third parameter -- see the
// // strandtest example for more information on possible values.

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  		clock_prescale_set(clock_div_1);
#endif
	
#define BT_CONNECTED_DISCONNECTED_DIR 4
#define BT_CONNECTED_SOUND 1
#define BT_DISCONNECTED_SOUND 2

PuzzleBox* pbox = nullptr;
BluetoothSerial* SerialBT = nullptr;
Adafruit_NeoPixel* pixels = nullptr;
Mp3Player* mp3 = nullptr;

bool connected = false;

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

  mp3 = new Mp3Player();
	mp3->setVolume(30);

  pbox = new PuzzleBox(SerialBT, pixels, mp3);

  Serial.println("finished setup");
}


void loop() {
  //check if bluetooth is connected
  if (!SerialBT->connected()) {
    if(connected){ // Was connected, now not
      Serial.println("Bluetooth disconnected!");
      pbox->cleanupGame();  // Optional: cleanup if disconnected
      mp3->playFilename(BT_CONNECTED_DISCONNECTED_DIR, BT_DISCONNECTED_SOUND); // Play disconnected sound
    }
    connected = false;
    return; // Exit loop if not connected
  }
  else // Connected
  {
    if(!connected) { // Was not connected, now is
      Serial.println("Bluetooth connected!");
      mp3->playFilename(BT_CONNECTED_DISCONNECTED_DIR, BT_CONNECTED_SOUND); // Play connected sound
    }
    connected = true;
  }
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