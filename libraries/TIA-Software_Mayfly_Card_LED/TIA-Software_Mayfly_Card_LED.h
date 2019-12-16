//  TIA-Softwaref_Mayfly_Card_LED.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_LED_H
#define TIA_SOFTWARE_MAYFLY_CARD_LED_H
#define TIA_SOFTWARE_MAYFLY_CARD_LED_VERSION 20191210

#include "Arduino.h"

class LED {
  public:
    LED();                                                  // constructor
    void setup(                                             // METHOD: setup the LED
      int pin,                                              // pin number for the LED
      String LEDname                                        // name for the LED
    );
    void setName(String name);                              // METHOD: set the LED name
    String getName();                                       // METHOD: get the LED name
    void turnOn();                                          // METHOD: turn on the LED
    void turnOff();                                         // METHOD: turn off the LED
    void switchState();                                     // METHOD: switch the light on or off
    boolean getState();                                     // METHOD: get the state of the LED
    
  protected:
    int _pin;                                               // LED pin number
    String _LEDname;                                        // name of the LED
    boolean _state;                                         // state of the LED
};

#endif