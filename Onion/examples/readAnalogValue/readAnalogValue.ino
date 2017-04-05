int analogPin = A0;

void setup() {
	// start serial
	Serial.begin(9600);
}

void loop() {
	// read the value on the analog pin
	int analogValue = analogRead(analogPin);

	// check for input on serial port
	if (Serial.available() > 0) {
		// read the input
		int inByte = Serial.read();

		// respond only if correct command is received
		if (inByte == (int)'r') {
			// respond with analog measurement
			Serial.println(analogValue, DEC);
		}
	}
}
