#define DCF_version = 20200102

#include "TIA-Software_Mayfly_Card.h"
Mayfly_card mayflyCard;                                           // establish instance of Mayfly Card

const String BeeModule = "Digi1234";                              // module in the Bee socket, default = "none"

// variables used to hold results of the console.txt profile request
char firstRecord[consoleRecordLength]         = "";               // holds the first record found in the console file
char lastRecord[consoleRecordLength]          = "";               // holds the last record found in the console file

char firstDateTime_YYYY_MM_DD_HH_MM_SS[20];                       // holds the datetime of the first console record in the console file
char lastDateTime_YYYY_MM_DD_HH_MM_SS[20];                        // holds the datetime of the last console record in the console file

unsigned long int firstTimestampSeconds       = 0;                // holds the timestamp for the first console record in the console file
unsigned long int lastTimestampSeconds        = 0;                // holds the timestamp for the last console record in the console file
  
unsigned long int firstFilePosition           = 0;                // holds the file position for the start of the first console record in the console file
unsigned long int lastFilePosition            = 0;                // holds the file position for the start of the last console record in the console file

void setup()
{
  mayflyCard.setup(BeeModule);                                    // setup the Mayfly Card with a BeeModule
  
  mayflyCard.redLED.turnOn();                                     // turn on the Red LED
  mayflyCard.greenLED.turnOn();                                   // turn on the Green LED
  delay(1000);                                                    // wait a second
  mayflyCard.redLED.turnOff();                                    // turn off the red LED

  // get the console profile
  boolean profileFlag = mayflyCard.SdCard.TIA_getConsoleProfile(
    &firstRecord,                                                 // holds first record in the console file
    &lastRecord,                                                  // holds last record in the console file
    &firstDateTime_YYYY_MM_DD_HH_MM_SS,                           // holds datetime of the first console record in the console file
    &lastDateTime_YYYY_MM_DD_HH_MM_SS,                            // holds datetime of the last console record in the console file
    &firstTimestampSeconds,                                       // holds timestamp for the first console record in the console file
    &lastTimestampSeconds,                                        // holds timestamp for the last console record in the console file
    &firstFilePosition,                                           // holds file position for the start of the first console record in the console file
    &lastFilePosition                                             // holds file position for the start of the last console record in the console file
  );

  // get the console information
  const int byteLimit = 2000;
  char consoleRecords[byteLimit];
  char startDate[] = "2019-09-05 12:00:00";
  char endDate[] = "2019-09-05 14:00:00";
  int numberOfConsoleBytes = mayflyCard.SdCard.TIA_getConsoleRecords(&consoleRecords[0], startDate, endDate, byteLimit);

  // get the directory from the SD Card
  const int sdCardDirectoryLimit = 5;                             // limit the number of directory names + file names to be displayed
  SdCardDirectory sd_card_directory[sdCardDirectoryLimit];        // define an array to hold the SD Card directory results
  int numberOfEntries = mayflyCard.SdCard.TIA_dir(&sd_card_directory[0], sdCardDirectoryLimit);     // get the SD Card directory & file names


  SerialMon.println(F("")); SerialMon.println(F("===== Starting Sketch Output ====="));
  /***** this code displays time information *****/
  /*                                             */
  SerialMon.println(""); SerialMon.print(F("Current dateTimeString: "));
  SerialMon.println(mayflyCard.realTimeClock.getDateTimeNowString());
  
  
  /***** this code displays console.txt profile *****/
  /*                                                */
  SerialMon.println(F("")); SerialMon.println(F("<<<<< CONSOLE FILE PROFILE >>>>>"));
  SerialMon.println(F("\t\tDateTime\t\tTimestamp\tFile Position\tRecord"));
  SerialMon.print(F(" First Record:\t"));
  SerialMon.print(firstDateTime_YYYY_MM_DD_HH_MM_SS); SerialMon.print(F("\t"));
  SerialMon.print(firstTimestampSeconds); SerialMon.print(F("\t"));
  SerialMon.print(firstFilePosition); SerialMon.print(F("\t\t"));
  SerialMon.println(firstRecord);
  SerialMon.print(F("  Last Record:\t"));
  SerialMon.print(lastDateTime_YYYY_MM_DD_HH_MM_SS); SerialMon.print(F("\t"));
  SerialMon.print(lastTimestampSeconds); SerialMon.print(F("\t"));
  SerialMon.print(lastFilePosition); SerialMon.print(F("\t\t"));
  SerialMon.println(lastRecord);

  
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
