#include "BluetoothSerial.h"
#include <stdbool.h>

#include <Adafruit_NeoPixel.h>



#include "ledMatrix.h"
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


LedMatrix matrix;//inits the led matrix

const uint wall = matrix.generateColor(5,5,5);
const uint target = matrix.generateColor(5,0,0);
const uint player = matrix.generateColor(0,5,0);

int target_r = 1;
int target_g = 0;
int target_b = 0;

int wall_r = 1;
int wall_g = 1;
int wall_b = 1;


uint* targets;
uint* walls;

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
  matrix.clearPixels(); // pixels.clear();
  
  for(int i = 0; i < NUMPIXELS; i++) { 
    if(!m.maze[i] && isVisible(i)) {
      matrix.lightPixel(i, walls[chooseColor(i)]); //.pixels.setPixelColor(convert(i), walls[chooseColor(i)]);
    }
  }
  if(isVisible(m.target))
  {
    matrix.lightPixel(m.target, targets[chooseColor(m.target)]); //.setPixelColor(convert(m.target), targets[chooseColor(m.target)]);
    // pixels.setPixelColor(convert(m.target), walls[chooseColor(i)]);
  }

  matrix.lightPixel(m.player, player); //pixels.setPixelColor(convert(m.player), player);
  matrix.show();
  // pixels.show(); 
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

int calcColor(int c, int i)
{
  return (c * ((m.dist + 1 - i) << (m.dist + 1 - i)) > 255) ? 255 : c * ((m.dist + 1 - i) << (m.dist + 1 - i));
}

void initColors()
{

  walls = (uint*)malloc((m.dist)*sizeof(uint32_t));
  targets = (uint*)malloc((m.dist)*sizeof(uint32_t));

  for(int i = 0; i < m.dist; i++)
  {
    
    walls[i] = matrix.generateColor(calcColor(wall_r, i), calcColor(wall_g, i), calcColor(wall_b, i));
    targets[i] = matrix.generateColor(calcColor(target_r, i), calcColor(target_g, i), calcColor(target_b, i));
  }
}

int chooseColor(int pixel)
{
  int pixel_x = pixel % COL_LEN;
  int pixel_y = pixel / COL_LEN;
  int player_x = m.player % COL_LEN;
  int player_y = m.player / COL_LEN;
  int abs_x = (pixel_x > player_x) ? (pixel_x - player_x) : (player_x - pixel_x);
  int abs_y = (pixel_y > player_y) ? (pixel_y - player_y) : (player_y - pixel_y);
  return (abs_x < abs_y ? abs_y : abs_x) - 1;
}

void setup() {

  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):

  // END of Trinket-specific code.

  Serial.begin(115200);              // USB serial monitor
  SerialBT.begin("ESP32_BT_Server"); // Bluetooth name
  
  m.dist = 3;

  initMoving();
  clearMaze();
  initColors();
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