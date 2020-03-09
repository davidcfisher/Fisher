//  TIA-Software_Mayfly_Card_Bee_DigiLTE.h - Copyright (c) 2020 TIA Software, LLC.  All rights reserved.

#ifndef TIA_SOFTWARE_MAYFLY_CARD_BEE_DIGILTE_H
#define TIA_SOFTWARE_MAYFLY_CARD_BEE_DIGILTE_H
#define TIA_SOFTWARE_MAYFLY_CARD_BEE_DIGILTE_VERSION 20200307

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"

#define TINY_GSM_MODEM_HAS_GPRS
#define TINY_GSM_MODEM_HAS_WIFI
#include <TIA-Software_TinyGsmClientXBee.h>
typedef TinyGsmXBee TinyGsm;
typedef TinyGsmXBee::GsmClient TinyGsmClient;
typedef TinyGsmXBee::GsmClientSecure TinyGsmClientSecure;

class DigiLTE {
  public:
    
    DigiLTE();                                              // constructor
    
    bool setup(                                             // METHOD: setup the DigiLTE module
    );
    
    TinyGsmXBee modem(SerialAT);
    
    TinyGsmClient client();
};

#endif