void setup() {
	pinMode(2, OUTPUT); // GPIO2 is typically connected to onboard LED
  }
  
  void loop() {
	digitalWrite(2, HIGH);
	delay(100);
	digitalWrite(2, LOW);
	delay(100);
  }