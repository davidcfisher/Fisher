//  TIA-Softwaref_Mayfly_Card_SdFat.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDFAT_VERSION 20191220

#define consoleLineLength 1000                                    // maximum length of a console record

#include "TIA-Software_DCF_Globals.h"                             // global headers
#include "SdFat.h"                                                // SD Card support
#include "Sodaq_DS3231.h"                                         // Real Time Clock support
   
    
typedef struct {                                                  // structure of an SD Card directory return
  int folderLevel;                                                // sub-directory depth, used to determine indentation level for displaying results
  boolean directoryFlag;                                          // true=directory, false=file
  String filename;                                                // directory or file name
  String modDateTime;                                             // modification date and time
  int sizeKb;                                                     // file size in KB
  boolean limitReached;                                           // true=more files may exist, but display limit reached
} SdCardDirectory;


typedef struct {                                                  // structure of a console record
  String record;                                                  // record contents
  int bytes;                                                      // number of bytes in the record
} consoleRecord;


class TIA_SdFat : public SdFat {
  public:

    TIA_SdFat();                                                  // constructor

    void TIA_setup(                                               // setup the SD Card
      boolean testFlag=false                                      // true=test the file write, read and remove capabilities of the SD Card
    );

    int TIA_dir(                                                  // list the files in the dir.  Returns the number of directory names + filenames
      SdCardDirectory *sd_card_directory,                         // array to hold directory results
      int limit                                                   // limit on the number of directory names + file names to be returned
    );
    
    void TIA_getConsoleRecords(                                    // returns number of records read.  Error codes: -1=file didn't open, -2=end date before start date
      char *destinationArray,                                     // pointer to array to hold console records
      String startDateTimeString,                                 // start reading at "YYYY-MM-DD HH:MM:SS"
      String endDateTimeString,                                   // end reading at "YYYY-MM-DD HH:MM:SS"
      int byteLimit                                               // limit on the number of bytes to be returned      
    );
    
    int TIA_consoleReadLines(                                     // returns number of records read.  Error codes: -1=file didn't open, -2=end date before start date
      consoleRecord *console_record,                              // array to hold console records
      String startDateTimeString,                                 // start reading at "YYYY-MM-DD HH:MM:SS"
      String endDateTimeString,                                   // end reading at "YYYY-MM-DD HH:MM:SS"
      int limit                                                   // limit on the number of colsole records to be returned      
    );
    
  protected:
    void TIA_init();                                              // initialize the SD card
    void TIA_writeTest();                                         // SD card write test
    void TIA_readTest();                                          // SD card read test
    void TIA_removeTest();                                        // SD card remove file test
    void TIA_processDirectory(     
      SdCardDirectory *sd_card_directory,                         // array of SD Card directory entries
      SdFile CFile,                                               // current file being read
      String dirName="Root",                                      // name of the current directory
      int numTabs=0,                                              // number of tabs for displaying sub-directories
      int limit=100                                               // limit on the number of directory+file names to be returned
    );                                         
};

#endif