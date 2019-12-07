#define SDCT_version = 20191206
#include "TIA-Software_DCF_Globals.h"
#include "TIA-Software_Mayfly_Card.h"

boolean debugFlag = false;                                      // true = print debug information
boolean testFlag = false;                                       // true = perform SD card write, read and delete tests

Mayfly_card mayflyCard;                                         // establish instance of Mayfly Card

SdCardDirectory sd_card_directory[100];
  
void setup() 
{
  mayflyCard.setup(testFlag, debugFlag);                        // setup the Mayfly Card
  mayflyCard.redLED.turnOn(debugFlag);                          // turn on the Red LED
  mayflyCard.greenLED.turnOn(debugFlag);                        // turn on the Green LED
  delay(2000);                                                  // wait a couple of seconds
  mayflyCard.redLED.turnOff(debugFlag);                         // turn off the red LED
  
  int numberOfFiles = mayflyCard.SdCard.TIA_dir(&sd_card_directory[0], debugFlag);     // list the SD Card directories & files

  for (int i=0; i<numberOfFiles; i++) {

    // print a blank line before a directory entry
    if (sd_card_directory[i].directoryFlag) { SerialMon.println(F("")); }
   

    // add tabs to show sub-directory level
    for (int j=0; j<sd_card_directory[i].folderLevel; j++) {
      SerialMon.print("\t");
    }

    // if this is a directory entry
    if (sd_card_directory[i].directoryFlag) {
      SerialMon.print(F("Directory: "));
      SerialMon.println(sd_card_directory[i].filename);
    }

    // otherwise, this is a file
    else {
      SerialMon.print(sd_card_directory[i].filename);
      SerialMon.print(F("\t"));
      SerialMon.print(sd_card_directory[i].sizeKb);
      SerialMon.println(F(" Kb"));
    }
  }

SerialMon.println("This is the Add-a-return branch.");  
  
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.println(": exiting Sketch setup()."); }
}

void loop() {
}
