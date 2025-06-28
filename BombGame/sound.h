#ifndef SOUND_H
#define SOUND_H

#include <HardwareSerial.h>
#include <Arduino.h>

#define TX_PIN 25
#define RX_PIN 26
#define MP3_BAUD 9600

class Mp3Player {
  HardwareSerial& mp3Serial;
  static constexpr byte start_byte = 0x7E;
  static constexpr byte end_byte = 0xEF;
  static constexpr byte set_volume_CMD = 0x31;
  static constexpr byte play_index_CMD = 0x41;
  static constexpr byte play_filename_CMD = 0x42;
  static constexpr uint8_t select_SD_CMD[5] = { 0x7e, 0x03, 0X35, 0x01, 0xef };
  static constexpr uint8_t reset_CMD[5] = { 0x7e, 0x03, 0X35, 0x05, 0xef };

public:
  Mp3Player(int8_t txPin = TX_PIN, int8_t rxPin = RX_PIN);


  bool reset();

  void selectSDCard();

  void setVolume(byte volume);

  void playFilename(int8_t directory, int8_t file);


};

#endif // SOUND_H