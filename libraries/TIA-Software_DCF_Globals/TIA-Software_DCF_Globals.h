//  TIA-Software_DCF_Globals.h - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#ifndef TIA_SOFTWARE_DCF_GLOBALS_H
#define TIA_SOFTWARE_DCF_GLOBALS_H
#define TIA_SOFTWARE_DCF_GLOBALS_VERSION 20200107

#define SerialMon Serial                              // establish the Serial Monitor
#define SerialAT Serial1                              // establish the Cellular connection
#define TIA_redLedPin 9                               // red LED connected to digital pin 9
#define TIA_greenLedPin 8                             // green LED connected to digital pin 8
#define TIA_batteryPin A6                             // battery connected to Mayfly analog pin 6
#define TIA_pushbuttonPin 21                          // pushbutton connected to digital pin 21
#define TIA_SD_CS_PIN SS                              // pin "SS" is defined in the Arduino library
#define SD_CS_PIN SS				                  // I2C for SD Card Reader ??
#define SD_SS_PIN 12				                  // SD Card
#define V12_FACTOR 0.0361			                  // multiply this * A0 pin reading from 10:1 resistor divider to get 12v battery voltage.
#define consoleRecordLength 256                       // maximum length of a record in console.txt on SD card

#endif