//  TIA-Software_Mayfly_Card.h - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#ifndef TIA_SOFTWARE_MAYFLY_CARD_H
#define TIA_SOFTWARE_MAYFLY_CARD_H
#define TIA_SOFTWARE_MAYFLY_CARD_VERSION 20200108

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"                         // global definitions
#include "TIA-Software_Mayfly_Card_RTC.h"                     // include the Real Time Clock library
#include "TIA-Software_Mayfly_Card_LED.h"                     // include the LED library
#include "TIA-Software_Mayfly_Card_SdFat.h"                   // include the SD Card library
#include "TIA-Software_Mayfly_Card_Bee.h"                     // include the Bee Socket library

class Mayfly_card { 
  public: 
    Mayfly_card();                                            // Mayfly card constructor
    static TIA_RTC    rtc;                                    // create real time clock instance "realTimeClock"
    static LED        redLED;                                 // create red led instance "redLED"
    static LED        greenLED;                               // create green led instance "greenLED"
    static TIA_SdFat  sd;                                     // create SD Card instance "SdCard"
    static BeeSocket  bee;                                    // create something on the BEE socket
    
    void setup(                                               // setup the Mayfly card
      const char *id,                                         // array holding the Mayfly ID
      const char *beeModule                                   // module in the Bee socket
    );
    
    char getId();                                             // get the Mayfly ID
  
  protected:
    char _id[5];                                              // Mayfly ID
};

#endif