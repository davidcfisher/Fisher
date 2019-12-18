//  TIA-Softwaref_Mayfly_Card_Bee.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_BEE_H
#define TIA_SOFTWARE_MAYFLY_CARD_BEE_H
#define TIA_SOFTWARE_MAYFLY_CARD_BEE_VERSION 20191217

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"

class BeeSocket {
  public:
    
    BeeSocket();                                            // constructor
    
    void setup(                                             // METHOD: setup the Bee Socket
      String module                                         // defines the module in the Bee socket
    );
};

#endif