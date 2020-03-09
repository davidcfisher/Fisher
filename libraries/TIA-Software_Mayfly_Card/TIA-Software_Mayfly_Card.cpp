//  TIA-Software_Mayfly_Card.cpp - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#include "TIA-Software_Mayfly_Card.h"                         // include the header file

Mayfly_card::Mayfly_card() {};                                // constructor

TIA_RTC     Mayfly_card::realTimeClock{};                     // Mayfly has a Real Time Clock
LED         Mayfly_card::redLED{};                            // Mayfly has a Red LED 
LED         Mayfly_card::greenLED{};                          // Mayfly has a Green LED
Pushbutton  Mayfly_card::pushbutton{};                        // Mayfly has a pushbutton
TIA_SdFat   Mayfly_card::sdCard{};                            // Mayfly has an SD Card
BeeSocket   Mayfly_card::beeSocket{};                         // Mayfly has a Bee Socket


// METHOD: setup the Mayfly Card
bool Mayfly_card::setup(
  const char *beeModule                                       // Bee module installed on the Mayfly
)
{
  Serial.begin(57600);                                        // initialize the Serial Monitor
  delay (100);

  Serial.println(F("\n<<< Mayfly Initialization >>>"));

  if (!redLED.setup(TIA_redLedPin, "Red LED")) {              // setup the red LED
    Serial.println("<<< ERROR: failed to setup Red LED. >>>");
    return false;
  };
  
  Serial.println("  STATUS: Red LED setup ok.");
  
  if (!greenLED.setup(TIA_greenLedPin, "Green LED")) {        // setup the green LED
    Serial.println("<<< ERROR: failed to setup Green LED. >>>");
    return false;
  };
  
  Serial.println("  STATUS: Green LED setup ok.");
  
  if (!realTimeClock.TIA_setup()) {                           // setup the real time clock
    Serial.println("<<< ERROR: failed to setup Real Time Clock. >>>");
    return false;
  };
  
  Serial.println("  STATUS: Real Time Clock setup ok.");
  
  if (!pushbutton.setup()) {                                  // setup the pushbutton
    Serial.println("<<< ERROR: failed to setup Pushbutton. >>>");
    return false;
  };
  
  Serial.println("  STATUS: Pushbutton setup ok.");
  
  if (!sdCard.TIA_setup()) {                                // setup the SD card
    Serial.println("<<< ERROR: failed to setup SD Card. >>>");
    return false;
  };
  
  Serial.println("  STATUS: SD Card setup ok.");
  
  if (!beeSocket.setup(beeModule)) {                         // setup the Bee Socket
    Serial.println("<<< ERROR: failed to setup Bee Socket. >>>");
    return false;
  };
  
  Serial.println("  STATUS: Bee Socket setup ok.");
  
  return true;
}


// METHOD: railroad - alternately flash the LEDs
void Mayfly_card::railroadLED(                                // alternately flash the LEDs
  int cycleCount,                                             // number of cycles to flash
  boolean enablePushbutton,                                   // true=button push terminates loop
  int cycleMs                                                 // flash cycle in ms
)
{
  int counter = cycleCount < 0 ? 1 : cycleCount;              // handle a negative cycleCount
  int halfCycleMs = cycleMs/2;                                // each LED stays lit for half the cycle time
  greenLED.turnOff();                                         // start with the green LED off
  redLED.turnOn();                                            // start with the red LED on
  
  for (int i=0; i < counter; i++) {                           // cycle thru the requested number of times
    delay(halfCycleMs);                                       // delay for half the cycle
    greenLED.switchState();                                   // switch the state of the green lED
    redLED.switchState();                                     // switch the state of the red LED
    if (enablePushbutton && pushbutton.readState()) break;    // break if the button is pressed
    
    delay(halfCycleMs);                                       // delay for half the cycle
    greenLED.switchState();                                   // switch the state of the green lED
    redLED.switchState();                                     // switch the state of the red LED
    if (enablePushbutton && pushbutton.readState()) break;    // break if the button is pressed
    
    if (cycleCount < 0) i = -1;                               // negative cycleCount means cycle forever
  }
  
  greenLED.turnOff();
  redLED.turnOff();
}


// METHOD - alternately flash the LEDs
void Mayfly_card::railroadLED(                                // alternately flash the LEDs
  String cycleCount,                                          // String defining how many cycles
  boolean enablePushbotton,                                   // true=button push terminates loop
  int cycleMs                                                 // flash cycle in ms
)
{
  cycleCount.toLowerCase();                                   // ensure cycleCount is all lowercase
  
  if (cycleCount == "forever") {
    railroadLED(-1, enablePushbotton, cycleMs);
  }
  
  else if (cycleCount == "ten") {
    railroadLED(10, enablePushbotton, cycleMs);
  }
  
  else {
    Serial.print("\n>>>ERROR calling railroadLED: unknown cycleCount=\"");
    Serial.print(cycleCount);
    Serial.println("\"");
  }
}