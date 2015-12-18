#include <OnionLibrary.h>

extern Onion* onionSetup;

// service routine to service write data request from I2C bus
void ReceiveEvent(int arg) {
	int      status;
	uint8_t  addr  = 0;

	// receive bytes from i2c bus
	while (Wire.available() > 0) {
		addr 	= Wire.read();

		// handle received commands and data
		status  = onionSetup->ReceiveEventHandler(addr);
	}

}

// service routine to service write data request from I2C bus
void RequestEvent() {
	int      status;
	uint8_t	 data	= 0;

	// respond on the i2c bus
	status 	= onionSetup->RequestEventHandler(data);

	Wire.write(data);

}