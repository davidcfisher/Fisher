//  TIA-Software_Mayfly_Card_Bee_DigiLTE.h - Copyright (c) 2020 TIA Software, LLC.  All rights reserved.

#ifndef TIA_SOFTWARE_MAYFLY_CARD_BEE_DIGILTE_H
#define TIA_SOFTWARE_MAYFLY_CARD_BEE_DIGILTE_H
#define TIA_SOFTWARE_MAYFLY_CARD_BEE_DIGILTE_VERSION 20200310

#define TINY_GSM_MODEM_XBEE                                 // define the Digi Xbee modem

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"
#include "TinyGsmClient.h"


class DigiLTE {
  public:
    
    DigiLTE();                                              // constructor
    
    bool setup();                                           // METHOD: setup the DigiLTE module
};

#endif