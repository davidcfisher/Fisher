//  TIA-Software_Mayfly_Card_Bee.cpp - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#include "TIA-Software_Mayfly_Card_Bee.h"                     // include the header file


// CONSTRUCTOR
BeeSocket::BeeSocket() {}                                     // constructor


// METHOD: setup module in bee socket
boolean BeeSocket::setup(const char *beeModule)
{
  if (strcmp(beeModule, "DigiLTE-M") == 0) {
    Serial.print("<<< setting up the bee socket with module: "); Serial.print(beeModule); Serial.println(" >>>");
    return true;
  }
  
  else {
    Serial.print("<<< ERROR: unknown moduel in the bee socket: "); Serial.print(beeModule); Serial.println(" >>>");
    return false;
  }
}
