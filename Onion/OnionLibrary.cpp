#include "OnionLibrary.h"

// constructor
Onion::Onion() 
{
	// reset class variables
	Reset();

	// Arduino Dock setup
	ArduinoDockSetup();
}

// destructor
Onion::~Onion()
{
	// nothing for now
}

// reset function
void Onion::Reset()
{
	neopixelPin				= 0;
	neopixelStripLength		= 0;
	neopixelBrightness 		= 0xff;
	neopixelStrip			= NULL;
}


//// private functions
// read byte from I2C interface
//	 return 0 if byte is valid, 1 if byte is invalid
int Onion::_ReadTwiByte (uint8_t &rdByte) 
{
	int status	= EXIT_FAILURE;

	if (Wire.available() > 0) {
		rdByte	= Wire.read();
		status	= EXIT_SUCCESS;
	}

	return status;
}

int Onion::_ReadTwiByte (int &rdByte) 
{
	int status	= EXIT_FAILURE;
	uint8_t		byte;

	// use the uint8_t version
	status 	= _ReadTwiByte(byte);
	rdByte 	= (int)byte;

	return status;
}


//// public functions
// enable I2C interface and setup ISR
void Onion::ArduinoDockSetup(uint8_t i2c_addr) {
	// disable watchdog timer
	wdt_disable();

	// join i2c bus 
	Wire.begin(i2c_addr);
	
	// register service routine
	Wire.onReceive(&ReceiveEvent);
	Wire.onRequest(&RequestEvent);
}

// Restarts program from beginning but does not reset the peripherals and registers
void Onion::SoftwareReset() {
	wdt_enable(WDTO_15MS);
} 

// perform a specific action based on the i2c address received
int Onion::ReceiveEventHandler(uint8_t addr)
{
	int 	status;
	int 	i, red, green, blue;
	uint8_t data;

	switch(addr) {
		case (ARDUINO_DOCK_ADDR_READBACK):
			// set the last command to: the address contained in the data
			status == _ReadTwiByte(data);
			if (status == EXIT_SUCCESS) {
				addr 	= data;
			}

			break;

		case (ARDUINO_DOCK_ADDR_RESET):
			status == _ReadTwiByte(data);
			if (status == EXIT_SUCCESS && data == ARDUINO_DOCK_CMD_RESET) {
				SoftwareReset();
			}

			break;

		case (ARDUINO_DOCK_ADDR_NEOPIXEL_INIT):
			status |= _ReadTwiByte(data);
			if (status == EXIT_SUCCESS && data != 0) {
				// set the selected pin to output 
				pinMode(neopixelPin, OUTPUT);
				// initialize the neopixel object
				neopixelStrip = new Adafruit_NeoPixel(neopixelStripLength, neopixelPin, NEO_GRB + NEO_KHZ800);
			}
			break;
		 
		case (ARDUINO_DOCK_ADDR_SET_NEOPIXEL_PIN):
			status |= _ReadTwiByte(data);
			if (status == EXIT_SUCCESS) {
				neopixelPin = data;
			}
			break;

		case (ARDUINO_DOCK_ADDR_SET_NEOPIXEL_STRIP_LENGTH):
			status |= _ReadTwiByte(data);
			if (status == EXIT_SUCCESS) {
				neopixelStripLength = data;
			}
			break;

		case (ARDUINO_DOCK_ADDR_SET_NEOPIXEL_BRIGHTNESS):
			status |= _ReadTwiByte(data);
			if (status == EXIT_SUCCESS) {
				neopixelBrightness = data;

				// change the brightness (if neopixel object has been initialized)
				if (neopixelStrip != NULL) {
					neopixelStrip->setBrightness(neopixelBrightness);
				}
			}
			break;

		case (ARDUINO_DOCK_ADDR_SET_NEOPIXEL_DATA):
			_neopixelAcceptBuffer();
			break;

		case (ARDUINO_DOCK_ADDR_SET_NEOPIXEL_DATAPOINT):
			// read pixel index, and colour values
			status |= _ReadTwiByte(i);
			status |= _ReadTwiByte(red);
			status |= _ReadTwiByte(green);
			status |= _ReadTwiByte(blue);

			// set the datapoint info
			if (status == EXIT_SUCCESS) {
				neopixelStrip->setPixelColor(i, red, green, blue);
			}
			lastCmdData 	= i;
			break;

		case (ARDUINO_DOCK_ADDR_SHOW_NEOPIXEL):
			status |= _ReadTwiByte(data);
			if (status == EXIT_SUCCESS && data != 0) {
				neopixelStrip->show();
			}

		default:
			break;
	}

	// note the last address
	lastCmdAddr 	= addr;

	return status;
}

int Onion::_neopixelAcceptBuffer ()
{
	int 	status;
	int 	i, length, startPoint;
	int 	red, green, blue;
	uint8_t data;

	// read the incoming transmission length and starting index
	status 	=  _ReadTwiByte(length);
	status 	|= _ReadTwiByte(startPoint);

	// read a buffer of neopixel pixels (3 bytes)
	for (i = startPoint; i < (length + startPoint); i++) {
		if (Wire.available() < 3) {
			status 	= EXIT_FAILURE;
			break;
		}

		red	 	= Wire.read();
		green 	= Wire.read();
		blue	= Wire.read();

		neopixelStrip->setPixelColor(i, red, green, blue);
	}
	
	lastCmdData 	= i;

	return status;
}

// perform a specific action based on the i2c address received
int Onion::RequestEventHandler(uint8_t &data)
{
	int 	status = EXIT_SUCCESS;;

	// respond based on the last addr that was written to
	switch(lastCmdAddr) {
		case (ARDUINO_DOCK_ADDR_RESET):
			data 	= 0xBE;
			break;

		case (ARDUINO_DOCK_ADDR_SET_NEOPIXEL_PIN):
			data 	= (uint8_t)neopixelPin;
			break;

		case (ARDUINO_DOCK_ADDR_SET_NEOPIXEL_STRIP_LENGTH):
			data 	= (uint8_t)neopixelStripLength;
			break;

		case (ARDUINO_DOCK_ADDR_SET_NEOPIXEL_BRIGHTNESS):
			data 	= (uint8_t)neopixelBrightness;
			break;

		case (ARDUINO_DOCK_ADDR_SET_NEOPIXEL_DATA):
		case (ARDUINO_DOCK_ADDR_SET_NEOPIXEL_DATAPOINT):
			data 	= (uint8_t)lastCmdData;
			break;

		default:
			status 	= EXIT_FAILURE;
			data 	= 0xff;
			break;
	}

	return status;
}