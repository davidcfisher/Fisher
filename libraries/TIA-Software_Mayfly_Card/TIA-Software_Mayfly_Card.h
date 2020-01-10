//  TIA-Software_Mayfly_Card.h - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#ifndef TIA_SOFTWARE_MAYFLY_CARD_H
#define TIA_SOFTWARE_MAYFLY_CARD_H
#define TIA_SOFTWARE_MAYFLY_CARD_VERSION 20200110

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
      const char *dataHeader,                                 // array holding definition of contents of measurement file
      const char *measurementFilename,                        // array holding the measurement filename
      const char *beeModule,                                  // module in the Bee socket
      const char *location                                    // Mayfly deployment location
    );
    
    char getId();                                             // get the Mayfly ID
    
    void railroadLEDs(
      int numberOfTimes = 20,                                 // number of times to cycle
      int millisecondsOnAndOff = 250                          // number of milliseconds each blink on, and following dark/off, should last
    );
  
  protected:
    char _id[5];                                              // Mayfly ID
    char _dataHeader[256];                                    // definition of contents of measurement file
    char _measurementFilename[50];                            // measurement filename
    char _location[100];                                      // Mayfly deployment location
    char _beeModule[];
};

#endif