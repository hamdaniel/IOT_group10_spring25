#include "BluetoothSerial.h"
#include <stdbool.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define MATRIX_PIN        13 // On Trinket or Gemma, suggest changing this to 1
#define U_PIN             23
#define D_PIN             22
#define R_PIN             1
#define L_PIN             3

bool prevButtonState[4]; //UDRL
bool currButtonState[4]; //UDRL
int buttonPins[4]; //UDRL
int moves[4]; //UDRL

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.

//Matrix init
#define NUMPIXELS 256
#define COL_LEN 16

Adafruit_NeoPixel pixels(NUMPIXELS, MATRIX_PIN, NEO_GRB + NEO_KHZ800);


const uint wall = pixels.Color(5,5,5);
const uint target = pixels.Color(5,0,0);
const uint player = pixels.Color(0,5,0);

//BT init
BluetoothSerial SerialBT;




//Maze init
typedef struct Maze {
  bool maze[NUMPIXELS];
  int player;
  int target;
  int dist;
  
} Maze;

Maze m;

void sendLocations()
{
  SerialBT.println(m.player);
  SerialBT.println(m.target);
}

//Matrix funcs


int convert(int num)
{
  // return (num % 16) * COL_LEN + (((num % 16) % 2 )? (COL_LEN - num / 16 - 1) : num / 16);
  int x = num % COL_LEN;
  int y = num / COL_LEN;
  if(x % 2)
  {
    return x * COL_LEN + COL_LEN - y - 1;
  }

  return x * COL_LEN + y;
}

void clearMaze() {
  for(int i = 0; i < NUMPIXELS; i++) { 
    m.maze[i] = true;
  }
}

void  updateMaze() {
	String maze = SerialBT.readStringUntil('\n');
  


	for (int i = 0; i < NUMPIXELS; i++) {
    
    switch (maze[i])
    {
    case '0':
      m.maze[i] = true;
      break;

    case '1':
      m.maze[i] = false;
      break;
    
    case '2':
      m.maze[i] = true;
      m.player = i;
      break;
    
    case '3':
      m.maze[i] = true;
      m.target = i;
      break;

    default:
      break;
    }

  }

}

void drawMaze() {
  pixels.clear();
  
  for(int i = 0; i < NUMPIXELS; i++) { 
    if(!m.maze[i] && isVisible(i)) {
      pixels.setPixelColor(convert(i), wall);
    }
  }
  pixels.setPixelColor(convert(m.target), target);
  pixels.setPixelColor(convert(m.player), player);
  pixels.show(); 
}


//Button funcs
void initMoving()
{
  buttonPins[0] = U_PIN;
  buttonPins[1] = D_PIN;
  buttonPins[2] = R_PIN;
  buttonPins[3] = L_PIN;

  moves[0] = -COL_LEN;
  moves[1] = COL_LEN;
  moves[2] = 1;
  moves[3] = -1;


  for (int i = 0; i < 4; i++)
  {
    prevButtonState[i] = HIGH;
    pinMode(buttonPins[i], INPUT_PULLUP); 
  }
}

void readInput()
{
  for (int i = 0; i < 4; i++)
  {
    prevButtonState[i] = currButtonState[i];
    currButtonState[i] = digitalRead(buttonPins[i]);
  }
}

bool isValid(int new_loc)
{
  int new_x = new_loc % COL_LEN;
  int new_y = new_loc / COL_LEN;

  return (new_x >=0 && new_x < COL_LEN && new_y >=0 && new_y < COL_LEN && m.maze[new_loc]);
}

void movePlayer()
{
  for (int i = 0; i < 4; i++)
  {
    if(currButtonState[i] && !prevButtonState[i])
    {
      int new_loc = m.player + moves[i];
      m.player = isValid(new_loc) ? new_loc : m.player;
      return;
    }
  }
}

bool isVisible(int pixel)
{
  int pixel_x = pixel % COL_LEN;
  int pixel_y = pixel / COL_LEN;
  int player_x = m.player % COL_LEN;
  int player_y = m.player / COL_LEN;
  int abs_x = (pixel_x > player_x) ? (pixel_x - player_x) : (player_x - pixel_x);
  int abs_y = (pixel_y > player_y) ? (pixel_y - player_y) : (player_y - pixel_y);

  return (abs_x <= m.dist && abs_y <= m.dist);
  
}

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

  initMoving();
  
  clearMaze();
  m.dist = 3;
}


void loop() {
  //read from bluetooth
  if (SerialBT.available()) {
    updateMaze();
  }

  //move player
  readInput();
  movePlayer();


  //draw maze
  drawMaze();

  //sendLocations();
}