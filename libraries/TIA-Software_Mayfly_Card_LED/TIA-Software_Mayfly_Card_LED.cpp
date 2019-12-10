//  TIA-Software_Mayfly_Card_LED.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card_LED.h"                     // include the header file

// CONSTRUCTOR
LED::LED() {}                                                 // constructor

// METHOD: setup an LED
void LED::setup(int pin, String LEDname)
{
  _pin = pin;                                                 // save the pin number for later use
  _LEDname = LEDname;                                         // save the LED name for later use
  
  pinMode(_pin, OUTPUT);                                      // make the pin an OUTPUT
  turnOff();                                                  // turn the LED off
}

// METHOD: turn the lED on
void LED::turnOn()
{
  digitalWrite(_pin, HIGH);
  _state = HIGH;
}

// METHOD: turn the LED off
void LED::turnOff()
{
  digitalWrite(_pin, LOW);
  _state = LOW;
}

// METHOD: switch the state of the LED
void LED::switchState()
{
  digitalWrite(_pin, (_state ? LOW : HIGH));
  _state = !_state;
}
