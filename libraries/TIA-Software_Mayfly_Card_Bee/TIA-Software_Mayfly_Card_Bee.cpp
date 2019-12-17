//  TIA-Software_Mayfly_Card_Bee.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card_Bee.h"                     // include the header file


// CONSTRUCTOR
BeeSocket::BeeSocket() {}                                     // constructor


// METHOD: setup an LED
void BeeSocket::setup(int pin)
{
  SerialMon.print("<<< setting up the bee socket with module: "); SerialMon.print(BeeModule); SerialMon.println(" >>>");
}
