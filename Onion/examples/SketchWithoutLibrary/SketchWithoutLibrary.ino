
#include <avr/wdt.h>
#include <Wire.h>

void setup() {
  // join I2C bus with address 0x08
  Wire.begin(0x08);
  // register handler function when data is received
  Wire.onReceive(receiveEvent);
 
}

void loop() {
  delay(100);
  // your code here
}


// Restarts program from beginning but does not reset the peripherals and registers
void _SoftwareReset() {
  wdt_enable(WDTO_15MS);
} 


// function that executes whenever data is received from master
void receiveEvent(int arg) {
  int addr;
  int data;

  // receive bytes from i2c bus
  while (Wire.available() > 0) {
    // read the register address being written to
    addr  = Wire.read();

    // handle received commands and data
    if (Wire.available() > 0) {
      data = Wire.read();

      // the reset 
      if (addr == 0xde && data == 0xad) {
        _SoftwareReset();
      }
    }
  }
}
