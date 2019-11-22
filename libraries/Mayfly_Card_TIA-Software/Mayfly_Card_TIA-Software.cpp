//  Mayfly_Card_TIA-Software.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "Mayfly_Card_TIA-Software.h"                         // include the header file

Mayfly_card::Mayfly_card()
{
  pinMode(pushbuttonPin, INPUT);                              // set the pushbutton pin to INPUT so it can be read
  
  SdFat SD;                                                   // create SD Card object
}

void Mayfly_card::setup(boolean debugFlag)
{
  SerialMon.begin(57600);                                     // initialize the Serial Monitor
  
  delay (100);
  if (debugFlag) SerialMon.println("Exiting Mayfly_card::setup()");
}
