//  TIA-Software_Mayfly_Card_LED.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card_LED.h"                     // include the header file

// Constructor
LED::LED()                                                    // constructor
{
}

// Setup an LED
void LED::setup(int pin, String LEDname, boolean debugFlag=false)
{
  _pin = pin;                                                 // save the pin number for later use
  _LEDname = LEDname;                                         // save the LED name for later use
  
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.print(": setting up "); SerialMon.print(_LEDname); SerialMon.print(" on pin ");SerialMon.print(_pin);SerialMon.print("..."); }
  pinMode(_pin, OUTPUT);                                      // make the pin an OUTPUT
  digitalWrite(_pin, LOW);                                    // turn the LED off
  if (debugFlag) { SerialMon.println("...complete."); }
}

// Turn on an lED
void LED::turnOn(boolean debugFlag=false)
{
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.print(": turning "); SerialMon.print(_LEDname); SerialMon.print(" on..."); }
  digitalWrite(_pin, HIGH);
  if (debugFlag) { SerialMon.print("...turned "); SerialMon.print(_LEDname); SerialMon.println(" on."); }
}

// Turn off an LED
void LED::turnOff(boolean debugFlag=false)
{
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.print(": turning "); SerialMon.print(_LEDname); SerialMon.print(" off..."); }
  digitalWrite(_pin, LOW);
  if (debugFlag) { SerialMon.print("...turned "); SerialMon.print(_LEDname); SerialMon.println(" off."); }
}
