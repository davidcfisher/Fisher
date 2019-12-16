//  TIA-Softwaref_Mayfly_Card_RTC.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_RTC_H
#define TIA_SOFTWARE_MAYFLY_CARD_RTC_H
#define TIA_SOFTWARE_MAYFLY_CARD_RTC_VERSION 20191216

#include "Arduino.h"
#include "Sodaq_DS3231.h"                                   // Real Time Clock support


class TIA_RTC {
  public:
    
    RTC();                                                  // constructor
    
    void setup(                                             // METHOD: setup the LED
      int pin                                               // pin number for the LED
    );
    
  protected:
    int _pin;                                               // LED pin number
};

#endif