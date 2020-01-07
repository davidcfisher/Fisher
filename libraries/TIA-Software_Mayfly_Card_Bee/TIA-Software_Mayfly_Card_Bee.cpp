//  TIA-Software_Mayfly_Card_Bee.cpp - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#include "TIA-Software_Mayfly_Card_Bee.h"                     // include the header file


// CONSTRUCTOR
BeeSocket::BeeSocket() {}                                     // constructor


// METHOD: setup module in bee socket
boolean BeeSocket::setup(const char *beeModule)
{
  if (strcmp(beeModule, "DigiLTE-M") == 0) {
    SerialMon.print("<<< setting up the bee socket with module: "); SerialMon.print(beeModule); SerialMon.println(" >>>");
    return true;
  }
  
  else {
    SerialMon.print("<<< ERROR: unknown moduel in the bee socket: "); SerialMon.print(beeModule); SerialMon.println(" >>>");
    return false;
  }
}
