#include <Wire.h>
#include <PCF8574.h>

// Your I2C pins
#define SDA_PIN 21
#define SCL_PIN 5

PCF8574 outputExpander(0x20);  // A0 A1 A2 = GND
PCF8574 inputExpander(0x21);   // A0 = VCC

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  outputExpander.begin();
  inputExpander.begin();

  Serial.println("Setup done");
}

void loop() {
  for (int i = 0; i < 8; i++) {
    // Set all outputs LOW
    for (int j = 0; j < 8; j++)
      outputExpander.write(j, LOW);

    // Set one output HIGH
    outputExpander.write(i, HIGH);
    delay(100);

    // Read inputs
    Serial.print("Output P");
    Serial.print(i);
    Serial.print(" HIGH | Inputs: ");
    for (int j = 0; j < 8; j++) {
      int val = inputExpander.read(j);
      Serial.print(val);
      Serial.print(" ");
    }
    Serial.println();

    delay(300);
  }

  Serial.println("---");
  delay(1000);
}