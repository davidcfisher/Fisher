//  LEDs_TIA-Software.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.

//  v1.0

#include "LEDs_TIA-Software.h"                                    // include the header file

Red_LED::Red_LED()
{
  pinMode(redLedPin, OUTPUT);                                     // set the red LED pin to OUTPUT
  digitalWrite(redLedPin, HIGH);                                  // initialize the red LED
}

Green_LED::Green_LED()
{
  pinMode(greenLedPin, OUTPUT);                                   // set the green LED pin to OUTPUT
  digitalWrite(greenLedPin, HIGH);                                // initialize the green LED
}

void Red_LED::turnOn(boolean debugFlag)
{
  digitalWrite(redLedPin, HIGH);                                  // turn on the red LED
  if (debugFlag) SerialMon.println("Red LED turned on");
}

void Red_LED::turnOff(boolean debugFlag)
{
  digitalWrite(redLedPin, LOW);                                   // turn off the red LED
  if (debugFlag) SerialMon.println("Red LED turned off");
}

void Green_LED::turnOn(boolean debugFlag)
{
  digitalWrite(greenLedPin, HIGH);                                // turn on the green LED
  if (debugFlag) SerialMon.println("Green LED turned on");
}

void Green_LED::turnOff(boolean debugFlag)
{
  digitalWrite(greenLedPin, LOW);                                 // turn off the green LED
  if (debugFlag) SerialMon.println("Green LED turned off");
}


