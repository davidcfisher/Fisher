//  TIA-Software_Mayfly_Card_SdFat.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card_SdFat.h"                       // include the header file

File testFile;                                                  // test file object
const char* testFilename = "TIA-Software_testFile.txt";           // test file name
const String SdTestPhrase = "This is the test phrase!";           // test phrase
                
// CONSTRUCTOR
TIA_SdFat::TIA_SdFat() : SdFat(){};

// METHOD: setup - setup the SD Card
void TIA_SdFat::TIA_setup(boolean debugFlag) {
  
  // initialize the SD Card
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.print(": initializing SD Card..."); }
  if (begin(SD_CS_PIN)) {                                         // if the SD Card Reader begins successfully
    if (debugFlag) { SerialMon.println("...successful."); }
  }
  else { if (debugFlag) { SerialMon.println("...FAILED."); }}
  
  // perform a write test
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.print(": starting SD Card file write test..."); }
  testFile = open(testFilename, FILE_WRITE);                      // open the test file for writing
  if (testFile) {                                                 // if the test file opened ok
    testFile.rewind();                                            // set pointer to the beginning of the file
    testFile.print(SdTestPhrase);                                 // write to the test file
    testFile.close();                                             // close the file
    if (debugFlag) { SerialMon.println("...successful."); }
  }
  else { if (debugFlag) { SerialMon.println("...FAILED."); }}
  
  // perform a read test
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.print(": starting SD Card file read test..."); }
  testFile = open(testFilename);                                  // open the test file for reading
  if (testFile) {                                                 // if the test file opened ok
    String readStr = testFile.readStringUntil('\n');              // read a line from the file
    readStr.trim();                                               // remove any \n's
    if (readStr.equals(SdTestPhrase)) {
      if (debugFlag) { SerialMon.println("...successful."); }
    }
    else {                                                        // file failed to open
      if (debugFlag) {
        SerialMon.println("...FAILED to match Write.");
        SerialMon.print(SdTestPhrase);
        SerialMon.print("<<< length=");
        SerialMon.println(SdTestPhrase.length());
        SerialMon.print(". Received>>>");
        SerialMon.print(readStr);
        SerialMon.print("<<< length=");
        SerialMon.print(readStr.length());
        SerialMon.println(".");
        //waitForButtonPress(true);
      }
    }
  }
  
  // perform a remove test
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.print(": starting SD Card file remove test..."); }
  
  if (remove(testFilename)) {
    if (debugFlag) { SerialMon.println("...successful."); }
  }
  else {
    if (debugFlag) {
      SerialMon.println("...FAILED to remove file.");
    }
    //waitForButtonPress(true);
  }
}

// METHOD: TIA_ls - list the files
void TIA_SdFat::TIA_ls(boolean debugFlag=false) {
  SdFat sd;
  SdFile dirFile;
  SdFile file;
  
  if (dirFile.open("/", O_READ)) {
    TIA_SdFat::TIA_printDirectory(dirFile, 0, debugFlag);
  }
}

// METHOD: TIA_printDirectory - list files in a directory
void TIA_SdFat::TIA_printDirectory(SdFile CFile, int numTabs, boolean debugFlag=false)
{
  
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.println(": starting SD Card file listing ***"); }

  SdFile file;
  uint16_t dirIndex[50];                                        // Position of file's directory entry
  SdFile parIndex[50];
  uint16_t numberOfFiles = 0;                                   // Number of files found
  
  SerialMon.println(); SerialMon.println("SD Card files - - - - - - - - - -");

  while (file.openNext(&CFile, O_READ)) {
    if (file.isHidden() || false) {                             // file hidden, skip
    }
    else {
       for (uint8_t i = 1; i <= numTabs; i++) {                  // create tabs for spacing
          Serial.print('\t');
       }
       
       if (file.isSubDir()) {
         SdFile SubDirFile;
         TIA_printDirectory(SubDirFile, numTabs+1);
       }
       
       else {
         numberOfFiles++;
         
         dirIndex[numberOfFiles] = file.dirIndex();             // Save dirIndex of file in directory
         parIndex[numberOfFiles] = CFile;
         
         SerialMon.print(numberOfFiles);                        // Print the file number and name
         SerialMon.print(". ");
         file.printName(&SerialMon);
         SerialMon.println();
       }
    }
    
    file.close();
  }
  SerialMon.println("- - - - - - - - - - - - - - - - -");
  if (debugFlag) { SerialMon.print(__FILE__);SerialMon.print(", line ");SerialMon.print(__LINE__); SerialMon.println(": SD Card file listing complete ***"); }
}