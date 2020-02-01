//  TIA-Software_Mayfly_Card.h - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#ifndef TIA_SOFTWARE_MAYFLY_CARD_H
#define TIA_SOFTWARE_MAYFLY_CARD_H
#define TIA_SOFTWARE_MAYFLY_CARD_VERSION 20200129

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"                         // global definitions
#include "TIA-Software_Mayfly_Card_RTC.h"                     // include the Real Time Clock library
#include "TIA-Software_Mayfly_Card_LED.h"                     // include the LED library
#include "TIA-Software_Mayfly_Card_Pushbutton.h"              // include the Pushbutton library
#include "TIA-Software_Mayfly_Card_SdFat.h"                   // include the SD Card library
#include "TIA-Software_Mayfly_Card_Bee.h"                     // include the Bee Socket library

class Mayfly_card { 
  public: 
    Mayfly_card();                                            // Mayfly card constructor
    static TIA_RTC    realTimeClock;                          // create real time clock instance "realTimeClock"
    static LED        redLED;                                 // create red led instance "redLED"
    static LED        greenLED;                               // create green led instance "greenLED"
    static Pushbutton pushbutton;                             // create Pushbutton object
    static TIA_SdFat  sdCard;                                 // create SD Card instance "SdCard"
    static BeeSocket  beeSocket;                              // create something on the BEE socket
    
    bool setup(                                               // setup the Mayfly card.  
      const char *beeModule = "none"                          // module in the Bee socket
    );
    
    void railroadLED(                                         // alternately flash the LEDs
      int cycleCount = 10,                                    // number of cycles to flash, negative number means railroad forever
      boolean enablePushbutton = true,                        // true=button push terminates loop
      int cycleMs = 1000                                      // flash cycle in ms
    );
    
    void railroadLED(                                         // alternately flash the LEDs
      String cycleCount,                                      // String defining number of cycles
      boolean enablePushbotton = true,                        // true=button push terminates loop
      int cycleMs = 1000                                      // flash cycle in ms
    );
};

#endif