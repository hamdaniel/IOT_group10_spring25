#ifndef WIRES_H
#define WIRES_H

#include <Arduino.h>
#include <PCF8574.h>
#include <Wire.h>
#include <BluetoothSerial.h>
#include <stdlib.h> 

#include "ledElement.h"
#include "sound.h"
#include "puzzle.h"

#define SDA_PIN 33
#define SCL_PIN 25
#define WIRE_BTN_PIN 14
#define COOLDOWN_MS 3000


class Wires : public Puzzle {
	private:
    enum CheckState {
		IDLE,
        SET_LOW,
        WAIT_STABLE,
        READ_INPUTS,
        SET_HIGH,
        FINISHED
    };
	
	PCF8574* outputExpander;
	PCF8574* inputExpander;

    CheckState checkState;

	int attempts;
	int left_attempts;
    int currentOutput;
    unsigned long checkTimestamp;
    int8_t detectedConnections[8];

    int8_t targetConnections[8];
    unsigned long lastTestTime;
    unsigned long lastDebounceTime;
    static constexpr unsigned long debounceDelay = 50;

    bool prevRawState;
    bool stableState;
    int num_wires;

    void printTarget();
    void analyzeDetectedConnections();

public:
    Wires(BluetoothSerial* bt, Mp3Player* mp3, LedElement* r, PCF8574* in,PCF8574* out, int n_w, int a);
    virtual ~Wires() override = default;

    void begin();
    void updateButton();
    void check();
    virtual void play() override;
    virtual void endAnimation() override;
};


#endif // WIRES_H
