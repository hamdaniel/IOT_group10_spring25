#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

const int button1Pin = 15;  // GPIO 0
const int button2Pin = 23;  // GPIO 2

bool lastButton1State = HIGH;
bool lastButton2State = HIGH;

void setup() {
  Serial.begin(115200);              // USB serial monitor
  SerialBT.begin("ESP32_BT_Server"); // Bluetooth name
  Serial.println("Bluetooth SPP started. Ready to pair.");

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
}

void loop() {
  // Check for Bluetooth messages
  if (SerialBT.available()) {
    String maze = SerialBT.readStringUntil('\n');
    String start_loc = SerialBT.readStringUntil('\n');
    String end_loc = SerialBT.readStringUntil('\n');

    Serial.print("Received via Bluetooth: \n");
    Serial.print("Maze: \n");  // Print to USB Serial monitor
    Serial.println(maze);  // Print to USB Serial monitor
    Serial.println("start: \n");
    Serial.println(start_loc);  // Print to USB Serial monitor
    Serial.println("End: \n");
    Serial.println(end_loc);  // Print to USB Serial monitor
	  Serial.print("\n");
  }
}
