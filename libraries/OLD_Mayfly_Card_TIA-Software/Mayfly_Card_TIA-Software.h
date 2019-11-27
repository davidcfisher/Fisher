//  TIA-Software_Mayfly_Card.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.

//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_H
#define TIA_SOFTWARE_MAYFLY_CARD_H

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"                           // global headers
#include "TIA-Software_Mayfly_Card_LED.h"                       // library to handle Mayfly LEDs
  
class Mayfly_card { 
  public: 
    Mayfly_card();                                              // constructor
    static LED redLED;                                          // Red LED
    static LED greenLED;                                        // Green LED
    void setup(boolean debugFlag=false);                        // setup the Mayfly card
};

#endif