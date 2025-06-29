#include <Wire.h>
#include <PCF8574.h>

#define SDA_PIN 33
#define SCL_PIN 25
#define WIRE_BTN_PIN 14
#define COOLDOWN_MS 3000

PCF8574 outputExpander(0x20);  // Outputs
PCF8574 inputExpander(0x21);   // Inputs

class WireTester {
private:
  int8_t targetConnections[8]; // output[i] should connect to input[targetConnections[i]], or -1
  unsigned long lastTestTime;
  unsigned long lastDebounceTime;
  static constexpr unsigned long debounceDelay = 50;
  bool prevRawState = HIGH;
  bool stableState = HIGH;

  void generateTargetConnections(int x = 6) {
    // Initialize all as unconnected
    for (int i = 0; i < 8; i++) {
      targetConnections[i] = -1;
    }

    // Prepare shuffled output and input indices
    int outputs[8], inputs[8];
    for (int i = 0; i < 8; i++) {
      outputs[i] = i;
      inputs[i] = i;
    }

    for (int i = 7; i > 0; i--) {
      int j = random(i + 1);
      std::swap(outputs[i], outputs[j]);
      j = random(i + 1);
      std::swap(inputs[i], inputs[j]);
    }

    if (x > 8) x = 8;

    for (int i = 0; i < x; i++) {
      int out = outputs[i];
      int in = inputs[i];
      targetConnections[out] = in;
    }
  }

  void printTarget() {
    Serial.println("Target Wiring:");
    for (int i = 0; i < 8; i++) {
      Serial.print("Output ");
      Serial.print(i);
      Serial.print(" -> ");
      if (targetConnections[i] == -1) {
        Serial.println("Not Connected");
      } else {
        Serial.print("Input ");
        Serial.println(targetConnections[i]);
      }
    }
    Serial.println("---");
  }

public:
  void begin() {
    generateTargetConnections();
    printTarget();
    lastTestTime = 0;
    lastDebounceTime = 0;
  }

  void updateButton() {
    unsigned long now = millis();
    bool raw = digitalRead(WIRE_BTN_PIN);

    if (raw != prevRawState) {
      lastDebounceTime = now;
    }

    if (now - lastDebounceTime > debounceDelay) {
      if (raw != stableState) {
        stableState = raw;

        if (stableState == LOW) {
          if (now - lastTestTime < COOLDOWN_MS) {
            Serial.println("Cooldown active. Please wait.");
          } else {
            check();
            lastTestTime = now;
          }
        }
      }
    }

    prevRawState = raw;
  }

  void check() {
  int8_t detectedConnections[8];  // detectedConnections[i] = input j or -1
  for (int i = 0; i < 8; i++) detectedConnections[i] = -1;

  // Set all outputs HIGH
  for (int i = 0; i < 8; i++) outputExpander.write(i, HIGH);

  // Scan
  for (int i = 0; i < 8; i++) {
    outputExpander.write(i, LOW);
    delay(5);

    for (int j = 0; j < 8; j++) {
      if (inputExpander.read(j) == LOW) {
        detectedConnections[i] = j;
        break;  // only one input per output
      }
    }

    outputExpander.write(i, HIGH);
  }

  // Print detected connections
  Serial.println("Detected Wiring:");
  for (int i = 0; i < 8; i++) {
    Serial.print("Output ");
    Serial.print(i);
    Serial.print(" -> ");
    if (detectedConnections[i] == -1) {
      Serial.println("Not Connected");
    } else {
      Serial.print("Input ");
      Serial.println(detectedConnections[i]);
    }
  }

  // Analyze
  int perfect = 0;
  int wrongInput = 0;
  int wrongOutput = 0;

  bool inputUsed[8] = { false };  // track inputs used in target
  bool inputSeen[8] = { false };  // track inputs seen in detected

  for (int i = 0; i < 8; i++) {
    int expected = targetConnections[i];
    int actual = detectedConnections[i];

    if (expected != -1) inputUsed[expected] = true;
    if (actual != -1) inputSeen[actual] = true;

    if (expected == -1 && actual == -1) {
      continue;  // both unconnected, fine
    } else if (expected == actual) {
      perfect++;
    } else if (expected != -1 && actual != -1) {
      // Connected but to wrong input
      wrongInput++;
    }
  }

  // Count inputs that were connected to the wrong output
  for (int j = 0; j < 8; j++) {
    if (inputSeen[j] && inputUsed[j]) {
      // input is expected and detected, but not in a perfect match
      bool matched = false;
      for (int i = 0; i < 8; i++) {
        if (targetConnections[i] == j && detectedConnections[i] == j) {
          matched = true;
          break;
        }
      }
      if (!matched) {
        wrongOutput++;
      }
    }
  }

  // Decide
  Serial.println("---");
  if (perfect == 8) {
    Serial.println("SUCCESS: Wiring matches the target!");
  } else {
    Serial.println("FAILURE: Wiring does not match.");
    Serial.println("Hints:");
    Serial.print("- ");
    Serial.print(perfect);
    Serial.println(" wires are perfectly placed.");
    Serial.print("- ");
    Serial.print(wrongInput);
    Serial.println(" outputs are connected to the wrong input.");
    Serial.print("- ");
    Serial.print(wrongOutput);
    Serial.println(" inputs are connected to the wrong output.");
  }
}

};
WireTester tester;


void setup() {
  Serial.begin(115200);
  delay(10000);
  Wire.begin(SDA_PIN, SCL_PIN);
  outputExpander.begin();
  inputExpander.begin();

  pinMode(WIRE_BTN_PIN, INPUT_PULLUP);

  // Set all outputs HIGH initially
  for (int i = 0; i < 8; i++) {
    outputExpander.write(i, HIGH);
  }

  tester.begin();
}

void loop() {
  tester.updateButton();
}
