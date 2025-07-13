void setup() {
	// Start the serial communication at 115200 baud rate
	Serial.begin(115200);
	
	// Wait for the serial port to initialize
	while (!Serial) {
	  ; // Wait for serial port to be ready
	}
	
	// Print a message to the Serial Monitor
	Serial.println("Serial Monitor Test Started");
	Serial.println("ESP32 is now communicating with the Serial Monitor!");
  }
  
  void loop() {
	// Print a message every 1 second
	Serial.println("Hello, this is a test message!");
	delay(1000); // Delay for 1 second
  }
  