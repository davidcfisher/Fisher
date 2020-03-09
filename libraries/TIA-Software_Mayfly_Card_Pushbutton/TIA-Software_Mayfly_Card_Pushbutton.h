//  TIA-Software_Mayfly_Card_Pushbutton.h - Copyright (c) 2020 TIA Software, LLC.  All rights reserved.

#ifndef TIA_SOFTWARE_MAYFLY_CARD_PUSHBUTTON_H
#define TIA_SOFTWARE_MAYFLY_CARD_PUSHBUTTON_H
#define TIA_SOFTWARE_MAYFLY_CARD_PUSHBUTTON_VERSION 20200126

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"

class Pushbutton {
  public:
    
    Pushbutton();                                           // constructor
    
    bool setup();                                           // METHOD: setup the pushbutton
    boolean readState();                                    // METHOD: read the pushbutton
    
  protected:
    boolean _state;
};

#endif