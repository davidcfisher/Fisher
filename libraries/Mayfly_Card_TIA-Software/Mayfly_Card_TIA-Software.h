//  Mayfly_Card_TIA-Software.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.

//  v1.0

#ifndef MAYFLY_CARD_TIA_SOFTWARE_H
#define MAYFLY_CaRD_TIA_SOFTWARE_H

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"                       // global headers
#include <Wire.h>                                           // library to handle I2C, which is used to access Real Time Clock
#include <Sodaq_DS3231.h>                                   // library to handle Real Time Clock functions
#include <ArduinoJson.h>                                    // include the JSON library
#include "SdFat.h"                                          // library to handle SD Card
#include "LEDs_TIA-Software.h"                              // library to handle Mayfly LEDs

class Mayfly_card {
  public:
    Mayfly_card();                                          // constructor
    void setup(boolean debugFlag=false);                    // setup the Mayfly card
};


#endif