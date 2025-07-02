#include <Wire.h>
#include <PCF8574.h>

#define SDA_PIN 33
#define SCL_PIN 25
#define WIRE_BTN_PIN 14

PCF8574 outputExpander(0x20);  // Outputs
PCF8574 inputExpander(0x21);   // Inputs

bool connectionMatrix[8][8] = { false };

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  outputExpander.begin();
  inputExpander.begin();

  // Set all output pins HIGH initially
  for (int i = 0; i < 8; i++) {
    outputExpander.write(i, HIGH);
  }

  Serial.println("Setup done");
}

void loop() {
  // Scan all 8 output pins
  for (int i = 0; i < 8; i++) {
    // Set all outputs HIGH
    for (int j = 0; j < 8; j++) {
      outputExpander.write(j, HIGH);
    }

    // Pull current output LOW
    outputExpander.write(i, LOW);
    delay(10);  // settle time

    // Read all inputs
    for (int j = 0; j < 8; j++) {
      int val = inputExpander.read(j);
      connectionMatrix[i][j] = (val == LOW);
    }

    // Restore current output to HIGH
    outputExpander.write(i, HIGH);
  }

  // Print matrix
  Serial.println("Connection Matrix (1 = connected):");
  Serial.print("    ");
  for (int j = 0; j < 8; j++) {
    Serial.print("In");
    Serial.print(j);
    Serial.print(" ");
  }
  Serial.println();

  for (int i = 0; i < 8; i++) {
    Serial.print("Out");
    Serial.print(i);
    Serial.print(": ");
    for (int j = 0; j < 8; j++) {
      Serial.print(" ");
      Serial.print(connectionMatrix[i][j] ? "1  " : "0  ");
    }
    Serial.println();
  }

  Serial.println("---");
  delay(7000);
}
