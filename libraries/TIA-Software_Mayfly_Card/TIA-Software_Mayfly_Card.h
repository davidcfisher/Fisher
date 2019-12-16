//  TIA-Software_Mayfly_Card.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_H
#define TIA_SOFTWARE_MAYFLY_CARD_H
#define TIA_SOFTWARE_MAYFLY_CARD_VERSION 20191209

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"                         // global definitions
#include "TIA-Software_Mayfly_Card_RTC.h"                     // include the Real Time Clock library
#include "TIA-Software_Mayfly_Card_LED.h"                     // include the LED library
#include "TIA-Software_Mayfly_Card_SdFat.h"                   // include the SD Card library


class Mayfly_card { 
  public: 
    Mayfly_card();                                            // Mayfly card constructor
    static TIA_RTC    realTimeClock;                          // create real time clock instance "realTimeClock"
    static LED        redLED;                                 // create red led instance "redLED"
    static LED        greenLED;                               // create green led instance "greenLED"
    static TIA_SdFat  SdCard;                                 // create SD Card instance "SdCard"
    
    void setup(boolean testFlag=false);                       // method to setup the Mayfly card.  true=do write, read and remove tests
};

#endif