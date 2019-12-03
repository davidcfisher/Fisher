//  TIA-Software_Mayfly_Card_SdFat.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card_SdFat.h"                           // include the header file
    
File testFile;                                                        // test file object
const char* testFilename = "TIA-Software_testFile.txt";               // test file name
const String SdTestPhrase = "This is the test phrase!";               // test phrase
                
// CONSTRUCTOR
TIA_SdFat::TIA_SdFat() : SdFat(){};                                   // Subclass of SdFat

// METHOD: setup - setup the SD Card
void TIA_SdFat::TIA_setup(boolean testFlag=false, boolean debugFlag=false) {
  
  TIA_init(debugFlag);                                                // initialize the SD card
    
  // test, if requested 
  if (testFlag) { 
    TIA_writeTest(debugFlag);                                         // write test for the SD card
    TIA_readTest(debugFlag);                                          // write test for the SD card
    TIA_removeTest(debugFlag);                                        // remove test for the SD card
  }
}

// METHOD: TIA_init - initialize the SD Card
void TIA_SdFat::TIA_init(boolean debugFlag) {
  
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(F(", line "));SerialMon.print(__LINE__); SerialMon.print(F(": initializing SD Card...")); }
  
  if (begin(SD_CS_PIN)) {                                             // if the SD Card Reader begins successfully
    if (debugFlag) { SerialMon.println(F("...successful.")); }
  }
  else { if (debugFlag) { SerialMon.println(F("...FAILED.")); }}
}

// METHOD: TIA_writeTest - test the write capability of the SD card
void TIA_SdFat::TIA_writeTest(boolean debugFlag) {
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(F(", line"));SerialMon.print(__LINE__); SerialMon.print(F(": starting SD Card file write test...")); }
  
  testFile = open(testFilename, FILE_WRITE);                          // open the test file for writing
  if (testFile) {                                                     // if the test file opened ok
    testFile.rewind();                                                // set pointer to the beginning of the file
    testFile.print(SdTestPhrase);                                     // write to the test file
    testFile.close();                                                 // close the file
    if (debugFlag) { SerialMon.println(F("...successful.")); }
  }
  else { if (debugFlag) { SerialMon.println(F("...FAILED.")); }}
}

// METHOD: TIA_readTest - test the read capability of the SD card
void TIA_SdFat::TIA_readTest(boolean debugFlag) {
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(F(", line"));SerialMon.print(__LINE__); SerialMon.print(F(": starting SD Card file read test...")); }
  
  testFile = open(testFilename);                                      // open the test file for reading
  if (testFile) {                                                     // if the test file opened ok
    String readStr = testFile.readStringUntil('\n');                  // read a line from the file
    readStr.trim();                                                   // remove any \n's
    if (readStr.equals(SdTestPhrase)) {                               
      if (debugFlag) { SerialMon.println(F("...successful.")); }      
    }                                                                 
    else {                                                            // file failed to open
      if (debugFlag) {
        SerialMon.println(F("...FAILED to match Write."));
        SerialMon.print(SdTestPhrase);
        SerialMon.print(F("<<< length="));
        SerialMon.println(SdTestPhrase.length());
        SerialMon.print(F(". Received>>>"));
        SerialMon.print(readStr);
        SerialMon.print(F("<<< length="));
        SerialMon.print(readStr.length());
        SerialMon.println(F("."));
      }
    }
  }
}

// METHOD: TIA_removeTest - test the 'remove file' capability of the SD card
void TIA_SdFat::TIA_removeTest(boolean debugFlag) {
  // perform a remove test
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(F(", line"));SerialMon.print(__LINE__); SerialMon.print(F(": starting SD Card file remove test...")); }
  
  if (remove(testFilename)) {
    if (debugFlag) { SerialMon.println(F("...successful.")); }
  }
  else {
    if (debugFlag) {
      SerialMon.println(F("...FAILED to remove file."));
    }
  }
}

// METHOD: TIA_dir - print the names of files in the directory
void TIA_SdFat::TIA_dir(boolean debugFlag=false) {
  SdFile file;
  
  if (file.open("/", O_READ)) {
    SerialMon.println(); SerialMon.println(F("SD Card files - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"));
  
    TIA_SdFat::TIA_printDirectory(file, "Root", 0, debugFlag);        // process the root directory
  
    SerialMon.println(F("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"));
  }
}

// METHOD: TIA_printDirectory - print all files in a directory and sub-directories
void TIA_SdFat::TIA_printDirectory(SdFile CFile, String dirName, int numTabs, boolean debugFlag=false)
{
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(F(", line"));SerialMon.print(__LINE__); SerialMon.println(F(": starting SD Card file listing ***")); }

  SdFile file;
  char filename[50];
  
  // display the directory name
  for (int i = 1; i <= numTabs; i++) { Serial.print(F("\t")); }       // insert tabs for spacing
  SerialMon.print(F("Directory: "));                                  // print the directory name
  SerialMon.println(dirName);
  
  // step thru all the files in this directory
  while (file.openNext(&CFile, O_READ)) {
    if (!file.isHidden()) {                                           // skip hidden files
      for (int i = 1; i <= numTabs; i++) { Serial.print(F("\t")); }   // insert tabs for spacing
      
      // if this is a sub-directory, process it
      if (file.isDir()) {
        SerialMon.println(F(""));                                     // blank line before this directory listing
        file.getName(filename,50);                                    // get the directory name
        TIA_printDirectory(file, filename, numTabs+1);                // process this directory
      }
      
      // a regular file, not hidden or a directory
      else {
        file.getName(filename,50);                                    // get the filename
        SerialMon.print(filename);                                    // print the filename
        SerialMon.print(F("\t"));

        file.printModifyDateTime(&SerialMon);                         // print modified date and time
        SerialMon.print(F("\t"));
        
        int size = file.fileSize();                                   // get the file size
        SerialMon.print(size);                                        // print the file size
        SerialMon.println(F(" kb"));
      }
    }
    
    file.close();
  }
  
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(F(", line"));SerialMon.print(__LINE__); SerialMon.println(F(": SD Card file listing complete ***")); }
}
