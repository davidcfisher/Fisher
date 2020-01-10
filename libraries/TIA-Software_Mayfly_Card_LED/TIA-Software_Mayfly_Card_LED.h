//  TIA-Softwaref_Mayfly_Card_LED.h - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#ifndef TIA_SOFTWARE_MAYFLY_CARD_LED_H
#define TIA_SOFTWARE_MAYFLY_CARD_LED_H
#define TIA_SOFTWARE_MAYFLY_CARD_LED_VERSION 20200109

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
    void blink(                                             // METHOD: blink the light on and off
      int numberOfTimes = 2,                                // number of times to blink
      int millisecondsOnAndOff = 250                        // number of milliseconds each blink on, and following dark/off, should last
    );
    boolean getState();                                     // METHOD: get the state of the LED
    
  protected:
    int _pin;                                               // LED pin number
    String _LEDname;                                        // name of the LED
    boolean _state;                                         // state of the LED
};

#endif