//  TIA-Software_Mayfly_Card.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card.h"                         // include the header file

#include <Wire.h>                                             // library to handle I2C, which is used to access Real Time Clock
#include <Sodaq_DS3231.h>                                     // library to handle Real Time Clock functions
#include <ArduinoJson.h>                                      // include the JSON library
  
File testFileObj2;                                         // create test file object
const char* testFilename2 = "TIA-Software_testFile.txt";   // create test file name
const String SdTestPhrase2 = "This is the test phrase!";   // create test phrase

/* FUNCTION to wait for the Mayfly's pushbutton to be pressed
    blinkLEDs     true = blink the LEDs while waiting for the button to be pushed
                  false = don't blink the LEDs
*/
//boolean waitForButtonPress(boolean blinkLEDs=false) {
//
//  // loop until the pushbutton is pressed
//  while (digitalRead(pushbuttonPin) == LOW) {
//
//    // blink the LEDs, if requested
//    if (blinkLEDs) {
//      if (redLedState == LOW) redLedState = HIGH; else redLedState = LOW;
//      digitalWrite(redLedPin, redLedState);
//      digitalWrite(greenLedPin, !redLedState);
//      delay(100);
//    }
//  }
//
//  // turn off the LEDs
//  digitalWrite(redLedPin, LOW);
//  digitalWrite(greenLedPin, LOW);
//} // end waitForButtonPress

Mayfly_card::Mayfly_card()
{
  pinMode(pushbuttonPin, INPUT);                              // set the pushbutton pin to INPUT so it can be read
};

LED Mayfly_card::redLED{};                                    // Red LED constructor
LED Mayfly_card::greenLED{};                                  // Green LED constructor
TIA_SdFat Mayfly_card::SdCard{};                              // SD Card constructor

void Mayfly_card::setup(boolean debugFlag)
{
  SerialMon.begin(57600);                                     // initialize the Serial Monitor
  delay (100);

  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.println(": starting Mayfly Card setup ==="); }
  
  redLED.setup(redLedPin, "Red LED", debugFlag);              // setup the red LED
  greenLED.setup(greenLedPin, "Green LED", debugFlag);        // setup the green LED
  SdCard.TIA_setup(debugFlag);                                // setup the SD card
  
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.println(": Mayfly Card setup complete. ==="); }

}
