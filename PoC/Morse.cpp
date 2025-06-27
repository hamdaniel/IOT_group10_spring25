#include "Morse.h"

// Morse code for letters A-Z
const char* MORSE_TABLE[26] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
  "....", "..", ".---", "-.-", ".-..", "--", "-.",
  "---", ".--.", "--.-", ".-.", "...", "-", "..-",
  "...-", ".--", "-..-", "-.--", "--.."
};

// LED timings
const int DOT_DURATION = 200;      // ms
const int DASH_DURATION = DOT_DURATION * 3;
const int SYMBOL_PAUSE = DOT_DURATION;
const int LETTER_PAUSE = DOT_DURATION * 3;

// Utility to convert char to Morse string
String getMorseCode(char c) {
  if (c >= 'A' && c <= 'Z') return MORSE_TABLE[c - 'A'];
  if (c >= 'a' && c <= 'z') return MORSE_TABLE[c - 'a'];
  return "";
}

void Morse::showInMorse(char c) {
  String morse = getMorseCode(c);
  for (char sym : morse) {
    if (sym == '.') {
      digitalWrite(MORSE_LED_PIN, HIGH);
      delay(DOT_DURATION);
    } else if (sym == '-') {
      digitalWrite(MORSE_LED_PIN, HIGH);
      delay(DASH_DURATION);
    }
    digitalWrite(MORSE_LED_PIN, LOW);
    delay(SYMBOL_PAUSE);
  }
  delay(LETTER_PAUSE); // Gap between letters
}

Morse::Morse(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, BigBtn* bb, String ow)
  : Puzzle(bt, mp3, r), btn(bb), orig_word(ow), target_val(0),
    state(MorseState::SHOW_MORSE), currentLetterIdx(0),
    currentSymbolIdx(0), lastActionTime(0), symbolOn(false)
{
  pinMode(MORSE_LED_PIN, OUTPUT);
  memset(permutation, 0, sizeof(permutation));

  // Copy original word into permutation for debug/optional use
  orig_word.toCharArray(permutation, orig_word.length() + 1);

  // Scramble the word
  scrambled = orig_word;
  int len = scrambled.length();
  for (int i = 0; i < len; ++i) {
    int j = random(i, len);
    char temp = scrambled[i];
    scrambled[i] = scrambled[j];
    scrambled[j] = temp;
  }

  for (int i = 0; i < orig_word.length(); i++) {
    target_val += (orig_word[i] * i);
  }
  target_val %= 3;

  // Prepare the first Morse letter
  currentMorse = getMorseCode(scrambled[0]);

  // Initialize timing and LED state
  digitalWrite(MORSE_LED_PIN, LOW);
  lastActionTime = millis();
}

void Morse::play() {
  unsigned long now = millis();

  switch (state) {
    case MorseState::SHOW_MORSE: {
      if (currentLetterIdx >= scrambled.length()) {
		for(int i = 0; i < orig_word.length(); i++)
        state = MorseState::WAIT_INPUT;
        return;
      }

      if (currentSymbolIdx < currentMorse.length()) {
        char sym = currentMorse[currentSymbolIdx];

        if (!symbolOn && now - lastActionTime >= LETTER_PAUSE) {
          digitalWrite(MORSE_LED_PIN, HIGH);
          lastActionTime = now;
          symbolOn = true;
        }
        else if (symbolOn && ((sym == '.' && now - lastActionTime >= DOT_DURATION) ||
                              (sym == '-' && now - lastActionTime >= DASH_DURATION))) {
          digitalWrite(MORSE_LED_PIN, LOW);
          lastActionTime = now;
          symbolOn = false;
          currentSymbolIdx++;
        }
      } else if (now - lastActionTime >= LETTER_PAUSE) {
        currentLetterIdx++;
        if (currentLetterIdx < scrambled.length()) {
          currentMorse = getMorseCode(scrambled[currentLetterIdx]);
          currentSymbolIdx = 0;
          lastActionTime = now;
        }
      }
      break;
    }

    case MorseState::WAIT_INPUT: {
      btn->readInput();

      if (target_val == 0 && btn->isSinglePress()) {
        status = puzzle_status::win;
        state = MorseState::DONE;
      } else if (target_val == 1 && btn->isDoublePress()) {
        status = puzzle_status::win;
        state = MorseState::DONE;
      } else if (target_val == 2 && btn->isLongPress()) {
        status = puzzle_status::win;
        state = MorseState::DONE;
      }
      break;
    }

    case MorseState::DONE: {
      // Nothing more to do. PuzzleBox will clean us up
      break;
    }
  }
}