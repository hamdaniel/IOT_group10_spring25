#include "Wires.h"

Wires::Wires(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, PCF8574* in, PCF8574* out, int n_w, int a)
    : Puzzle(bt, mp3, r),
	  inputExpander(in),
	  outputExpander(out),
      lastTestTime(0),
      lastDebounceTime(0),
      prevRawState(HIGH),
      stableState(HIGH),
      num_wires(n_w),
	  attempts(a),
	  left_attempts(a),
      checkState(IDLE),
      currentOutput(0),
      checkTimestamp(0)
{
    for (int i = 0; i < 8; i++)
	{
        targetConnections[i] = -1;
        detectedConnections[i] = -1;
    }

    int outputs[8], inputs[8];

    for (int i = 0; i < 8; i++)
	{
        outputs[i] = i;
        inputs[i] = i;
    }

    for (int i = 7; i > 0; i--)
	{
        int j = random(i + 1);
        std::swap(outputs[i], outputs[j]);
        j = random(i + 1);
        std::swap(inputs[i], inputs[j]);
    }

    for (int i = 0; i < num_wires && i < 8; i++)
        targetConnections[outputs[i]] = inputs[i];

	printTarget();
}

void Wires::printTarget() {
    Serial.println("Target Wiring:");
    for (int i = 0; i < 8; i++)
	{
        Serial.print("Output ");
        Serial.print(i);
        Serial.print(" -> ");
        if (targetConnections[i] == -1)
            Serial.println("Not Connected");
        else
		{
            Serial.print("Input ");
            Serial.println(targetConnections[i]);
        }
    }
    Serial.println("---");
}

void Wires::updateButton() {
    unsigned long now = millis();
    bool raw = digitalRead(WIRE_BTN_PIN);

    if (raw != prevRawState)
        lastDebounceTime = now;

    if (now - lastDebounceTime > debounceDelay)
	{
        if (raw != stableState)
		{
            stableState = raw;

            if (stableState == LOW && now - lastTestTime >= COOLDOWN_MS && checkState == IDLE)
			{
				check();
				lastTestTime = now;
            }
        }
    }

    prevRawState = raw;
}

void Wires::check() {
    switch (checkState) {
        case IDLE:
		{
			for (int i = 0; i < 8; i++)
			{
				detectedConnections[i] = -1;
                outputExpander->write(i, HIGH);
            }
            currentOutput = 0;
            checkState = SET_LOW;
            break;
		}

        case SET_LOW:
		{
			outputExpander->write(currentOutput, LOW);
            checkTimestamp = millis();
            checkState = WAIT_STABLE;
            break;
		}

        case WAIT_STABLE:
		{
			if (millis() - checkTimestamp >= 5)
				checkState = READ_INPUTS;
    
            break;
		}

        case READ_INPUTS:
		{
			for (int j = 0; j < 8; j++)
			{
				if (inputExpander->read(j) == LOW)
				{
					detectedConnections[currentOutput] = j;
                    break;
                }
            }
            checkState = SET_HIGH;
            break;
		}

        case SET_HIGH:
		{
			outputExpander->write(currentOutput, HIGH);
            currentOutput++;
            if (currentOutput >= 8)
				checkState = FINISHED;
            else
				checkState = SET_LOW;
            
            break;
		}

        case FINISHED:
		{
			analyzeDetectedConnections();
            checkState = IDLE;
            break;
		}
    }
}

void Wires::analyzeDetectedConnections() {
    Serial.println("Detected Wiring:");
    for (int i = 0; i < 8; i++)
	{
        Serial.print("Output ");
        Serial.print(i);
        Serial.print(" -> ");
        if (detectedConnections[i] == -1)
            Serial.println("Not Connected");
        else
		{
            Serial.print("Input ");
            Serial.println(detectedConnections[i]);
        }
    }

    int perfect = 0;
	int leftCorrectRightNot = 0;
	int rightCorrectLeftNot = 0;

	// First pass: find perfect matches and left-correct/right-not
	for (int i = 0; i < 8; i++) {
		int expectedInput = targetConnections[i];
		int actualInput = detectedConnections[i];

		if (expectedInput == -1 && actualInput == -1) {
			continue; // both unconnected, fine
		}

		if (expectedInput == actualInput) {
			perfect++;
		} else if (expectedInput != -1 && actualInput != -1 && expectedInput != actualInput) {
			leftCorrectRightNot++;
		}
	}

	// Second pass: check for rightCorrectLeftNot
	for (int j = 0; j < 8; j++) {
		// Find if this input j is expected somewhere
		bool inputExpected = false;
		for (int i = 0; i < 8; i++) {
			if (targetConnections[i] == j) {
				inputExpected = true;
				break;
			}
		}

		// Now see if it's used, but by the wrong output
		bool inputDetected = false;
		bool isPerfect = false;

		for (int i = 0; i < 8; i++) {
			if (detectedConnections[i] == j) {
				inputDetected = true;
				if (targetConnections[i] == j) {
					isPerfect = true;
				}
				break;
			}
		}

		if (inputExpected && inputDetected && !isPerfect) {
			rightCorrectLeftNot++;
		}
	}

	Serial.println("---");
	if (perfect == num_wires) {
		Serial.println("SUCCESS: Wiring matches the target!");
		status = Puzzle::puzzle_status::win_anim;
	}
	else {
		Serial.println("FAILURE: Wiring does not match.");
		Serial.println("Hints:");
		Serial.print("- ");
		Serial.print(perfect);
		Serial.println(" wires are perfectly placed (correct left and right).");
		Serial.print("- ");
		Serial.print(leftCorrectRightNot);
		Serial.println(" wires are connected from the correct left pin, but to the wrong input.");
		Serial.print("- ");
		Serial.print(rightCorrectLeftNot);
		Serial.println(" wires are connected to the correct right pin, but from the wrong output.");
		Serial.println(" Any other wire is incorrect on both sides.");

		left_attempts--;
		if(!left_attempts)
			status = Puzzle::puzzle_status::lose_anim;
	}
	
	serialBT->println(String(perfect) + "," + String(leftCorrectRightNot) + "," + String(rightCorrectLeftNot));
	
}

void Wires::play() {
	switch (status)
	{
	case Puzzle::puzzle_status::not_finished:
	{
		updateButton();
    	if (checkState != IDLE)
			check();
		
		ring->clearPixels();
		double fraction = (double)left_attempts / (double)attempts;
		ring->lightFraction(fraction, ring_color);
		break;
	}
	
	case Puzzle::puzzle_status::win_anim:
	{
		endAnimation();
		break;
	}

	case Puzzle::puzzle_status::lose_anim:
	{
		endAnimation();
		break;
	}

	default:
		break;
	}
    
}

void Wires::endAnimation() {
    if(end_anim_start_time != 0) // End animation already started
	{
		if(canDelete()) // finished end animation, clear board
			status = (status == Puzzle::puzzle_status::win_anim ? Puzzle::puzzle_status::win : Puzzle::puzzle_status::lose);
		
		return;
	}
	
	end_anim_start_time = millis();
	mp3_player->playFilename(GAME_WIN_LOSE_SOUND_DIR, status == Puzzle::puzzle_status::win_anim ? GAME_WIN_SOUND : GAME_LOSE_SOUND);

	ring->lightSolid((status == Puzzle::puzzle_status::win_anim) ? ring_win_color : ring_lose_color);
}
