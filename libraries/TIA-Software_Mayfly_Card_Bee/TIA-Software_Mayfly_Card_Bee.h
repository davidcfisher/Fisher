//  TIA-Software_Mayfly_Card_Bee.h - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#ifndef TIA_SOFTWARE_MAYFLY_CARD_BEE_H
#define TIA_SOFTWARE_MAYFLY_CARD_BEE_H
#define TIA_SOFTWARE_MAYFLY_CARD_BEE_VERSION 20200107

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"

class BeeSocket {
  public:
    
    BeeSocket();                                            // constructor
    
    void setup(                                             // METHOD: setup the Bee Socket
      const char *beeModule                                 // defines the module in the Bee socket
    );
};

#endif