//  TIA-Software_Mayfly_Card_Pushbutton.cpp - Copyright (c) 2020 TIA Software, LLC.  All rights reserved.

#include "TIA-Software_Mayfly_Card_Pushbutton.h"              // include the header file
#include "TIA-Software_DCF_Globals.h"

// CONSTRUCTOR
Pushbutton::Pushbutton()                                      // constructor
{
  boolean _state = LOW;                                       // initialize the pushbutton state
}


// METHOD: setup module in bee socket
void Pushbutton::setup()
{
  pinMode(TIA_pushbuttonPin, INPUT);                          // the pushbutton pin is used as input
}


// METHOD: read the state of the pushbutton
boolean Pushbutton::readState()
{
   _state = digitalRead(TIA_pushbuttonPin);
  //Serial.print("Pushbutton state=");Serial.println(_state ? "Pushed" : "Not pushed");
  return _state;
}
