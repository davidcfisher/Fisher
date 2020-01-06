#define DCF_version = 20200106

#include "TIA-Software_Mayfly_Card.h"

const String BeeModule = "Digi1234";                              // module in the Bee socket, default = "none"

Mayfly_card mayflyCard;                                           // establish instance of Mayfly Card

void setup()
{
  mayflyCard.setup(BeeModule);                                    // setup the Mayfly Card with a BeeModule
  
  mayflyCard.redLED.turnOn();                                     // turn on the Red LED
  mayflyCard.greenLED.turnOn();                                   // turn on the Green LED
  delay(1000);                                                    // wait a second
  mayflyCard.redLED.turnOff();                                    // turn off the red LED

  // get the console information
  const int byteLimit = 6000;                                     // return full console records, not to exceed this total number of bytes
  char consoleRecords[byteLimit];                                 // return the console records into this array
  char startDate[] = "2019-09-08 12:00:00";                       // return console records starting at this dateTime
  char endDate[] = "2019-09-10 14:00:00";                         // return console records ending at this dateTime
  int numberOfConsoleBytes = mayflyCard.SdCard.getConsoleRecords(&consoleRecords[0], startDate, endDate, byteLimit);

  // get the directory from the SD Card
  const int sdCardDirectoryLimit = 10;                            // limit the number of directory names + file names to be displayed
  SdCardDirectory sd_card_directory[sdCardDirectoryLimit];        // define an array to hold the SD Card directory results
  int numberOfEntries = mayflyCard.SdCard.TIA_dir(&sd_card_directory[0], sdCardDirectoryLimit);     // get the SD Card directory & file names




  SerialMon.println(F("")); SerialMon.println(F("===== Starting Sketch Output ====="));
  
  /***** this code displays the console records *****/
  /*                                                */
  SerialMon.println(F("")); SerialMon.println(F("<<< CONSOLE RECORDS >>>"));
  SerialMon.print(numberOfConsoleBytes); SerialMon.println(F(" bytes returned:"));
  SerialMon.println(consoleRecords);


  /***** this code displays the directory information *****/
  /*                                                      */
  // process each file
  for (int i=0; i < numberOfEntries; i++) {

    if (sd_card_directory[i].directoryFlag) SerialMon.println(F(""));                               // print a blank line before a directory entry

    for (int j=0; j<sd_card_directory[i].folderLevel; j++) { SerialMon.print(F("\t")); }            // add tabs to indent sub-directory level

    // if this is a directory entry
    if (sd_card_directory[i].directoryFlag) {
      SerialMon.print(F("<<< ")); SerialMon.print(sd_card_directory[i].filename); SerialMon.println(F(" >>>"));
    }

    // otherwise, this is a file
    else {
      SerialMon.print(sd_card_directory[i].filename);     SerialMon.print(F("\t"));
      if (strlen(sd_card_directory[i].filename) <= 12)    SerialMon.print(F("\t"));
      SerialMon.print(sd_card_directory[i].modDateTime);  SerialMon.print(F("\t"));
      SerialMon.print(sd_card_directory[i].sizeKb);       SerialMon.println(F(" KB"));
    }

    // if we've reached the limit of directory+file names
    if(sd_card_directory[i].limitReached) {
      SerialMon.println(F(""));SerialMon.print(F("=== More files may exist.  Maximum display limit of ")); SerialMon.print(sdCardDirectoryLimit); SerialMon.println(F(" reached. ==="));
      break;
    }
  }
}


void loop() {
  delay(500);
  mayflyCard.greenLED.switchState();
}
