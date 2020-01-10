//  TIA-Software_Mayfly_Card.cpp - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#include "TIA-Software_Mayfly_Card.h"                         // include the header file

Mayfly_card::Mayfly_card() {};                                // constructor

TIA_RTC   Mayfly_card::rtc{};                                 // Mayfly has a Real Time Clock
LED       Mayfly_card::redLED{};                              // Mayfly has a Red LED 
LED       Mayfly_card::greenLED{};                            // Mayfly has a Green LED
TIA_SdFat Mayfly_card::sd{};                                  // Mayfly has an SD Card
BeeSocket Mayfly_card::bee{};                                 // Mayfly has a Bee Socket


// METHOD: setup the Mayfly Card
void Mayfly_card::setup(
  const char *id,                                             // Mayfly ID
  const char *dataHeader,                                     // definition of contents of measurement file
  const char *measurementFilename,                            // measurement filename
  const char *beeModule,                                      // identifier of the module in the Bee socket
  const char *location                                        // Mayfly deployment location
)
{
  strcpy (_id, id);                                           // save the ID
  strcpy (_dataHeader, dataHeader);                           // save the data headers
  strcpy (_measurementFilename, measurementFilename);         // save the measurement filenane
  strcpy (_beeModule, _beeModule);                            // save the identification of the module in the Bee socket
  strcpy (_location, location);                               // save the deployment location
  
  SerialMon.begin(57600);                                     // initialize the Serial Monitor
  delay (100);

  // setup the LEDs
  redLED.setup(TIA_redLedPin, "Red LED");                     // setup the red LED
  greenLED.setup(TIA_greenLedPin, "Green LED");               // setup the green LED
  
  // setup the SD card
  sd.TIA_setup(
    _id,                                                      // Mayfly ID
    _dataHeader,                                              // definition of contents of measurement file
    _measurementFilename,                                     // measurement filename
    _location                                                 // Mayfly deployment location
  );
  
  // setup the Bee Socket
  bee.setup(beeModule);
}


// METHOD:  get the Mayfly ID
char Mayfly_card::getId()
{
  return *_id;                                                 // return the Mayfly ID
}


// METHOD:  railroadLEDs() - blink the LEDs alternatingly
void Mayfly_card::railroadLEDs(
  int numberOfTimes,                                          // number of times to cycle
  int millisecondsOnAndOff                                    // number of milliseconds each blink on, and following dark/off, should last
)
{
  int halfCycles = numberOfTimes * 2;                         // half the cycle is LED on, half is LED off
  
  redLED.turnOff();                                           // start with the redLED off
  greenLED.turnOff();                                         // start with the greenLED off
  delay(millisecondsOnAndOff);                                // wait the requested time
  redLED.turnOn();
  
  // alternatingly, blink the LEDs
  for (int i=0; i < halfCycles; i++) {
    redLED.switchState();                                     // switch the on/off state
    greenLED.switchState();                                   // switch the on/off state
    delay(millisecondsOnAndOff);                              // wait the requested time
  }
  
  redLED.turnOff();
  greenLED.turnOff();
}
