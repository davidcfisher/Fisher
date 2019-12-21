#define DCF_version = 20191221

#include "TIA-Software_Mayfly_Card.h"
Mayfly_card mayflyCard;                                           // establish instance of Mayfly Card
const String BeeModule = "Digi1234";                              // module in the Bee socket, default = "none"

void setup()
{
  mayflyCard.setup(BeeModule);                                    // setup the Mayfly Card.  True=test SD card file write, read and remove.
  
  mayflyCard.redLED.turnOn();                                     // turn on the Red LED
  mayflyCard.greenLED.turnOn();                                   // turn on the Green LED
  delay(1000);                                                    // wait a second
  mayflyCard.redLED.turnOff();                                    // turn off the red LED

  // get the console information
  const int byteLimit = 2000;
  char consoleBytes[byteLimit];
  int result = mayflyCard.SdCard.TIA_getConsoleRecords(&consoleBytes[0], "2019-09-01 11:30:00", "2019-09-01 13:29:00", byteLimit);

  //// get the console information
  //const int consoleRecordLimit = 200;
  //consoleRecord console_record[consoleRecordLimit];
  //int numberOfConsoleRecords = mayflyCard.SdCard.TIA_consoleRead(&console_record[0], "2019-09-01 11:30:00", "2019-09-01 13:29:00", consoleRecordLimit);

  // get the directory from the SD Card
  const int sdCardDirectoryLimit = 5;                            // limit the number of directory names + file names to be displayed
  SdCardDirectory sd_card_directory[sdCardDirectoryLimit];        // define an array to hold the SD Card directory results
  int numberOfEntries = mayflyCard.SdCard.TIA_dir(&sd_card_directory[0], sdCardDirectoryLimit);     // get the SD Card directory & file names


  /***** the code below displays time information *****/
  /*                                                  */
  SerialMon.print("dateTimeString: ");
  SerialMon.println(mayflyCard.realTimeClock.getDateTimeNowString());
  
  

  /***** the code below displays the console record information *****/
  /*                                                                */
  //SerialMon.print("numberOfConsoleBytes=");SerialMon.println(numberOfConsoleBytes);

  // process each record
  //for (int i=0; i < numberOfConsoleBytes; i++) {

    //SerialMon.print(i+1);
    //SerialMon.print(F(": ("));
    //SerialMon.print(console_record[i].bytes);
    //SerialMon.print(F(" bytes) "));
    //SerialMon.println(console_record[i].record);
  //}

  /***** the code below simply displays the directory information received *****/
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
