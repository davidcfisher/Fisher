#include "TIA-Software_DCF_Globals.h"
#include "TIA-Software_Mayfly_Card.h"

boolean debugFlag = false;

// establish instance of Mayfly Card
Mayfly_card mayflyCard;
  
void setup() 
{
  mayflyCard.setup(debugFlag);
  mayflyCard.redLED.turnOn();
  mayflyCard.greenLED.turnOn();
  mayflyCard.SdCard.TIA_ls(debugFlag);
  delay(2000);
  mayflyCard.redLED.turnOff();
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.println(": exiting Sketch setup()."); }
}

void loop() {
}
