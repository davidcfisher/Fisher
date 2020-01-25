//  TIA-Software_Mayfly_Card.cpp - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#include "TIA-Software_Mayfly_Card.h"                         // include the header file

Mayfly_card::Mayfly_card() {};                                // constructor

TIA_RTC   Mayfly_card::realTimeClock{};                       // Mayfly has a Real Time Clock
LED       Mayfly_card::redLED{};                              // Mayfly has a Red LED 
LED       Mayfly_card::greenLED{};                            // Mayfly has a Green LED
TIA_SdFat Mayfly_card::sdCard{};                              // Mayfly has an SD Card
BeeSocket Mayfly_card::beeSocket{};                           // Mayfly has a Bee Socket

// METHOD: setup the Mayfly Card
void Mayfly_card::setup(const char *beeModule = "none")
{
  SerialMon.begin(57600);                                     // initialize the Serial Monitor
  delay (100);

  redLED.setup(TIA_redLedPin, "Red LED");                     // setup the red LED
  greenLED.setup(TIA_greenLedPin, "Green LED");               // setup the green LED
  realTimeClock.TIA_setup();                                  // setup the real time clock
  sdCard.TIA_setup();                                         // setup the SD card
  beeSocket.setup(beeModule);                                 // setup the Bee Socket
}
