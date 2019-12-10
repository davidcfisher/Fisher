//  TIA-Softwaref_Mayfly_Card_LED.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.

//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_LED_H
#define TIA_SOFTWARE_MAYFLY_CARD_LED_H
#define TIA_SOFTWARE_MAYFLY_CARD_LED_VERSION 20191209

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"                       // global headers

class LED {
  public:
    LED();
    void setup(                                             // setup the LED
      int pin,                                              // pin number for the LED
      String LEDname                                        // name for the LED
    );
    void turnOn();                                          // turn on the LED
    void turnOff();                                         // turn off the LED
    void switchState();
  protected:
    int _pin;                                               // holds the pin number
    String _LEDname;                                        // name of the LED
    boolean _state;                                         // state of the LED
};

#endif