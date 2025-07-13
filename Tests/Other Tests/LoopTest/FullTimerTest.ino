// #include <Arduino.h>
// #include <TM1637Display.h>

// // Module connection pins (Digital Pins)
// #define CLK 19
// #define DIO 18

// #define TEST_DELAY   1000

// TM1637Display display(CLK, DIO);

// int sec_left = 10;
// unsigned long lastUpdate = 0;
// bool blinking = false;
// int blinkCount = 0;
// unsigned long lastBlink = 0;
// bool blinkState = false;

// void setup()
// {
//   display.setBrightness(0x0f);
//   display.showNumberDec(sec_left, true);
//   uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
// }

// void loop()
// {
//   unsigned long currentMillis = millis();
  

//   if (!blinking) {
//     // Countdown logic
//     if (sec_left >= 0 && (currentMillis - lastUpdate >= TEST_DELAY)) {
//       display.showNumberDec(sec_left, true);
//       lastUpdate = currentMillis;
//       sec_left--;
//     }
//     // Start blinking when countdown ends
//     if (sec_left < 0) {
//       blinking = true;
//       blinkCount = 0;
//       blinkState = false;
//       lastBlink = currentMillis;
//     }
//   } else {
//     // Blinking logic: blink zero three times
//     if (currentMillis - lastBlink >= TEST_DELAY) {
//       blinkState = !blinkState;
//       lastBlink = currentMillis;
//       if (blinkState) {
//         display.showNumberDec(0, true);
//         blinkCount++;
//       } else {
//         display.setSegments(blank);
//       }
//         // Stop blinking after five blinks
//         if (blinkCount >= 5) {
//             blinking = false; // Stop blinking
//             sec_left = 10; // Reset countdown
//             display.setSegments(blank);
//         }
//     }
//   }
// }