//  TIA-Software_Mayfly_Card_Bee.h - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#ifndef TIA_SOFTWARE_MAYFLY_CARD_BEE_H
#define TIA_SOFTWARE_MAYFLY_CARD_BEE_H
#define TIA_SOFTWARE_MAYFLY_CARD_BEE_VERSION 20200306

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"

class BeeSocket {
  public:
    
    BeeSocket();                                            // constructor
    
    bool setup(                                             // METHOD: setup the Bee Socket
      const char *beeModule                                 // defines the module in the Bee socket
    );
};

#endif