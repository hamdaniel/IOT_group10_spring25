#include "BluetoothSerial.h"
#include <stdbool.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        13 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 256 // Popular NeoPixel ring size
#define ROW_LEN 16

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

BluetoothSerial SerialBT;

typedef struct Maze {
  bool maze[NUMPIXELS];
  int player;
  int target;
  
} Maze;

bool lastButton1State = HIGH;
bool lastButton2State = HIGH;

const uint wall = pixels.Color(5,5,0);
const uint player = pixels.Color(0,5,0);
const uint target = pixels.Color(5,0,0);
Maze m;

void setup() {

  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  Serial.begin(115200);              // USB serial monitor
  SerialBT.begin("ESP32_BT_Server"); // Bluetooth name
  Serial.println("Bluetooth SPP started. Ready to pair.");

 


  //pinMode(button1Pin, INPUT_PULLUP);
  //pinMode(button2Pin, INPUT_PULLUP);
}

void  updateMaze() {
	String maze = SerialBT.readStringUntil('\n');
  String start_loc = SerialBT.readStringUntil('\n');
  String end_loc = SerialBT.readStringUntil('\n');

  Serial.print("maze: ");
  Serial.println(maze);
  Serial.print("player: ");
  Serial.println(start_loc);
  Serial.print("target: ");
  Serial.println(end_loc);

	for (int i = 0; i < NUMPIXELS; i++) {
		m.maze[i] = (maze[i] == '1');
    Serial.print(m.maze[i]);
	}

	m.player = ((start_loc[3] -'0') + 10 * (start_loc[2] -'0')) * ROW_LEN + (start_loc[0] - '0') * 10 + (start_loc[1] - '0');
	m.target = ((end_loc[3] -'0') + 10 * (end_loc[2] -'0')) * ROW_LEN + (end_loc[0] - '0') * 10 + (end_loc[1] - '0');
	
  Serial.print("calculated start: ");
  Serial.println(m.player);

  Serial.print("calculated target: ");
  Serial.println(m.target);

}

void loop() {
  pixels.clear();
  // Check for Bluetooth messages
  if (SerialBT.available()) {
    updateMaze();
  }
  for(int i = 0; i < NUMPIXELS; i++) { 
    if(m.maze[i]) {
      Serial.print("set pixel:");
      Serial.println(i);
      pixels.setPixelColor(i, wall);
    }
  }
  pixels.setPixelColor(m.player, player);
  pixels.setPixelColor(m.target, target);
  pixels.show(); 
}