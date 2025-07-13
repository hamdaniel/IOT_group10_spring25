#include "sound.h"


constexpr uint8_t Mp3Player::select_SD_CMD[5];
constexpr uint8_t Mp3Player::reset_CMD[5];


Mp3Player::Mp3Player(int8_t txPin, int8_t rxPin) : mp3Serial(Serial2) {
    mp3Serial.begin(MP3_BAUD, SERIAL_8N1, txPin, rxPin);
    
    if (reset()) {
      Serial.println("reset MP3 success");
    } else {
      Serial.println("reset MP3 fail");
    }
    selectSDCard();
    delay(1200);
  }

  bool Mp3Player::reset() {
    Serial.println("MP3 RESET");
    mp3Serial.flush();
    for (int i = 0; i < 5; i++) {
      mp3Serial.write(reset_CMD[i]);
    }
    return mp3Serial.available();
  }

  void Mp3Player::selectSDCard() {
    Serial.println("MP3_select_SD_card");
    for (int i = 0; i < 5; i++) {
      mp3Serial.write(select_SD_CMD[i]);
    }
  }

  void Mp3Player::setVolume(byte volume) {
    Serial.print("set volume= ");
    Serial.print(volume);
    Serial.println(" of 30");
    mp3Serial.write(start_byte);
    mp3Serial.write((byte)0x03);
    mp3Serial.write(set_volume_CMD);
    mp3Serial.write(volume);
    mp3Serial.write(end_byte);
  }

  void Mp3Player::playFilename(int8_t directory, int8_t file) {
    Serial.print("playing directory ");
    Serial.print(directory);
    Serial.print(" file number ");
    Serial.println(file);
    mp3Serial.write(start_byte);
    mp3Serial.write((byte)0x04);
    mp3Serial.write(play_filename_CMD);
    mp3Serial.write((byte)directory);
    mp3Serial.write((byte)file);
    mp3Serial.write(end_byte);
  }
