#include "TIA-Software_Mayfly_Card.h"

boolean debugFlag = false;                            // true = print debug information
boolean testFlag = false;                             // true = perform SD card write, read and delete tests

Mayfly_card mayflyCard;                               // establish instance of Mayfly Card
  
void setup() 
{
  mayflyCard.setup(testFlag, debugFlag);              // setup the Mayfly Card
  mayflyCard.redLED.turnOn(debugFlag);                // turn on the Red LED
  mayflyCard.greenLED.turnOn(debugFlag);              // turn on the Green LED
  delay(2000);                                        // wait a couple of seconds
  mayflyCard.redLED.turnOff(debugFlag);               // turn off the red LED
  
  mayflyCard.SdCard.TIA_dir(debugFlag);               // list the SD Card directories & files
  
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.println(": exiting Sketch setup()."); }
}

void loop() {
}
