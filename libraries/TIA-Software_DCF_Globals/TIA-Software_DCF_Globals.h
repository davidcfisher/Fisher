//  TIA-Software_DCF_Globals.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_DCF_GLOBALS_H
#define TIA_SOFTWARE_DCF_GLOBALS_H
#define TIA_SOFTWARE_DCF_GLOBALS_VERSION 20191205

#include "Arduino.h"

#define SerialMon Serial                              // establish the Serial Monitor
#define SerialAT Serial1        `                     // establish the Cellular connection
#define redLedPin 9                                   // red LED connected to digital pin 9
#define greenLedPin 8                                 // green LED connected to digital pin 8
#define batteryPin A6                                 // battery connected to Mayfly analog pin 6
#define pushbuttonPin 21                              // pushbutton connected to digital pin 21
#define SD_CS_PIN SS                                  // pin "SS" is defined in the Arduino library

#endif