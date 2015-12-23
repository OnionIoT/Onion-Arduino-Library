/*
 * OnionLibrary
 * Version 0.1 November, 2015
 * Copyright 2015 Onion Corporation

*/

#ifndef _ONION_LIBRARY_H_
#define _ONION_LIBRARY_H_


#include <Wire.h>
#include <avr/wdt.h>

#include <Adafruit_NeoPixel.h>


#define	REBOOT_STRING                  (0xdead)
#define	ARDUINO_DOCK_I2C_DEVICE_ADDR   (0x08)

#define ARDUINO_DOCK_ADDR_READBACK		(0x00)
#define	ARDUINO_DOCK_ADDR_RESET			(0xde)
#define ARDUINO_DOCK_CMD_RESET	 		(0xad)

#define ARDUINO_DOCK_ADDR_NEOPIXEL_INIT				(0x20)
#define ARDUINO_DOCK_ADDR_SET_NEOPIXEL_PIN			(0x21)
#define ARDUINO_DOCK_ADDR_SET_NEOPIXEL_STRIP_LENGTH	(0x22)
#define ARDUINO_DOCK_ADDR_SET_NEOPIXEL_BRIGHTNESS	(0x26)
#define ARDUINO_DOCK_ADDR_SET_NEOPIXEL_DATA			(0x23)
#define ARDUINO_DOCK_ADDR_SET_NEOPIXEL_DATAPOINT	(0x24)
#define ARDUINO_DOCK_ADDR_SHOW_NEOPIXEL				(0x25)


// function 
void 	ReceiveEvent    (int arg);
void 	RequestEvent    ();

// Onion Library Class
class Onion {
public:
	Onion();
	~Onion();

	void 	Reset 				(void);

	void 	ArduinoDockSetup	(uint8_t i2c_addr = ARDUINO_DOCK_I2C_DEVICE_ADDR);
	
	int 	ReceiveEventHandler		(uint8_t addr);
	int 	RequestEventHandler		(uint8_t &data);
	void 	SoftwareReset 			(void);
	
	

private:

	int 	_ReadTwiByte 	(uint8_t &rdByte);
	int 	_ReadTwiByte 	(int &rdByte);

	int 	_neopixelAcceptBuffer ();


	// private members
	int 		lastCmdAddr;
	int 		lastCmdData;

	int 		neopixelPin;
	int 		neopixelStripLength;
	int 		neopixelBrightness;
	uint8_t 	*neopixelBuffer;

	Adafruit_NeoPixel 	*neopixelStrip;

};

#endif // _ONION_LIBRARY_H_