//  LEDs_TIA-Software.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.

//  v1.0

#ifndef LEDS_TIA_SOFTWARE_H
#define LEDS_TIA_SOFTWARE_H

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"                        // global headers

class Red_LED {
  public:
    Red_LED();
    void turnOn(boolean debugFlag=false);                   // turn on the Red LED
    void turnOff(boolean debugFlag=false);                  // turn off the Red LED
};

class Green_LED {
  public:
    Green_LED();
    void turnOn(boolean debugFlag=false);                   // turn on the Green LED
    void turnOff(boolean debugFlag=false);                  // turn off the Red LED
};

#define redLedPin 9                                         // red LED connected to digital pin 9
#define greenLedPin 8                                       // green LED connected to digital pin 8

#endif