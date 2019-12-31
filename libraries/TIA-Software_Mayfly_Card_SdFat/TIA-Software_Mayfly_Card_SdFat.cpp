//  TIA-Software_Mayfly_Card_SdFat.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card_SdFat.h"                           // include the header file
    
File testFile;                                                        // test file object
const char* testFilename = "TIA-Software_testFile.txt";               // test file name
const String SdTestPhrase = "This is the test phrase!";               // test phrase
//const int consoleRecordLength = 256;

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


// FUNCTION: return the number of seconds since 1/1/200 from a String "YYYY-MM-DD HH:MM:SS"
unsigned long int secondsSince1Jan2000(String dateTimeToEncode) {
  
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


// FUNCTION: return seconds since 1/1/200 if input is a valid dateTime char array of the format:  YYYY-MM-DD HH:MM:SS
unsigned long int secondsSince1Jan2kFromTimestamp(
  char line[consoleRecordLength]
)
{  
  char dateTemplate_Mmm_DD_YYYY[12] = {"Mmm DD YYYY"};                // template for date in this format
  char timeTemplate_HH_MM_SS[9]     = {"HH:MM:SS"};                   // template for time in this format
  
  // used to construct dateTemplate_Mmm_DD_YYYY
  char months[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  
  // establish arrays to hold timestamp parts to be validated  
  char potentialYearArray[5]    = {"YYYY"};
  char potentialDash1Array[2]   = {"-"};
  char potentialMonthArray[3]   = {"MM"};
  char potentialDash2Array[2]   = {"-"};
  char potentialDayArray[11]    = {"DD"};     
  char potentialHourArray[3]    = {"HH"};
  char potentialColon1Array[2]  = {":"};
  char potentialMinuteArray[3]  = {"MM"};
  char potentialColon2Array[2]  = {":"};
  char potentialSecondArray[3]  = {"SS"};
   
  // pull the potential pieces out of the input line
  for (int i=0; i<4; i++)   potentialYearArray[i]   = line[i];
  for (int i=0; i<1; i++)   potentialDash1Array[i]  = line[i+4];
  for (int i=0; i<2; i++)   potentialMonthArray[i]  = line[i+5];
  for (int i=0; i<1; i++)   potentialDash2Array[i]  = line[i+7];
  for (int i=0; i<2; i++)   potentialDayArray[i]    = line[i+8];
  for (int i=0; i<2; i++)   potentialHourArray[i]   = line[i+11];
  for (int i=0; i<1; i++)   potentialColon1Array[i] = line[i+13];
  for (int i=0; i<2; i++)   potentialMinuteArray[i] = line[i+14];
  for (int i=0; i<1; i++)   potentialColon2Array[i] = line[i+16];
  for (int i=0; i<2; i++)   potentialSecondArray[i] = line[i+17];
   
  // establish integer values for the appropriate pieces
  int potentialYearInt    = atoi(potentialYearArray);
  int potentialMonthInt   = atoi(potentialMonthArray);
  int potentialDayInt     = atoi(potentialDayArray);
  int potentialHourInt    = atoi(potentialHourArray);
  int potentialMinuteInt  = atoi(potentialMinuteArray);
  int potentialSecondInt  = atoi(potentialSecondArray);
 
  // if every piece of the input line is valid
  if (potentialYearInt > 2000                                   &&
      potentialDash1Array[0] == '-'                             &&
      potentialMonthInt >= 1        && potentialMonthInt <= 12  &&
      potentialDash2Array[0] == '-'                             &&
      potentialDayInt >= 1          && potentialDayInt <= 31    &&
      potentialHourInt >= 0         && potentialHourInt <= 23   &&
      potentialColon1Array[0] == ':'                            &&
      potentialMinuteInt >= 0       && potentialMinuteInt <= 59 &&
      potentialColon2Array[0] == ':'                            &&
      potentialSecondInt >= 0       && potentialSecondInt <= 59
    )
  {
     
    // construct the dateTemplate_Mmm_DD_YYYY for format "Mmm DD YYYY" (e.g. Dec 19 2019), used to get the number of seconds since 1/1/2000
    for (int i=0; i<3; i++) dateTemplate_Mmm_DD_YYYY[i]   = months[potentialMonthInt - 1][i];                 // "Mmm"
    for (int i=0; i<2; i++) dateTemplate_Mmm_DD_YYYY[i+4] = potentialDayArray[i];                             // "DD"
    for (int i=0; i<4; i++) dateTemplate_Mmm_DD_YYYY[i+7] = potentialYearArray[i];                            // "YYYY"
    
    // construct the timeTemplate_HH_MM_SS for format "HH:MM:SS", used to get the number of seconds since 1/1/2000
    for (int i=0; i<2; i++) timeTemplate_HH_MM_SS[i] = potentialHourArray[i];                                 // "HH"
    for (int i=0; i<2; i++) timeTemplate_HH_MM_SS[i+3] = potentialMinuteArray[i];                             // "MM"
    for (int i=0; i<2; i++) timeTemplate_HH_MM_SS[i+6] = potentialSecondArray[i];                             // "SS"

    // establish the timestamp of the console record in seconds since 1/1/2000
    return (DateTime(dateTemplate_Mmm_DD_YYYY, timeTemplate_HH_MM_SS)).get(); 
  }
  
  // otherwise, no valid dateTime was found
  else return 0;
}


// FUNCTION: scan thru console file backwards looking for the previous record
boolean getPreviousRecord(                                            // true=previous record found
  char *line,                                                         // char array holding the previous record
  unsigned long int *progressPos_ptr                                  // position to start scanning the console record backwards
)
{
  char fgetsLine[consoleRecordLength];                                // hold a console record from fgets
  boolean lineFeedFoundFlag = false;                                  // true = we found a Line Feed (\n) while scanning backwards
  SdFile consoleFile;                                                 // console file
    
  if (!consoleFile.open("console.txt", O_READ)) {                     // if the file doesn't open
    SerialMon.println("Console.txt did not open."); 
    return false; 
  } 
    
  consoleFile.seekSet(*progressPos_ptr);                               // start scanning at the position where we previously found a Line Feed
    
  // scan backwards until we find a previous Line Feed, indicating an ever earlier console record
  while (!lineFeedFoundFlag) {  
    consoleFile.seekCur(-1);                                          // back up one character

    // if a Line Feed is found      
    if (consoleFile.peek() == 10) { 
      lineFeedFoundFlag = true;                                       // flag that we found the Line Feed
      *progressPos_ptr = consoleFile.curPosition();                   // grab the position of the Line Feed
    } 
  } 
    
  consoleFile.seekCur(1);                                             // move forward past the Line Feed  
  consoleFile.fgets(fgetsLine, consoleRecordLength);                  // get this whole console record
  for (int i=0; i < consoleRecordLength; i++) line[i] = fgetsLine[i]; // copy this record into the line to be returned
  return true;
}


// METHOD: get console.txt profile
boolean TIA_SdFat::TIA_getConsoleProfile(
  
  char (*firstRecord)[consoleRecordLength],
  char (*lastRecord)[consoleRecordLength],
  
  char (*firstDateTime_YYYY_MM_DD_HH_MM_SS)[20],                      // datetime of the first console record in the console file
  char (*lastDateTime_YYYY_MM_DD_HH_MM_SS)[20],                       // datetime of the last console record in the console file
  
  unsigned long int *firstTimestampSeconds,                           // timestamp for the first console record in the console file
  unsigned long int *lastTimestampSeconds,                            // timestamp for the last console record in the console file
    
  unsigned long int *firstFilePosition,                               // file position for the start of the first console record in the console file
  unsigned long int *lastFilePosition                                 // file position for the start of the last console record in the console file
)
{
  SdFile consoleFile;                                                 // console file
  char line[consoleRecordLength]  = "";
  boolean firstRecordFoundFlag    = false;
  
  if (!consoleFile.open("console.txt", O_READ)) {                     // if the file doesn't open
    SerialMon.println("Error 388: console.txt did not open.");
    return -1;                                                        // return an error code
  }
  
  // search until we find the first record with a timestamp
  while (!firstRecordFoundFlag) {
    
    // get the next record of the console file
    consoleFile.fgets(line, consoleRecordLength);                     // get this whole console record
    if (line[strlen(line)-1] = '\n') line[strlen(line)-1] = '\0';     // remove the New Line ('\n')
    
    *firstTimestampSeconds = secondsSince1Jan2kFromTimestamp(line);   // save the timestamp of the first record, 0=invalid dateTime

    // if the timestamp is valid, *firstTimestampSeconds will be set
    if (*firstTimestampSeconds > 0) {
      *firstFilePosition = consoleFile.curPosition();                 // point to the last character
      strcpy(*firstRecord, line);                                     // save the first record
      line[19] = '\0';
      strncpy(*firstDateTime_YYYY_MM_DD_HH_MM_SS, line, 20);
      firstRecordFoundFlag = true;
    }   
  }
  
  // process the last record in the file
  consoleFile.seekEnd();                                              // seek to eof
  consoleFile.seekCur(-1);                                            // seek to last character before the eof, likely LF
  *lastFilePosition = consoleFile.curPosition();                      // point to the last character
  getPreviousRecord(&line[0], lastFilePosition);                      // position to start scanning the console record backwards
  if (line[strlen(line)-1] = '\n') line[strlen(line)-1] = '\0';       // remove the New Line ('\n')

  *lastTimestampSeconds = secondsSince1Jan2kFromTimestamp(line);      // get the number of seconds since 1/1/2000 for this record
  
  // if the last record had a valid timestamp
  if (*lastTimestampSeconds > 0) {
    *lastFilePosition = consoleFile.curPosition();                    // save the file position of the last record
    strcpy(*lastRecord, line);                                        // save the last record
    line[19] = '\0';
    strncpy(*lastDateTime_YYYY_MM_DD_HH_MM_SS, line, 20);             // save the dateTime of the last record
  }   

  if (false) {
    SerialMon.println("<<< CONSOLE FILE PROFILE >>> in getConsoleProfile()");
    SerialMon.println("\t\tDateTime\t\tTimestamp\tFile Posn\tRecord");
    SerialMon.print(" First Record:\t");
    SerialMon.print(*firstDateTime_YYYY_MM_DD_HH_MM_SS);SerialMon.print("\t");
    SerialMon.print(*firstTimestampSeconds);SerialMon.print("\t");
    SerialMon.print(*firstFilePosition);SerialMon.print("\t\t");
    SerialMon.println(*firstRecord);
    SerialMon.print("  Last Record:\t");
    SerialMon.print(*lastDateTime_YYYY_MM_DD_HH_MM_SS);SerialMon.print("\t");
    SerialMon.print(*lastTimestampSeconds);SerialMon.print("\t");
    SerialMon.print(*lastFilePosition);SerialMon.print("\t\t");
    SerialMon.println(*lastRecord);
  }
}


// METHOD: get console.txt records between two dates.  Dates specified as YYYY-MM-DD HH:MM:SS
int TIA_SdFat::TIA_getConsoleRecords(                                 // returns number of records read.
  char *destinationArray,                                             // pointer to array to hold console records
  String returnStartDateTimeString,                                   // return records starting at "YYYY-MM-DD HH:MM:SS"
  String returnEndDateTimeString,                                     // return records ending at "YYYY-MM-DD HH:MM:SS"
  int byteLimit                                                       // limit on the number of bytes to be returned      
)
{  
  char firstRecord[consoleRecordLength]         = "";                 // holds the first console record found
  char lastRecord[consoleRecordLength]          = "";                 // holds the last console record found
  
  char firstDateTime_YYYY_MM_DD_HH_MM_SS[20];                         // datetime of the first console record in the console file
  char endDateTime_YYYY_MM_DD_HH_MM_SS[20];                           // datetime of the last console record in the console file
  
  unsigned long int firstTimestampSeconds       = 0;                  // timestamp for the first console record in the console file
  unsigned long int lastTimestampSeconds        = 0;                  // timestamp for the last console record in the console file
    
  unsigned long int firstFilePosition           = 0;                  // file position for the start of the first console record in the console file
  unsigned long int lastFilePosition            = 0;                  // file position for the start of the last console record in the console file
  
  unsigned long int returnStartDateTimeSecondsSince1Jan2k  = secondsSince1Jan2000(returnStartDateTimeString);  // start reading console records at this datetime
  unsigned long int returnEndDateTimeSecondsSince1Jan2k    = secondsSince1Jan2000(returnEndDateTimeString);    // end reading console records after this datetime
  
  SdFile consoleFile;                                                 // console file
  char line[consoleRecordLength]                = "";
  unsigned long int timestampSeconds            = 0;
  
  SerialMon.print("<<< getting Console records, start="); SerialMon.print(returnStartDateTimeString);
  SerialMon.print(", end="); SerialMon.print(returnEndDateTimeString);
  SerialMon.print(", byteLimit="); SerialMon.print(byteLimit); SerialMon.println(F(" >>>"));
  
  // get the console profile
  boolean profileFlag = TIA_getConsoleProfile(
    &firstRecord,                                                     // copy of the first record found
    &lastRecord,                                                      // copy of the last record found
    &firstDateTime_YYYY_MM_DD_HH_MM_SS,                               // datetime of the first console record in the console file
    &endDateTime_YYYY_MM_DD_HH_MM_SS,                                 // datetime of the last console record in the console file
    &firstTimestampSeconds,                                           // timestamp for the first console record in the console file
    &lastTimestampSeconds,                                            // timestamp for the last console record in the console file
    &firstFilePosition,                                               // file position for the start of the first console record in the console file
    &lastFilePosition                                                 // file position for the start of the last console record in the console file
  );
  
  // check for errors in the requested dates relative to the dates in the console file
  if (returnStartDateTimeSecondsSince1Jan2k > returnEndDateTimeSecondsSince1Jan2k) return -1; // requested start dateTime is after requested end dateTime
  if (returnStartDateTimeSecondsSince1Jan2k > lastTimestampSeconds) return -2;                // requested start dateTime is after last console record dateTime
  if (returnEndDateTimeSecondsSince1Jan2k < firstTimestampSeconds) return -3;                 // requested end dateTime is before first console record dateTime
  
  // check for out of bounds requests
  if (returnStartDateTimeSecondsSince1Jan2k < firstTimestampSeconds) returnStartDateTimeSecondsSince1Jan2k = firstTimestampSeconds; // start at the first record
  if (returnEndDateTimeSecondsSince1Jan2k > lastTimestampSeconds) returnEndDateTimeSecondsSince1Jan2k = lastTimestampSeconds;       // end at the last record
  
  // determine what % returnStartDate is of the entire console file
  float pct = float(returnStartDateTimeSecondsSince1Jan2k - firstTimestampSeconds) * 100.0 / float(lastTimestampSeconds - firstTimestampSeconds);
  
  // determine the position at which to start the seach for the requested starting record
  unsigned long int position = (lastFilePosition - firstFilePosition) * pct/100;

  boolean debug = false;
  if (debug) {
    SerialMon.println("<<< CONSOLE FILE PROFILE >>> in getConsoleRecords()");
    SerialMon.println("\t\tDateTime\t\tTimestamp\tFile Position\tRecord");
    SerialMon.print(" First Record:\t");
    SerialMon.print(firstDateTime_YYYY_MM_DD_HH_MM_SS);SerialMon.print("\t");
    SerialMon.print(firstTimestampSeconds);SerialMon.print("\t");
    SerialMon.print(firstFilePosition);SerialMon.print("\t\t");
    SerialMon.println(firstRecord);
    SerialMon.print("  Last Record:\t");
    SerialMon.print(endDateTime_YYYY_MM_DD_HH_MM_SS);SerialMon.print("\t");
    SerialMon.print(lastTimestampSeconds);SerialMon.print("\t");
    SerialMon.print(lastFilePosition);SerialMon.print("\t\t");
    SerialMon.println(lastRecord);
    SerialMon.print(" Request from:\t");
    SerialMon.print(returnStartDateTimeString);SerialMon.print("\t");
    SerialMon.print(returnStartDateTimeSecondsSince1Jan2k);SerialMon.print("\t");
    SerialMon.print(position);SerialMon.print("\t\t");
    SerialMon.print("Start looking at ");SerialMon.print(pct);SerialMon.println("% of console file length");
    SerialMon.print("   Request to:\t");
    SerialMon.print(returnEndDateTimeString);SerialMon.print("\t");
    SerialMon.print(returnEndDateTimeSecondsSince1Jan2k);SerialMon.println("\t");
  }
  
  int loopCounter = 0;
  timestampSeconds = returnEndDateTimeSecondsSince1Jan2k;               // initialize the timestampSeconds
  
  // go backwards from this point until we find a record earlier than the requested start timestamp
  while (timestampSeconds >= returnStartDateTimeSecondsSince1Jan2k) {
    
    getPreviousRecord(&line[0], &position);                             // get the previous record
    timestampSeconds = secondsSince1Jan2kFromTimestamp(line);           // get the number of seconds since 1/1/2000 for this record
    
    SerialMon.print(".");                                               // print out a period to show progress
    loopCounter++;
    if (loopCounter >= 100) {                                           // break progress display into multiple lines
      loopCounter = 0;
      SerialMon.println("");
    }
  }
  
  if (debug) SerialMon.println("");
  
  if (!consoleFile.open("console.txt", O_READ)) {                       // if the file doesn't open
    SerialMon.println("Error 550: console.txt did not open.");
    return -1;                                                          // return an error code
  }
  
  consoleFile.seekSet(position);
  
  // get the next record of the console file
  consoleFile.fgets(line, consoleRecordLength);                         // get this whole console record
  timestampSeconds = secondsSince1Jan2kFromTimestamp(line);             // get the number of seconds since 1/1/2000 for this record
  
  int recordCounter = 0;
  int recordBytes   = 0;
  int totalBytes    = 0;
  
  // keep getting lines until we go past the requested end date
  while (
    (recordBytes = consoleFile.fgets(line, sizeof(line))) > 0 &&
    timestampSeconds <= returnEndDateTimeSecondsSince1Jan2k)
  {
    timestampSeconds = secondsSince1Jan2kFromTimestamp(line);           // get the number of seconds since 1/1/2000 for this record
    
    if (totalBytes + recordBytes > byteLimit) break;                    // don't go over the byte limit
    
   
    for (int i=0; i < recordBytes; i++) {
      *destinationArray = line[i];                                      // copy the line to the destination array
      destinationArray++;                                               // point to the next location in the destinationArray
    }
    
    totalBytes += recordBytes;                                          // add this record's bytes to the total byte count
    
  }
  
  *destinationArray = '\0';                                             // add string terminator
  
  return totalBytes;
}
