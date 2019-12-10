//  TIA-Software_Mayfly_Card_LED.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card_LED.h"                     // include the header file

// Constructor
LED::LED() {}                                                 // constructor

// Setup an LED
void LED::setup(int pin, String LEDname)
{
  _pin = pin;                                                 // save the pin number for later use
  _LEDname = LEDname;                                         // save the LED name for later use
  _state = false;
  
  pinMode(_pin, OUTPUT);                                      // make the pin an OUTPUT
  digitalWrite(_pin, LOW);                                    // turn the LED off
}

// Turn on an lED
void LED::turnOn()
{
  digitalWrite(_pin, HIGH);
  _state = true;
}

// Turn off an LED
void LED::turnOff()
{
  digitalWrite(_pin, LOW);
  _state = false;
}

// switch the state of the LED
void LED::switchState()
{
  digitalWrite(_pin,(_state ? LOW : HIGH));
  _state = !_state;
}
