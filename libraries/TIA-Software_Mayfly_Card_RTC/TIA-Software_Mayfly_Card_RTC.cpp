//  TIA-Software_Mayfly_Card_RTC.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card_RTC.h"                     // include the header file


// CONSTRUCTOR
TIA_RTC::RTC() {}                                                 // constructor


// METHOD: setup an LED
void TIA_RTC::setup(int pin)
{
  _pin = pin;                                                 // save the pin number for later use
  
  pinMode(_pin, OUTPUT);                                      // make the pin an OUTPUT
}
