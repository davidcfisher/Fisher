#define Mayfly_Initialization_version = 20200127

#include "TIA-Software_Mayfly_Card.h"

const char beeModule[] = "DigiLTE-M";                                           // module in the Bee socket

Mayfly_card mayflyCard;                                                         // establish instance of Mayfly Card


// set the Mayfly clock
void clockSet(long unsigned int mayflyDtSeconds) {
  DateTime mayflyDT;                                                            // DaatTime of the Mayfly
  char mayflyDtArray[21];                                                       // holds the date string, format: "Jan 29 1954 23:04:33"
  char keyboardChar;                                                            // holds a character from the keyboard
  char lastKeyboardChar;                                                        // holds the last character, so it can be repeated without re-entering
  char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};  // format used by DateTime functions
  boolean breakout = false;                                                     // true=break out of the proposed time adjustment

  while (!breakout) {                                                           // keep adjusting the proposed time until it what we like

    mayflyDT = DateTime(mayflyDtSeconds);                                       // get a DateTime object corresponding to the seconds
    sprintf(                                                                    // create the date string to display
      mayflyDtArray,
      "%s %02d %4d %02d:%02d:%02d", 
      months[mayflyDT.month()-1], mayflyDT.date(), mayflyDT.year(), mayflyDT.hour(), mayflyDT.minute(), mayflyDT.second()
    );

    Serial.println(F("\n================================================"));
    Serial.print(F("   Enter an adjustment for: ")); Serial.println(mayflyDtArray);

    Serial.println(F("          Increment Decrement"));
    Serial.println(F("          --------- ---------"));
    Serial.println(F("     Hour:    h         r"));
    Serial.println(F("   Minute:    m         e"));
    Serial.println(F("   Second:    s         d\n"));

    Serial.println(F("   Repeat last adjustment: ENTER"));
    Serial.println(F("   Target date & time ok, continue: c"));

    while (Serial.available() > 0) char trash = Serial.read();                    // strip off any newlines
    while (Serial.available() <= 0);                                              // wait for a keyboard entry
    keyboardChar = Serial.read();                                                 // get a character from the keyboard
    
    if (keyboardChar == 10) {                                                     // if the ENTER key was pressed by itself
      keyboardChar = lastKeyboardChar;                                            // repeat the last character
    }
    
    switch (keyboardChar) {                                                       // handle the character
      case 'S':                                                                   // increase second
      case 's':
        mayflyDtSeconds++;
        lastKeyboardChar = keyboardChar;
        break;
      case 'D':                                                                   // decrease second
      case 'd':
        mayflyDtSeconds--;
        lastKeyboardChar = keyboardChar;
        break;
      case 'M':                                                                   // increase the minute
      case 'm':
        mayflyDtSeconds += 60;
        lastKeyboardChar = keyboardChar;
        break;
      case 'E':                                                                   // decrease the minute
      case 'e':
        mayflyDtSeconds -= 60;
        lastKeyboardChar = keyboardChar;
        break;
      case 'H':                                                                   // increase the hour
      case 'h':
        mayflyDtSeconds += 3600;        
        lastKeyboardChar = keyboardChar;
        break;
      case 'R':                                                                   // decrease the hour
      case 'r':
        mayflyDtSeconds -= 3600;
        lastKeyboardChar = keyboardChar;
        break;
      case 'C':                                                                   // set the time to this value
      case 'c':
        breakout = true;
        
        while (Serial.available() > 0) char trash = Serial.read();                // strip off any newlines
        while (Serial.available() <= 0) {                                         // wait for a keyboard entry
          delay(1000);
          mayflyDT = mayflyCard.realTimeClock.getDateTimeNow();                   // get the current DateTime
          sprintf(                                                                // create the date string to display
            mayflyDtArray,
            "%s %02d %4d %02d:%02d:%02d", 
            months[mayflyDT.month()-1], mayflyDT.date(), mayflyDT.year(), mayflyDT.hour(), mayflyDT.minute(), mayflyDT.second()
          );

          Serial.print(F("\nCurrent time: ")); Serial.println(mayflyDtArray);
          Serial.println(F("Press ENTER to continue"));
        }
        
        keyboardChar = Serial.read();                                             // get a character from the keyboard
    
        break;
      default:
        Serial.print(F("\nERROR: unknown request - key pressed = ")); Serial.println(keyboardChar);
        break;
    }
  }    
}

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

  mayflyDtString = mayflyCard.realTimeClock.getDateTimeNowString("Mmm DD YYYY");  // String containing Mayfly's current date and time
  mayflyDT = mayflyCard.realTimeClock.getDateTimeNow();                           // DateTime object of Mayfly's current date and time
  long unsigned int mayflyDtSeconds = mayflyDT.get();                             // seconds since 1/1/2000 for Mayfly
  long unsigned int computerDtSeconds = computerDT.get();                         // seconds since 1/1/2000 for Computer
  unsigned long int deltaSeconds = computerDtSeconds - mayflyDtSeconds;           // difference between computer's seconds and Mayfly's seconds

  // show the computer's and Mayfly's dates and times
  Serial.print(F("Computer: ")); Serial.print(__DATE__); Serial.print(F(" "));Serial.print(__TIME__);  Serial.print(F("  (")); Serial.print(computerDtSeconds); Serial.println(F(")"));
  Serial.print(F("  Mayfly: ")); Serial.print(mayflyDtString); Serial.print(F("  (")); Serial.print(mayflyDtSeconds); Serial.println(F(")"));

  // if it looks like the Mayfly clock needs to be set
  if (deltaSeconds > 30000) {
    Serial.println(F("\nLooks like the Mayfly clock needs to be set.\nEnter 'y' to set the clock..."));

    while (Serial.available() <= 0);                                              // wait for a keyboard entry
    keyboardChar = Serial.read();                                                 // get a character from the keyboard

    // process the entry
    if (keyboardChar == 'y' || keyboardChar == 'Y') {
      Serial.println(F("setting the clock"));
      clockSet(computerDtSeconds + 60);                                           // start with the computer's seconds, with a little time added
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
