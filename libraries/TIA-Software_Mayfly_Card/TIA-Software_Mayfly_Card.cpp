//  TIA-Software_Mayfly_Card.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card.h"                         // include the header file

Mayfly_card::Mayfly_card() {};                                // constructor

LED Mayfly_card::redLED{};                                    // Mayfly has a Red LED 
LED Mayfly_card::greenLED{};                                  // Mayfly has a Green LED 
TIA_SdFat Mayfly_card::SdCard{};                              // Mayfly has an SD Card 

// METHOD: setup the Mayfly Card
void Mayfly_card::setup(boolean testFlag, boolean debugFlag)
{
  SerialMon.begin(57600);                                     // initialize the Serial Monitor
  delay (100);

  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(F(", line "));SerialMon.print(__LINE__); SerialMon.println(F(": starting Mayfly Card setup ===")); }
  
  redLED.setup(redLedPin, "Red LED", debugFlag);              // setup the red LED
  greenLED.setup(greenLedPin, "Green LED", debugFlag);        // setup the green LED
  SdCard.TIA_setup(testFlag, debugFlag);                      // setup the SD card
  
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(F(", line "));SerialMon.print(__LINE__); SerialMon.println(F(": Mayfly Card setup complete. ===")); SerialMon.println(F("")); }
}
