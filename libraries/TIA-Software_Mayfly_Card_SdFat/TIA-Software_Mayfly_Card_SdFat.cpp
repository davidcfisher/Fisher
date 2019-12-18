//  TIA-Software_Mayfly_Card_SdFat.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card_SdFat.h"                           // include the header file
    
File testFile;                                                        // test file object
const char* testFilename = "TIA-Software_testFile.txt";               // test file name
const String SdTestPhrase = "This is the test phrase!";               // test phrase

int numberOfFiles;                                                    // number of files in directory, including directory names


// CONSTRUCTOR
TIA_SdFat::TIA_SdFat() : SdFat(){};                                   // Subclass of SdFat


// METHOD: setup - setup the SD Card
void TIA_SdFat::TIA_setup(boolean testFlag) {
  
  TIA_init();                                                         // initialize the SD card
    
  // test, if requested 
  if (testFlag) { 
    TIA_writeTest();                                                  // write test for the SD card
    TIA_readTest();                                                   // write test for the SD card
    TIA_removeTest();                                                 // remove test for the SD card
  }
}


// METHOD: TIA_init - initialize the SD Card
void TIA_SdFat::TIA_init() {
  
  if (begin(TIA_SD_CS_PIN)) {                                         // if the SD Card Reader begins successfully
  }
  else {
    }
}


// METHOD: TIA_writeTest - test the write capability of the SD card
void TIA_SdFat::TIA_writeTest() {
  testFile = open(testFilename, FILE_WRITE);                          // open the test file for writing
  if (testFile) {                                                     // if the test file opened ok
    testFile.rewind();                                                // set pointer to the beginning of the file
    testFile.print(SdTestPhrase);                                     // write to the test file
    testFile.close();                                                 // close the file
  }
  else {
    
  }
}


// METHOD: TIA_readTest - test the read capability of the SD card
void TIA_SdFat::TIA_readTest() {
  testFile = open(testFilename);                                      // open the test file for reading
  if (testFile) {                                                     // if the test file opened ok
    String readStr = testFile.readStringUntil('\n');                  // read a line from the file
    readStr.trim();                                                   // remove any \n's
    if (readStr.equals(SdTestPhrase)) {                               
    }                                                                 
    else {                                                            // file failed to open
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


// METHOD: TIA_removeTest - test the 'remove file' capability of the SD card
void TIA_SdFat::TIA_removeTest() {
  // perform a remove test
  if (remove(testFilename)) {
  }
  else {
  }
}


// METHOD: TIA_dir - get the directory names and filenames on the SD Card
int TIA_SdFat::TIA_dir(
  SdCardDirectory *sd_card_directory,                                 // pointer to array to hold the results
  int limit                                                           // limit on the number of directory names + filenames to be returned
)
{
  SdFile file;
  numberOfFiles = 0;
  
  if (file.open("/", O_READ)) {                                       // if opening the root directory was successful
    TIA_processDirectory(&sd_card_directory[0], file, "Root", 0, limit);        // process the root directory
  }
  
  return numberOfFiles;
}


// METHOD: TIA_processDirectory - recuresively get all directory names and filenames in a directory and sub-directories
void TIA_SdFat::TIA_processDirectory(
  SdCardDirectory *sd_card_directory,                                 // pointer to array holding results of dir request
  SdFile CFile,                                                       // 
  String dirName,                                                     // assume method is called while pointing to a directory name
  int numTabs,                                                        // number of tabs to indent this directories information
  int limit                                                           // limit on the number of direcory+file names to be returned
)
{
  SdFile file;
  char filename[50];
  directoryEntry sd_card_directoryEntry;                              // variable to hold the FAT directory entry
  
  // save the directory information
  sd_card_directory[numberOfFiles].folderLevel    = numTabs;
  sd_card_directory[numberOfFiles].directoryFlag  = true;
  sd_card_directory[numberOfFiles].filename       = dirName;
  sd_card_directory[numberOfFiles].modDateTime    = "";
  sd_card_directory[numberOfFiles].sizeKb         = 0;
  sd_card_directory[numberOfFiles].limitReached   = false;
  
  // increment the files counter
  numberOfFiles++;
  
  // if we've reached the maximum number of directory+file names allowed, terminate
  if (numberOfFiles == limit) {
    sd_card_directory[numberOfFiles-1].limitReached = true;
    return;
  }
  
  // step thru all the files in this directory
  while (file.openNext(&CFile, O_READ)) {
    if (!file.isHidden()) {                                           // skip hidden files
      
      for (int i = 1; i <= numTabs; i++) { Serial.print(F("\t")); }   // insert tabs for spacing
      
      // if this is a sub-directory, process it
      if (file.isDir()) {
        //SerialMon.println(F(""));                                   // blank line before this directory listing
        file.getName(filename,50);                                    // get the directory name
        TIA_processDirectory(&sd_card_directory[0], file, String(filename), numTabs+1, limit);  // process this directory
      }
      
      // a regular file, not hidden or a directory
      else {
        file.getName(filename,50);                                    // get the filename
 
        //directoryEntry sd_card_directoryEntry;                        // declare a variable to hold the FAT directory entry
        file.dirEntry(&sd_card_directoryEntry);                       // get the FAT directory entry
        
        // get the encoded last modification day
        unsigned int lastWriteDay = (sd_card_directoryEntry.lastWriteDate << 11) >> 11;            // strip off the encoded year and month - bits on the left
        
        // get the encoded last modification month.  Strip off the encoded year - bits on the left, then...
        unsigned int lastWriteMonth = (sd_card_directoryEntry.lastWriteDate << 7) >> 12;           // ...strip off the encoded day - bits on the right
        
        // get the last modification encoded year, and add the base year of 1980 to it
        unsigned int lastWriteYear = (sd_card_directoryEntry.lastWriteDate >> 9) + 1980;          // strip off the encoded month and date - bits on the right
        
        // get the encoded last modification hour
        unsigned int lastWriteHour = (sd_card_directoryEntry.lastWriteTime >> 11);
        
        // set AM or PM
        String AmPm = "AM";                                           // default is AM
        if (lastWriteHour > 12) {                                     // adjust for PM
          lastWriteHour -= 12;
          AmPm = "PM";
        }
        
        // get the encoded last modification minute
        unsigned int tempInt = (sd_card_directoryEntry.lastWriteTime << 5) >> 10;
        char lastWriteMinute[3];
        sprintf(lastWriteMinute,"%02d", tempInt);                     // ensure minutes are two characters
        
        // establish the last modification date and time
        String modDateTime = String(lastWriteMonth) + "/" + String(lastWriteDay) + "/" + String(lastWriteYear) +
          "  " + String(lastWriteHour) + ":" + String(lastWriteMinute) + " " + AmPm;
        
        // save the file information
        sd_card_directory[numberOfFiles].folderLevel    = numTabs;
        sd_card_directory[numberOfFiles].directoryFlag  = false;
        sd_card_directory[numberOfFiles].filename       = filename;
        sd_card_directory[numberOfFiles].modDateTime    = modDateTime;
        sd_card_directory[numberOfFiles].sizeKb         = file.fileSize();
        sd_card_directory[numberOfFiles].limitReached   = false;
      
        // increment the files counter
        numberOfFiles++;
        
        // if we've reached the maximum number of directory+file names allowed, terminate
        if (numberOfFiles == limit) {
          sd_card_directory[numberOfFiles-1].limitReached = true;
          return;
        }
      }
    }
    
    file.close();
  }
}


// FUNCTION: return the number of seconds since 1/1/200 from a string "YYYY-MM-DD HH:MM:SS"
double secondsSince1Jan2000(String dateTimeToEncode) {
  
  // validate the dateTimeToEncode
  int year = (dateTimeToEncode.substring(0,4)).toInt();               // should be integer in the format YYYY
  String dash1 = dateTimeToEncode.substring(4,5);                     // should be a "-"
  int month = (dateTimeToEncode.substring(5,7)).toInt();              // should be integer in the format MM
  String dash2 = dateTimeToEncode.substring(7,8);                     // should be a "-"
  int dayOfMonth = (dateTimeToEncode.substring(8,10)).toInt();        // should be integer in the format DD
  int hours = (dateTimeToEncode.substring(11,13)).toInt();            // should be integer in the format HH
  String colon1 = dateTimeToEncode.substring(13,14);                  // should be a ":"
  int minutes = (dateTimeToEncode.substring(14,16)).toInt();          // should be integer in the format MM
  String colon2 = dateTimeToEncode.substring(16,17);                  // should be a ":"
  int seconds = (dateTimeToEncode.substring(16,18)).toInt();          // should be integer in the format SS
  
  if (                                                                // indicate an invalid DateTime
    ! (
      year > 2000 &&
      dash1 == "-" &&
      month >= 1 && month <= 12 &&
      dash2 == "-" &&
      dayOfMonth >= 1  && dayOfMonth <= 31 &&
      hours >= 0 && hours <= 23 &&
      colon1 == ":" &&
      minutes >= 0 && minutes <= 59 &&
      colon2 == ":" &&
      seconds >= 0 && seconds <= 59
    )
  ) return -1;
  
  // DateTime expects the date to be in the format: Oct 2 2015
  String monthNames[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  
  String dateString =                                                 // establish a String containing the date
    monthNames[(dateTimeToEncode.substring(5,7)).toInt() - 1] +       // month in the format: Oct
    " " +
    dateTimeToEncode.substring(8,10) +                                // day of month
    " " +
    dateTimeToEncode.substring(0,4);                                  // year
    
  String timeString = dateTimeToEncode.substring(11,19);              // establish a String containing the time
  
  char *date = dateString.c_str();                                    // establish a pointer to a char array holding the date, as needed by DateTime
  char *time = timeString.c_str();                                    // establish a pointer to a char array holding the time, as needed by DateTime
  
  DateTime dT = DateTime(date, time);                                 // establish the DateTime
  return dT.get();                                                    // return the number of seconds since 1/1/2000
}


// FUNCTION: scan thru file backwards, byte by byte, looking for Line Feed (ASCII 10)
unsigned long int scanBackwardsForLineFeed(
  SdFile *scanFile,
  unsigned long int startPos
){
  return startPos;
}


// METHOD get Console records
void TIA_SdFat::TIA_getConsoleRecords(                                 // returns number of records read.  Error codes: -1=file didn't open, -2=end date before start date
  char *destinationArray,                                             // pointer to array to hold console records
  String startDateTimeString,                                         // start reading at "YYYY-MM-DD HH:MM:SS"
  String endDateTimeString,                                           // end reading at "YYYY-MM-DD HH:MM:SS"
  int byteLimit                                                       // limit on the number of bytes to be returned      
)
{
  SerialMon.print("<<< getting Console records, start=");
  SerialMon.print(startDateTimeString);
  SerialMon.print(", end=");
  SerialMon.print(endDateTimeString);
  SerialMon.print(", byteLimit=");
  SerialMon.print(byteLimit);
  SerialMon.println(" >>>");
  
  SdFile consoleFile;                                                 // console file
  
  double startDateTime = secondsSince1Jan2000(startDateTimeString);   // start reading console records at this datetime
  double endDateTime = secondsSince1Jan2000(endDateTimeString);       // end reading console records after this datetime
  
  
  if (!consoleFile.open("console.txt", O_READ)) {                     // if the file doesn't open
    return -1;                                                        // return an error code
  }
  
  SerialMon.print("console.txt opened for reading, curPosition=");
  SerialMon.print(consoleFile.curPosition());
  SerialMon.print(", size=");
  SerialMon.println(consoleFile.fileSize());
  consoleFile.seekEnd();
  SerialMon.print("console.txt executed seekEnd(), curPosition=");
  SerialMon.println(consoleFile.curPosition());
  boolean resultFlag = consoleFile.seekCur(-1);
  SerialMon.print("seekCur(-1) result=");
  SerialMon.println(resultFlag);
  char p = consoleFile.peek();
  SerialMon.print("peek=");
  SerialMon.print(p);
  SerialMon.print("<<<, ASCII=");
  SerialMon.println(int(p));
  
  unsigned long int t = scanBackwardsForLineFeed(&consoleFile, consoleFile.curPosition());
  SerialMon.print("t=");
  SerialMon.println(t);
  
return 42;
}


// METHOD: read the console record
int TIA_SdFat::TIA_consoleReadLines(                                  // returns the number of console records in the console_record array
  consoleRecord *console_record,                                      // array to hold console records
  String startDateTimeString,                                         // start reading at "YYYY-MM-DD HH:MM:SS"
  String endDateTimeString,                                           // end reading at "YYYY-MM-DD HH:MM:SS"
  int limit                                                           // limit on the number of colsole records to be returned      
)
{
  SdFile console_file;                                                // console file
  char line[consoleLineLength];                                       // holds a line read from the console file
  int recordBytes;                                                    // number of characters from console_file into line
  double recordSecondsSince1Jan2000;                                  // for the current record: holds number of seconds since 1/1/2000
  
  double startDateTime = secondsSince1Jan2000(startDateTimeString);   // start reading console records at this datetime
  double endDateTime = secondsSince1Jan2000(endDateTimeString);       // end reading console records after this datetime
  
  boolean startDateTimeFoundFlag = false;                             // true=we've found a record with the startDateTime
  
  if (endDateTime <= startDateTime) return -2;                        // error: start time is after the end time
  
  if (!console_file.open("console.txt", O_READ)) {                    // if the file doesn't open
    return -1;                                                        // return an error code
  }
 
  int numberOfConsoleRecords = 0;                                     // keep track of the number of console records read
  while ((recordBytes = console_file.fgets(line, sizeof(line))) > 0 && numberOfConsoleRecords < limit) {    // read the next record
    
    String record = String(line);                                     // get line as a String
    record.replace("\n","");                                          // remove all \n's
    recordSecondsSince1Jan2000 = secondsSince1Jan2000(record);        // get the number of seconds since 1/1/2000 for this record        
    
    if (recordSecondsSince1Jan2000 == -1 && startDateTimeFoundFlag) { // invalid timestamp on record, so don't check for start or end date exceptions
    }
    
    // if the record is before the start time, continue the loop
    else if (recordSecondsSince1Jan2000 < startDateTime) continue;
    
    // if the record is after the end time, stop processing
    if (recordSecondsSince1Jan2000 > endDateTime) break;
    
    startDateTimeFoundFlag = true;                                    // we've found the startDateTime
    
    console_record[numberOfConsoleRecords].record = record;           // save the entry
    console_record[numberOfConsoleRecords].bytes = recordBytes-1;     // save the number of bytes - trailing "\n" has been removed, hence the -1
    
    numberOfConsoleRecords++;                                         // increment the record counter
  }

  return numberOfConsoleRecords;
}
