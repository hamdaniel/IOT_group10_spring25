#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

const int ledPin = 2;  // Most ESP32 boards have the blue LED on GPIO 2

void setup() {
  Serial.begin(115200);              
  SerialBT.begin("ESP32_BT_Server"); 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.println("Bluetooth SPP started. Ready to pair.");
}

void loop() {
  if (SerialBT.available()) {
    String incoming = SerialBT.readStringUntil('\n');
    incoming.trim();  // Remove whitespace and newline characters

    Serial.print("Received via Bluetooth: ");
    Serial.println(incoming);

    bool isNaturalNumber = true;
    for (int i = 0; i < incoming.length(); i++) {
      if (!isDigit(incoming.charAt(i))) {
        isNaturalNumber = false;
        break;
      }
    }

    if (isNaturalNumber && incoming.length() > 0) {
      int duration = incoming.toInt();
      if (duration > 0) {
        digitalWrite(ledPin, HIGH);
        delay(duration * 1000);
        digitalWrite(ledPin, LOW);
      }
    }
  }

  delay(100);
}
