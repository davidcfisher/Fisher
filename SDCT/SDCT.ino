#define SDCT_version = 20191210

#include "TIA-Software_Mayfly_Card.h"
Mayfly_card mayflyCard;                                           // establish instance of Mayfly Card
 
void setup() 
{
  mayflyCard.setup();                                             // setup the Mayfly Card.  True=test SD card file write, read and remove.
  mayflyCard.redLED.turnOn();                                     // turn on the Red LED
  mayflyCard.greenLED.turnOn();                                   // turn on the Green LED
  delay(1000);                                                    // wait a couple of seconds
  mayflyCard.redLED.turnOff();                                    // turn off the red LED

  // get the directory from the SD Card
  const int SdCardDirectoryLimit = 50;                            // limit the number of directory names + file names to be displayed
  SdCardDirectory sd_card_directory[SdCardDirectoryLimit];        // define an array to hold the SD Card directory results
  int numberOfEntries = mayflyCard.SdCard.TIA_dir(&sd_card_directory[0], SdCardDirectoryLimit);     // get the SD Card directory & file names

  /***** the code below simply displays the directory information received ***/
  // process each file
  for (int i=0; i < numberOfEntries; i++) {
    
    if (sd_card_directory[i].directoryFlag) SerialMon.println(F(""));                               // print a blank line before a directory entry
   
    for (int j=0; j<sd_card_directory[i].folderLevel; j++) { SerialMon.print("\t"); }               // add tabs to indent sub-directory level

    // if this is a directory entry
    if (sd_card_directory[i].directoryFlag) {
      SerialMon.print(F("Directory: ")); SerialMon.println(sd_card_directory[i].filename);
    }

    // otherwise, this is a file
    else {
      SerialMon.print(sd_card_directory[i].filename);     SerialMon.print(F("\t"));
      SerialMon.print(sd_card_directory[i].modDateTime);  SerialMon.print(F("\t"));
      SerialMon.print(sd_card_directory[i].sizeKb);       SerialMon.println(F(" KB"));
    }
    
    // if we've reached the limit of directory+file names
    if(sd_card_directory[i].limitReached) {
      SerialMon.println(F(""));SerialMon.print(F("=== More files may exist.  Maximum display limit of ")); SerialMon.print(SdCardDirectoryLimit); SerialMon.println(F(" reached. ==="));
      break;
    }
  }

  //mayflyCard.greenLED.setName("The pretty GREEN LED");
  
  SerialMon.println("This is the Add-a-return branch.");  
}

void loop() {
  delay(500);
  mayflyCard.redLED.switchState();
  mayflyCard.greenLED.switchState();
//  SerialMon.print(mayflyCard.redLED.getName()); SerialMon.print(F(" state=")); SerialMon.println(mayflyCard.redLED.getState());
//  SerialMon.print(mayflyCard.greenLED.getName()); SerialMon.print(F(" state=")); SerialMon.println(mayflyCard.greenLED.getState());

}
