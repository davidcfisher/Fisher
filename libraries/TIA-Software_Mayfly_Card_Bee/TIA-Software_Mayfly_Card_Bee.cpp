//  TIA-Software_Mayfly_Card_Bee.cpp - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#include "TIA-Software_Mayfly_Card_Bee.h"                     // include the header file
#include "TIA-Software_Mayfly_Card_Bee_DigiLTE.h"             // include the Digi LTE module

// CONSTRUCTOR
BeeSocket::BeeSocket() {}                                     // constructor


// METHOD: setup module in bee socket
bool BeeSocket::setup(const char *beeModule)
{
  if (strcmp(beeModule, "none") == 0) {
    Serial.println(F("  STATUS: setting up the Bee socket with no module..."));
    return true;
  }
  
  else if (strcmp(beeModule, "DigiLTE") == 0) {
    Serial.print(F("  STATUS: setting up the Bee socket with module=")); Serial.print(beeModule); Serial.println(F("..."));
    
    DigiLTE digi;
    digi.setup();
    return true;
  }
  
  else {
    Serial.print(F("<<< ERROR: unknown module in the Bee socket: ")); Serial.print(beeModule); Serial.println(F(" >>>"));
    return false;
  }
}
