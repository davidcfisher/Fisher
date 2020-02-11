//  TIA-Software_Mayfly_Card_Bee.cpp - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#include "TIA-Software_Mayfly_Card_Bee.h"                     // include the header file


// CONSTRUCTOR
BeeSocket::BeeSocket() {}                                     // constructor


// METHOD: setup module in bee socket
bool BeeSocket::setup(const char *beeModule)
{
  if (strcmp(beeModule, "none") == 0) {
    Serial.println(F("  STATUS: setting up the bee socket with no module."));
    return true;
  }
  
  else if (strcmp(beeModule, "DigiLTE-M") == 0) {
    Serial.print(F("  STATUS: setting up the bee socket with module=")); Serial.print(beeModule); Serial.println(F("."));
    return true;
  }
  
  else {
    Serial.print(F("<<< ERROR: unknown module in the bee socket: ")); Serial.print(beeModule); Serial.println(F(" >>>"));
    return false;
  }
}
