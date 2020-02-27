#define Mayfly_Initialization_version 20200227

#include "TIA-Software_Mayfly_Card.h"

const char beeModule[] = "none";                                                  // module in the Bee socket.  Valid: "none", "DigiLTE-M"

Mayfly_card mayflyCard;                                                           // establish instance of Mayfly Card

void setup() {

  if (!mayflyCard.setup(&beeModule[0])){                                          // setup the Mayfly Card with a BeeModule.  if it fails...
    Serial.println("<<< ERROR: failed to setup the Mayfly. >>>");
    Serial.println("\nProcessing terminated.");
    while (true);
  };

  /***** Test the LEDs and pushbutton *****/
  /*                                      */
  //Serial.println("\n>>>ACTION: if the LEDs are 'railroading,' push the BUTTON for a second to continue");
  //mayflyCard.railroadLED("forever");                                              // Railroad the LEDs with the pushbutton enabled
  //Serial.println("\n  STATUS: LEDs and button look ok");


  /***** Set the clock, if needed *****/
  /*                                  */
  DateTime computerDT(__DATE__, __TIME__);                                        // DateTime of the compile
  DateTime mayflyDT;                                                              // DaatTime of the Mayfly
  String mayflyDtString;                                                          // holds the Mayfly DateTime
  char keyboardChar;                                                              // holds a character from the keyboard
  
  mayflyDtString = mayflyCard.realTimeClock.getDateTimeNowString("Mmm DD YYYY");  // String containing Mayfly's current date and time
  mayflyDT = mayflyCard.realTimeClock.getDateTimeNow();                           // DateTime object of Mayfly's current date and time
  long int mayflyDtSeconds = mayflyDT.get();                                      // seconds since 1/1/2000 for Mayfly
  long int computerDtSeconds = computerDT.get();                                  // seconds since 1/1/2000 for Computer
  long int deltaSeconds = computerDtSeconds - mayflyDtSeconds;                    // difference between computer's seconds and Mayfly's seconds

  // if the Mayfly clock is significantly different than the compile date and time
  if (abs(deltaSeconds) > 60) {
    Serial.println(F("  STATUS: the Mayfly clock may need to be set."));
    Serial.print(F("  Computer: ")); Serial.print(__DATE__); Serial.print(F(" "));Serial.print(__TIME__);  Serial.print(F("  (")); Serial.print(computerDtSeconds); Serial.println(F(" seconds since Jan 1, 1970)"));
    Serial.print(F("    Mayfly: ")); Serial.print(mayflyDtString); Serial.print(F("  (")); Serial.print(mayflyDtSeconds); Serial.println(F(" seconds since Jan 1, 1970)"));
    Serial.print(F("\n>>>ACTION: enter 'y' to set the clock..."));

    while (Serial.available() <= 0);                                              // wait for a keyboard entry
    keyboardChar = Serial.read();                                                 // get a character from the keyboard

    // process the entry
    if (keyboardChar == 'y' || keyboardChar == 'Y') {
      Serial.println(F("setting the clock"));
      mayflyCard.realTimeClock.adjustClock(computerDtSeconds + 60);               // set the clock time, propose a minute after the compile time
    }

    else {
      Serial.println(F("skipping clock setting"));
    }
  }

  // clock doesn't need to be set
  else {
    Serial.print(F("  STATUS: Mayfly clock looks ok - ")); Serial.println(mayflyDtString);
  }

  /***** Test the SD Card *****/
  /*                          */
  mayflyCard.sdCard.testSdCard();

  /***** Ensure "console.txt" exists on SD Card *****/
  /*                                                */
//  SdFat sd;                                                         // create SdFat object
//  if (!sd.begin(TIA_SD_CS_PIN)) {                                   // of the SD card doesn't start
//    Serial.println("<<< ERROR: SD Card failure.  Ensure SD Card is properly seated in Mayfly. >>>");
//  }
//  
//  File consoleFile;                                                 // create the consoleFile object
//  consoleFile = sd.open("console.txt", FILE_WRITE);                 // open "console.txt" for writing
//  consoleFile.close();
//
//  if (!consoleFile) Serial.println(F("  STATUS: \"console.txt \""));
//  mayflyCard.sdCard.getConsoleProfile(
//  );
//
//  Serial.println(F("")); Serial.println(F("<<<<< CONSOLE FILE PROFILE >>>>>"));
//  Serial.println(F("\t\tDateTime\t\tTimestamp\tFile Position\tRecord"));
//  Serial.print(F(" First Record:\t"));
//  Serial.print(firstDateTime_YYYY_MM_DD_HH_MM_SS); Serial.print(F("\t"));
//  Serial.print(firstTimestampSeconds); SerialMprint(F("\t"));
//  Serial.print(firstFilePosition); Serial.print(F("\t\t"));
//  Serial.println(firstRecord);
//  Serial.print(F("  Last Record:\t"));
//  Serial.print(lastDateTime_YYYY_MM_DD_HH_MM_SS); Serial.print(F("\t"));
//  Serial.print(lastTimestampSeconds); Serial.print(F("\t"));
//  Serial.print(lastFilePosition); Serial.print(F("\t\t"));
//  Serial.println(lastRecord);  


//  // get the console information
//  const int byteLimit = 6000;                                     // return full console records, not to exceed this total number of bytes
//  char consoleRecords[byteLimit];                                 // return the console records into this array
//  char startDate[] = "2019-09-08 12:00:00";                       // return console records starting at this dateTime
//  char endDate[] = "2019-09-08 12:30:00";                         // return console records ending at this dateTime
//  int numberOfConsoleBytes = mayflyCard.sdCard.getConsoleRecords(&consoleRecords[0], startDate, endDate, byteLimit);
//
//  /***** this code displays the console records *****/
//  /*                                                */
//  Serial.println(F("")); Serial.println(F("<<< CONSOLE RECORDS >>>"));
//  Serial.print(numberOfConsoleBytes); Serial.println(F(" bytes returned:"));
//  Serial.println(consoleRecords);



  /***** display the directory information *****/
  /*                                           */
  // get the directory from the SD Card
  const int sdCardDirectoryLimit = 10;                              // limit the number of directory names + file names to be displayed
  SdCardDirectory sd_card_directory[sdCardDirectoryLimit];          // define an array to hold the SD Card directory results
  int numberOfEntries = mayflyCard.sdCard.TIA_dir(&sd_card_directory[0], sdCardDirectoryLimit);     // get the SD Card directory & file names
  Serial.println(F("  STATUS: displaying SD Card directory -"));

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


//#define SerialMon Serial
//  // get the directory from the SD Card
//  const int sdCardDirectoryLimit = 10;                            // limit the number of directory names + file names to be displayed
//  SdCardDirectory sd_card_directory[sdCardDirectoryLimit];        // define an array to hold the SD Card directory results
//  int numberOfEntries = mayflyCard.sdCard.TIA_dir(&sd_card_directory[0], sdCardDirectoryLimit);     // get the SD Card directory & file names
//
//
//  /***** this code displays the directory information *****/
//  /*                                                      */
//  // process each file
//  for (int i=0; i < numberOfEntries; i++) {
//
//    if (sd_card_directory[i].directoryFlag) SerialMon.println(F(""));                               // print a blank line before a directory entry
//
//    for (int j=0; j<sd_card_directory[i].folderLevel; j++) { SerialMon.print(F("\t")); }            // add tabs to indent sub-directory level
//
//    // if this is a directory entry
//    if (sd_card_directory[i].directoryFlag) {
//      SerialMon.print(F("<<< ")); SerialMon.print(sd_card_directory[i].filename); SerialMon.println(F(" >>>"));
//    }
//
//    // otherwise, this is a file
//    else {
//      SerialMon.print(sd_card_directory[i].filename);     SerialMon.print(F("\t"));
//      if (strlen(sd_card_directory[i].filename) <= 12)    SerialMon.print(F("\t"));
//      SerialMon.print(sd_card_directory[i].modDateTime);  SerialMon.print(F("\t"));
//      SerialMon.print(sd_card_directory[i].sizeKb);       SerialMon.println(F(" KB"));
//    }
//
//    // if we've reached the limit of directory+file names
//    if(sd_card_directory[i].limitReached) {
//      SerialMon.println(F(""));SerialMon.print(F("=== More files may exist.  Maximum display limit of ")); SerialMon.print(sdCardDirectoryLimit); SerialMon.println(F(" reached. ==="));
//      break;
//    }
//  }


}


void loop() {
  delay(500);
  mayflyCard.greenLED.switchState();
}
