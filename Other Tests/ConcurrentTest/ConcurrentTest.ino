#include <Arduino.h>
#include "BluetoothSerial.h"
#include <stdlib.h>

TaskHandle_t TaskCore0;
TaskHandle_t TaskCore1;

const int buttonPin = 33;


// Task on Core 0: Monitor button and kill TaskCore1 if pressed
void PrintCore0(void * parameter) {
  pinMode(buttonPin, INPUT_PULLUP);  // Configure button pin

  while (true) {
    if (digitalRead(buttonPin) == LOW) {  // Button pressed (active low)
      Serial.println("Button pressed! Killing TaskCore1...");
      vTaskDelete(TaskCore1);  // Delete the task running on Core 1
      TaskCore1 = NULL;
    } else {
      Serial.println("Core 0 monitoring button...");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);  // FreeRTOS delay
  }
}

// Task on Core 1: Just print periodically
void PrintCore1(void * parameter) {
  while (true) {
    Serial.println("Hello from Core 1");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);  // Ensure Serial is ready

  // Create task on Core 0
  xTaskCreatePinnedToCore(
    PrintCore0,
    "Core0Task",
    2048,
    NULL,
    1,
    &TaskCore0,
    0  // Run on Core 0
  );

  // Create task on Core 1
  xTaskCreatePinnedToCore(
    PrintCore1,
    "Core1Task",
    2048,
    NULL,
    1,
    &TaskCore1,
    1  // Run on Core 1
  );
}

void loop() {
  // Nothing here — everything is in tasks
}
