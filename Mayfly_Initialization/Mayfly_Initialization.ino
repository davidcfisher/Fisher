#define Mayfly_Initialization_version = 20200126

#include "TIA-Software_Mayfly_Card.h"

const char beeModule[] = "DigiLTE-M";                             // module in the Bee socket

Mayfly_card mayflyCard;                                           // establish instance of Mayfly Card

DateTime computerDateTime(__DATE__, __TIME__);
DateTime mayflyDateTime;
String mayflyDateAndTimeString;

void setup()
{
  mayflyCard.setup(&beeModule[0]);                                // setup the Mayfly Card with a BeeModule
  
  mayflyDateAndTimeString = mayflyCard.realTimeClock.getDateTimeNowString("Mmm DD YYYY");
  mayflyDateTime = mayflyCard.realTimeClock.getDateTimeNow();
  long unsigned int mayflyDateTimeSeconds = mayflyDateTime.get();
  long unsigned int computerDateTimeSeconds = computerDateTime.get();
 
  mayflyCard.redLED.turnOn();                                     // turn on the Red LED
  mayflyCard.greenLED.turnOn();                                   // turn on the Green LED
  delay(1000);                                                    // wait a second
  mayflyCard.redLED.turnOff();                                    // turn off the red LED

  // show the computer's and Mayfly's dates and times
  Serial.print(F("Computer: ")); Serial.print(__DATE__); Serial.print(F(" "));Serial.print(__TIME__);  Serial.print(F("  (")); Serial.print(computerDateTimeSeconds); Serial.println(F(")"));
  Serial.print(F("  Mayfly: ")); Serial.print(mayflyDateAndTimeString); Serial.print(F("  (")); Serial.print(mayflyDateTimeSeconds); Serial.println(F(")"));

  unsigned long int deltaSeconds = computerDateTimeSeconds - mayflyDateTimeSeconds;

  // if it looks like the Mayfly clock needs to be set
  if (deltaSeconds > 30000) {
    Serial.print(F("Looks like the Mayfly clock needs to be set.\nEnter 'y' to set the clock..."));

    // wait for a keyboard entry
    while (Serial.available() <= 0);
    
    char keyboardChar = Serial.read();                                                     // get a character from the keyboard

    // if ENTER was pressed
    if (keyboardChar == 'y') {
      
      Serial.println(F("setting the Mayfly clock\n"));

      Serial.println(F("       +1  -1"));
      Serial.println(F("       --  --"));
      Serial.println(F("Year    e   i"));
      Serial.println(F("Month   o   t"));
      Serial.println(F("Day     a   y"));
      Serial.println(F("Hour    h   r"));
      Serial.println(F("Minute  m   e"));
      Serial.println(F("Second  s   d\n"));
      Serial.println(F("Repeat last adjustment: ENTER"));
      Serial.println(F("Target date & time ok, continue: k"));

      // wait for a keyboard entry
      while (Serial.available() <= 0);

    }

    else {
      Serial.println(F("skipping clock setting"));
    }
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
