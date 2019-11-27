//  TIA-Softwaref_Mayfly_Card_LED.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.

//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_LED_H
#define TIA_SOFTWARE_MAYFLY_CARD_LED_H

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"                       // global headers

class LED {
  public:
    LED();
    void setup(int pin, String LEDname, boolean debugFlag=false);           // setup the LED
    void turnOn(boolean debugFlag=false);                   // turn on the LED
    void turnOff(boolean debugFlag=false);                  // turn off the LED
  protected:
    int _pin;                                               // holds the pin number
    String _LEDname;                                        // name of the LED
};

#endif