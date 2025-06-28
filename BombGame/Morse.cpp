#include "Morse.h"

// Morse code for letters A-Z
const char* MORSE_TABLE[26] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
  "....", "..", ".---", "-.-", ".-..", "--", "-.",
  "---", ".--.", "--.-", ".-.", "...", "-", "..-",
  "...-", ".--", "-..-", "-.--", "--.."
};

// Timings (doubled)
const int DOT_DURATION = 400;
const int DASH_DURATION = DOT_DURATION * 3;
const int SYMBOL_PAUSE = DOT_DURATION;
const int LETTER_PAUSE = DOT_DURATION * 3;


Morse::Morse(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, BigBtn* bb, String ow)
  : Puzzle(bt, mp3, r), btn(bb), orig_word(ow), target_val(0),
    state(MorseState::START_ANIMATION), currentLetterIdx(0),
    currentSymbolIdx(0), lastActionTime(0), start_anim_start_time(millis()), symbolOn(false)
{
  pinMode(MORSE_LED_PIN, OUTPUT);
  memset(permutation, 0, sizeof(permutation));

  // Copy original word into permutation for optional use
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

  // Calculate target value
  for (int i = 0; i < orig_word.length(); i++) {
    target_val += ((orig_word[i] + 1 - 'a') * (i+1));
  }
  target_val %= 3;
  switch (target_val)
  {
  case 0:
	Serial.printf("Quick press to win!\n");
	break;
  case 1:
	Serial.printf("Double press to win!\n");
	break;
  case 2:
	Serial.printf("Long press to win!\n");
	break;
  default:
	break;
  }
  

  currentMorse = MORSE_TABLE[scrambled[0] - 'a'];
  digitalWrite(MORSE_LED_PIN, LOW);
}

void Morse::checkForInput() {
  btn->update();

  BigBtn::PressType press = btn->getPressType();

  if(press == BigBtn::NONE)
  	return;

  state = MorseState::END_ANIMATION; // If got here the player pressed the button someway, an end animation will play
  status = (BigBtn::PressTypes[target_val] == press) ? puzzle_status::win_anim : puzzle_status::lose_anim;
	
}

void Morse::play() {
  unsigned long now = millis();

  switch (state) {
    case MorseState::START_ANIMATION: {
      unsigned long elapsed = now - start_anim_start_time;
      if ((elapsed / 1000) % 2 == 0)
        ring->lightSolid(ring_color);
      else
        ring->clearPixels();

      if (elapsed >= 4000) {
        ring->clearPixels();
        state = MorseState::SHOW_MORSE;
        lastActionTime = now;
      }

      checkForInput();
      break;
    }

    case MorseState::SHOW_MORSE: {
      if (currentLetterIdx >= scrambled.length()) {
        state = MorseState::WAIT_RESTART;
        lastActionTime = now;
        break;
      }

      if (currentSymbolIdx < currentMorse.length()) {
        char sym = currentMorse[currentSymbolIdx];

        if (!symbolOn && now - lastActionTime >= LETTER_PAUSE) {
          digitalWrite(MORSE_LED_PIN, HIGH);
          lastActionTime = now;
          symbolOn = true;
        }
        else if (symbolOn &&
                ((sym == '.' && now - lastActionTime >= DOT_DURATION) ||
                 (sym == '-' && now - lastActionTime >= DASH_DURATION))) {
          digitalWrite(MORSE_LED_PIN, LOW);
          lastActionTime = now;
          symbolOn = false;
          currentSymbolIdx++;
        }
      }
      else if (now - lastActionTime >= LETTER_PAUSE) {
        currentLetterIdx++;
        if (currentLetterIdx < scrambled.length()) {
          currentMorse = MORSE_TABLE[scrambled[currentLetterIdx] - 'a'];
          currentSymbolIdx = 0;
          lastActionTime = now;
        }
      }

      checkForInput();
      break;
    }

    case MorseState::WAIT_RESTART: {
      if (now - lastActionTime >= 2000) {
        currentLetterIdx = 0;
        currentSymbolIdx = 0;
        currentMorse = MORSE_TABLE[scrambled[0] - 'a'];
        symbolOn = false;
        lastActionTime = now;
        start_anim_start_time = now;
        state = MorseState::START_ANIMATION;
      }

      checkForInput();
      break;
    }

    case MorseState::END_ANIMATION: {
      endAnimation();
      break;
    }

    case MorseState::DONE:
      break;
  }
}

void Morse::endAnimation()
{
  if(end_anim_start_time != 0) // End animation already started
  {
    if(canDelete()) // finished end animation, clear board
    {
      ring->clearPixels();
      digitalWrite(MORSE_LED_PIN, LOW);
      state = MorseState::DONE;
      status = (status == Puzzle::puzzle_status::win_anim ? Puzzle::puzzle_status::win : Puzzle::puzzle_status::lose);
    }
    return;
  }
  
  end_anim_start_time = millis();
  mp3_player->playFilename(GAME_WIN_LOSE_SOUND_DIR, status == Puzzle::puzzle_status::win_anim ? GAME_WIN_SOUND : GAME_LOSE_SOUND);

  ring->lightSolid((status == Puzzle::puzzle_status::win_anim) ? ring_win_color : ring_lose_color);
  digitalWrite(MORSE_LED_PIN, HIGH);
  
}