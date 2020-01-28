#define Mayfly_Initialization_version = 20200128

#include "TIA-Software_Mayfly_Card.h"

const char beeModule[] = "DigiLTE-M";                                           // module in the Bee socket

Mayfly_card mayflyCard;                                                         // establish instance of Mayfly Card


void setup() {
  DateTime computerDT(__DATE__, __TIME__);                                        // DateTime of the compile
  DateTime mayflyDT;                                                              // DaatTime of the Mayfly
  String mayflyDtString;                                                          // holds the Mayfly DateTime
  char keyboardChar;                                                              // holds a character from the keyboard
  
  // signal that we've entered Mayfly setup
  mayflyCard.redLED.turnOn();                                                     // turn on the Red LED
  mayflyCard.greenLED.turnOn();                                                   // turn on the Green LED
  
  mayflyCard.setup(&beeModule[0]);                                                // setup the Mayfly Card with a BeeModule
  
  mayflyCard.redLED.turnOff();                                                    // signal that we've exited Mayfly setup - turn off the red LED


  /***** Set the clock, if needed *****/
  /*                                  */
  mayflyDtString = mayflyCard.realTimeClock.getDateTimeNowString("Mmm DD YYYY");  // String containing Mayfly's current date and time
  mayflyDT = mayflyCard.realTimeClock.getDateTimeNow();                           // DateTime object of Mayfly's current date and time
  long int mayflyDtSeconds = mayflyDT.get();                                      // seconds since 1/1/2000 for Mayfly
  long int computerDtSeconds = computerDT.get();                                  // seconds since 1/1/2000 for Computer
  long int deltaSeconds = computerDtSeconds - mayflyDtSeconds;                    // difference between computer's seconds and Mayfly's seconds

  // if the Mayfly clock is significantly different than the compile date and time
  if (abs(deltaSeconds) > 60) {
    Serial.println(F("\nACTION: looks like the Mayfly clock needs to be set."));
    Serial.print(F("   Computer: ")); Serial.print(__DATE__); Serial.print(F(" "));Serial.print(__TIME__);  Serial.print(F("  (")); Serial.print(computerDtSeconds); Serial.println(F(" seconds since Jan 1, 1970)"));
    Serial.print(F("     Mayfly: ")); Serial.print(mayflyDtString); Serial.print(F("  (")); Serial.print(mayflyDtSeconds); Serial.println(F(" seconds since Jan 1, 1970)"));
    Serial.print(F("\nEnter 'y' to set the clock..."));

    while (Serial.available() <= 0);                                              // wait for a keyboard entry
    keyboardChar = Serial.read();                                                 // get a character from the keyboard

    // process the entry
    if (keyboardChar == 'y' || keyboardChar == 'Y') {
      Serial.println(F("setting the clock"));
      mayflyCard.realTimeClock.setClock(computerDtSeconds + 60);                  // set the clock time, propose a minute after the compile time
    }

    else {
      Serial.println(F("skipping clock setting"));
    }
  }

  // clock doesn't need to be set
  else {
    Serial.print(F("STATUS: Mayfly clock looks ok - ")); Serial.print(mayflyDtString);
  }

  // get the console information
  const int byteLimit = 6000;                                     // return full console records, not to exceed this total number of bytes
  char consoleRecords[byteLimit];                                 // return the console records into this array
  char startDate[] = "2019-09-08 12:00:00";                       // return console records starting at this dateTime
  char endDate[] = "2019-09-08 12:30:00";                         // return console records ending at this dateTime
  int numberOfConsoleBytes = mayflyCard.sdCard.getConsoleRecords(&consoleRecords[0], startDate, endDate, byteLimit);

  // get the directory from the SD Card
  const int sdCardDirectoryLimit = 10;                            // limit the number of directory names + file names to be displayed
  SdCardDirectory sd_card_directory[sdCardDirectoryLimit];        // define an array to hold the SD Card directory results
  int numberOfEntries = mayflyCard.sdCard.TIA_dir(&sd_card_directory[0], sdCardDirectoryLimit);     // get the SD Card directory & file names


  Serial.println(F("")); Serial.println(F("===== Starting Sketch Output ====="));
  
  /***** this code displays the console records *****/
  /*                                                */
  Serial.println(F("")); Serial.println(F("<<< CONSOLE RECORDS >>>"));
  Serial.print(numberOfConsoleBytes); Serial.println(F(" bytes returned:"));
  Serial.println(consoleRecords);


  /***** this code displays the directory information *****/
  /*                                                      */
  // process each file
  for (int i=0; i < numberOfEntries; i++) {

    if (sd_card_directory[i].directoryFlag) Serial.println(F(""));                                  // print a blank line before a directory entry

    for (int j=0; j<sd_card_directory[i].folderLevel; j++) { Serial.print(F("\t")); }               // add tabs to indent sub-directory level

    // if this is a directory entry
    if (sd_card_directory[i].directoryFlag) {
      Serial.print(F("<<< ")); Serial.print(sd_card_directory[i].filename); Serial.println(F(" >>>"));
    }

    // otherwise, this is a file
    else {
      Serial.print(sd_card_directory[i].filename);     Serial.print(F("\t"));
      if (strlen(sd_card_directory[i].filename) <= 12) Serial.print(F("\t"));
      Serial.print(sd_card_directory[i].modDateTime);  Serial.print(F("\t"));
      Serial.print(sd_card_directory[i].sizeKb);       Serial.println(F(" KB"));
    }

    // if we've reached the limit of directory+file names
    if(sd_card_directory[i].limitReached) {
      Serial.println(F(""));Serial.print(F("=== More files may exist.  Maximum display limit of ")); Serial.print(sdCardDirectoryLimit); Serial.println(F(" reached. ==="));
      break;
    }
  }
}


void loop() {
  delay(500);
  mayflyCard.greenLED.switchState();
}
