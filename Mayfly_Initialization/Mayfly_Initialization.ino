#define Mayfly_Initialization_version 20200306

#include "TIA-Software_Mayfly_Card.h"

const char beeModule[] = "DigiLTE";                                               // module in the Bee socket.  Valid: "none", "DigiLTE-M"
//SdFat sd;

Mayfly_card mayflyCard;                                                           // establish instance of Mayfly Card

void setup() {

  if (!mayflyCard.setup(&beeModule[0])){                                          // setup the Mayfly Card with a BeeModule.  if it fails...
    Serial.println("<<< ERROR: failed to setup the Mayfly. >>>");
    Serial.println("\nProcessing terminated.");
    while (true);
  };

  /***** Check for minimum library levels *****/
  /*                                          */
  Serial.println("  STATUS: checking Library versions...");
  if (SD_FAT_VERSION < 10102) {
    Serial.print("    >>>WARNING: downlevel SD_FAT, should be at least 10102, is: "); Serial.println(SD_FAT_VERSION);
  }
  else Serial.println("    - SD_FAT library: ok");
 
  if (TIA_SOFTWARE_DCF_GLOBALS_VERSION < 20200307) {
    Serial.print("    >>>WARNING: downlevel TIA-SOFTWARE_DCF_GLOBALS, should be at least 20200307, is: "); Serial.println(TIA_SOFTWARE_DCF_GLOBALS_VERSION);
  }
  else Serial.println("    - TIA-SOFTWARE_DCF_GLOBALS library: ok");

  if (TIA_SOFTWARE_MAYFLY_CARD_VERSION < 20200306) {
    Serial.print("    >>>WARNING: downlevel TIA-SOFTWARE_MAYFLY_CARD, should be at least 20200306, is: "); Serial.println(TIA_SOFTWARE_MAYFLY_CARD_VERSION);
  }
  else Serial.println("    - TIA-SOFTWARE_MAYFLY_CARD library: ok");

  if (TIA_SOFTWARE_MAYFLY_CARD_LED_VERSION < 20200201) {
    Serial.print("    >>>WARNING: downlevel TIA_SOFTWARE_MAYFLY_CARD_LED, should be at least 20200201, is: "); Serial.println(TIA_SOFTWARE_MAYFLY_CARD_LED_VERSION);
  }
  else Serial.println("    - TIA_SOFTWARE_MAYFLY_CARD_LED library: ok");

  if (TIA_SOFTWARE_MAYFLY_CARD_PUSHBUTTON_VERSION < 20200126) {
    Serial.print("    >>>WARNING: downlevel TIA_SOFTWARE_MAYFLY_CARD_PUSHBUTTON, should be at least 20200126, is: "); Serial.println(TIA_SOFTWARE_MAYFLY_CARD_PUSHBUTTON_VERSION);
  }
  else Serial.println("    - TIA_SOFTWARE_MAYFLY_CARD_PUSHBUTTON library: ok");

  if (TIA_SOFTWARE_MAYFLY_CARD_RTC_VERSION < 20200128) {
    Serial.print("    >>>WARNING: downlevel TIA_SOFTWARE_MAYFLY_CARD_RTC, should be at least 20200128, is: "); Serial.println(TIA_SOFTWARE_MAYFLY_CARD_RTC_VERSION);
  }
  else Serial.println("    - TIA_SOFTWARE_MAYFLY_CARD_RTC library: ok");

  if (TIA_SOFTWARE_MAYFLY_CARD_SDFAT_VERSION < 20200302) {
    Serial.print("    >>>WARNING: downlevel TIA_SOFTWARE_MAYFLY_CARD_SDFAT, should be at least 20200302, is: "); Serial.println(TIA_SOFTWARE_MAYFLY_CARD_SDFAT_VERSION);
  }
  else Serial.println("    - TIA_SOFTWARE_MAYFLY_CARD_SDFAT library: ok");

  if (TIA_SOFTWARE_MAYFLY_CARD_BEE_VERSION < 20200306) {
    Serial.print("    >>>WARNING: downlevel TIA_SOFTWARE_MAYFLY_CARD_BEE, should be at least 20200306, is: "); Serial.println(TIA_SOFTWARE_MAYFLY_CARD_BEE_VERSION);
  }
  else Serial.println("    - TIA_SOFTWARE_MAYFLY_CARD_BEE library: ok");

  if (TIA_SOFTWARE_MAYFLY_CARD_BEE_DIGILTE_VERSION < 20200307) {
    Serial.print("    >>>WARNING: downlevel TIA_SOFTWARE_MAYFLY_CARD_BEE_DIGILTE, should be at least 20200307, is: "); Serial.println(TIA_SOFTWARE_MAYFLY_CARD_BEE_DIGILTE_VERSION);
  }
  else Serial.println("    - TIA_SOFTWARE_MAYFLY_CARD_BEE_DIGILTE library: ok");


  /***** Test the LEDs and pushbutton *****/
  /*                                      */
  Serial.println("\n>>>ACTION: if the LEDs are 'railroading,' push the BUTTON for a second to continue");
  mayflyCard.railroadLED("forever");                                              // Railroad the LEDs with the pushbutton enabled
  Serial.println("\n  STATUS: LEDs and button look ok");


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
      Serial.println(F("skipping clock setting\n"));
    }
  }

  // clock doesn't need to be set
  else {
    Serial.print(F("  STATUS: Mayfly clock looks ok - ")); Serial.println(mayflyDtString);
  }


  /***** Test the SD Card *****/
  /*                          */
  Serial.println(F("  STATUS: testing the SD Card..."));
  if (!mayflyCard.sdCard.testSdCard(true)) {                                           // true = verbose
    Serial.println(F("  >>>WARNING: SD Card test failed"));
  }
  else {
    Serial.println(F("  STATUS: SD Card test passed"));
  }

  /***** check for file: console.txt *****/
  /*                                     */
  

  /***** display the SD Card directory information *****/
  /*                                                   */
  // get the directory from the SD Card
  const int sdCardDirectoryLimit = 10;                              // limit the number of directory names + file names to be displayed
  SdCardDirectory sd_card_directory[sdCardDirectoryLimit];          // define an array to hold the SD Card directory results
  int numberOfEntries = mayflyCard.sdCard.TIA_dir(&sd_card_directory[0], sdCardDirectoryLimit);     // get the SD Card directory & file names
  Serial.println(F("  STATUS: displaying SD Card directory -"));

  // process each directory or file
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


void loop() {
  delay(500);
  mayflyCard.greenLED.switchState();
}
