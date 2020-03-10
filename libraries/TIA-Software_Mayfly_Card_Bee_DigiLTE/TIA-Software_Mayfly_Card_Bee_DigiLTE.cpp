//  TIA-Software_Mayfly_Card_Bee_DigiLTE.cpp - Copyright (c) 2020 TIA Software, LLC.  All rights reserved.

#include "TIA-Software_Mayfly_Card_Bee_DigiLTE.h"                     // include the header file


// CONSTRUCTOR
DigiLTE::DigiLTE() {}                                                 // constructor


// METHOD: setup module in bee socket
bool DigiLTE::setup()
{
  #define TINY_GSM_MODEM_XBEE                                         // Digi is an Xbee modem
  #define TINY_GSM_DEBUG Serial                                       // Turn on TinyGSM debugging
  
  TinyGsm modem(SerialAT);                                            // define the modem
  TinyGsmClient client(modem);                                        // define the cellular client
  
  analogWrite(A5, 255);     // make sure XBEE is turned on - requires XBee adapter and solder joints done. 
  pinMode(23, OUTPUT);      // to BEE pin 9 - Sleep control
  digitalWrite(23, LOW);    // LOW is not sleep, HIGH is sleep, make sure you set sleep mode SM:  0=never sleep, 1=sleep by pin 23
  
  SerialAT.begin(9600);                                               // begin talking to the Digi chip
  
  if (!modem.testAT(5000L)) {
    Serial.println("\n<<< Error: modem did not connect >>>");
    return false;
  }
  
  modem.commandMode();                                                // put the modem into command mode
  modem.sendAT(GF(""));                                               // send AT command
  String at_out = modem.readResponseString(200) ;                     // receive response
  
  if (at_out.indexOf("OK") >= 0)  {
    Serial.println("modem connected ok.");
    return true;
  }
  else {
    Serial.println(" <<< ERROR: modem failed to connect >>>");
    return false;
   }
}
