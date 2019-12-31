#define DCF_version = 20191231

#include "TIA-Software_Mayfly_Card.h"
Mayfly_card mayflyCard;                                           // establish instance of Mayfly Card

const String BeeModule = "Digi1234";                              // module in the Bee socket, default = "none"

// variables used to hold results of the console.txt profile request
char firstRecord[consoleRecordLength]         = "";               // holds the first record found in the console file
char lastRecord[consoleRecordLength]          = "";               // holds the last record found in the console file

char firstDateTime_YYYY_MM_DD_HH_MM_SS[20];                       // holds the datetime of the first console record in the console file
char endDateTime_YYYY_MM_DD_HH_MM_SS[20];                         // holds the datetime of the last console record in the console file

unsigned long int startTimestampSeconds       = 0;                // holds the timestamp for the first console record in the console file
unsigned long int endTimestampSeconds         = 0;                // holds the timestamp for the last console record in the console file
  
unsigned long int startFilePosition           = 0;                // holds the file position for the start of the first console record in the console file
unsigned long int endFilePosition             = 0;                // holds the file position for the start of the last console record in the console file

void setup()
{
  mayflyCard.setup(BeeModule);                                    // setup the Mayfly Card.  True=test SD card file write, read and remove.
  
  mayflyCard.redLED.turnOn();                                     // turn on the Red LED
  mayflyCard.greenLED.turnOn();                                   // turn on the Green LED
  delay(1000);                                                    // wait a second
  mayflyCard.redLED.turnOff();                                    // turn off the red LED

  // get the console profile
  boolean profileFlag = mayflyCard.SdCard.TIA_getConsoleProfile(
    &firstRecord,                                                 // holds first record in the console file
    &lastRecord,                                                  // holds last record in the console file
    &firstDateTime_YYYY_MM_DD_HH_MM_SS,                           // holds datetime of the first console record in the console file
    &endDateTime_YYYY_MM_DD_HH_MM_SS,                             // holds datetime of the last console record in the console file
    &startTimestampSeconds,                                       // holds timestamp for the first console record in the console file
    &endTimestampSeconds,                                         // holds timestamp for the last console record in the console file
    &startFilePosition,                                           // holds file position for the start of the first console record in the console file
    &endFilePosition                                              // holds file position for the start of the last console record in the console file
  );

  // get the console information
  const int byteLimit = 2000;
  char consoleRecords[byteLimit];
  int numberOfConsoleBytes = mayflyCard.SdCard.TIA_getConsoleRecords(&consoleRecords[0], "2019-10-01 15:30:00", "2019-10-01 16:29:00", byteLimit);

  // get the directory from the SD Card
  const int sdCardDirectoryLimit = 5;                             // limit the number of directory names + file names to be displayed
  SdCardDirectory sd_card_directory[sdCardDirectoryLimit];        // define an array to hold the SD Card directory results
  int numberOfEntries = mayflyCard.SdCard.TIA_dir(&sd_card_directory[0], sdCardDirectoryLimit);     // get the SD Card directory & file names


  /***** this code displays time information *****/
  /*                                             */
  SerialMon.print("Current dateTimeString: ");
  SerialMon.println(mayflyCard.realTimeClock.getDateTimeNowString());
  
  
  /***** this code displays console.txt profile *****/
  /*                                                */
  SerialMon.println("<<<<< CONSOLE FILE PROFILE >>>>>");
  SerialMon.println("\t\tDateTime\t\tTimestamp\tFile Position\tRecord");
  SerialMon.print(" First Record:\t");
  SerialMon.print(firstDateTime_YYYY_MM_DD_HH_MM_SS);SerialMon.print("\t");
  SerialMon.print(startTimestampSeconds);SerialMon.print("\t");
  SerialMon.print(startFilePosition);SerialMon.print("\t\t");
  SerialMon.println(firstRecord);
  SerialMon.print("  Last Record:\t");
  SerialMon.print(endDateTime_YYYY_MM_DD_HH_MM_SS);SerialMon.print("\t");
  SerialMon.print(endTimestampSeconds);SerialMon.print("\t");
  SerialMon.print(endFilePosition);SerialMon.print("\t\t");
  SerialMon.println(lastRecord);

  
  /***** this code displays the console records *****/
  /*                                                */
  SerialMon.println("<<< CONSOLE RECORDS >>>");
  SerialMon.println(consoleRecords);
  SerialMon.println("<<< CONSOLE RECORDS COMPLETE >>>");


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
