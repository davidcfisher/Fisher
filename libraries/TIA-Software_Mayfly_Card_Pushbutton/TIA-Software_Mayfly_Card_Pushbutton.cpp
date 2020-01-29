//  TIA-Software_Mayfly_Card_Pushbutton.cpp - Copyright (c) 2020 TIA Software, LLC.  All rights reserved.

#include "TIA-Software_Mayfly_Card_Pushbutton.h"              // include the header file
#include "TIA-Software_DCF_Globals.h"

// CONSTRUCTOR
Pushbutton::Pushbutton()                                      // constructor
{
  boolean _state = LOW;                                       // initialize the pushbutton state
}


// METHOD: setup the pushbutton
void Pushbutton::setup()
{
  pinMode(TIA_pushbuttonPin, INPUT);                          // the pushbutton pin is used as input
}


// METHOD: read the state of the pushbutton
boolean Pushbutton::readState()
{
   _state = digitalRead(TIA_pushbuttonPin);                   // read the state of the pushbutton
  return _state ? HIGH : LOW;                                 // return the state as HIGH or LOW
}
