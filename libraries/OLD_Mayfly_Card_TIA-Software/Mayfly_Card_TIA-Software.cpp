//  TIA-Software_Mayfly_Card.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card.h"                           // include the header file
#include <Wire.h>                                               // library to handle I2C, which is used to access Real Time Clock
#include <Sodaq_DS3231.h>                                       // library to handle Real Time Clock functions
#include <ArduinoJson.h>                                        // include the JSON library
#include "SdFat.h"                                              // library to handle SD Card
  
// Mayfly card constructor  
Mayfly_card::Mayfly_card()                                      // constructor
{
  //pinMode(pushbuttonPin, INPUT);                              // set the pushbutton pin to INPUT so it can be read
  //SdFat SD;                                                   // create SD Card object
}

// include a red LED
LED Mayfly_card::redLED;

// include a green LED
LED Mayfly_card::greenLED;

// setup the Mayfly card
void Mayfly_card::setup(boolean debugFlag=false)
{
  SerialMon.begin(57600);                                     // initialize the Serial Monitor
  delay (100);
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__);SerialMon.println(": starting Mayfly Card setup..."); }
  
  // setup the red LED
  redLED.setup(redLedPin, "Red LED", debugFlag);
  
  // setup the green LED
  greenLED.setup(greenLedPin, "Green LED", debugFlag);
  
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__);SerialMon.println(": Mayfly Card setup complete."); }
}
