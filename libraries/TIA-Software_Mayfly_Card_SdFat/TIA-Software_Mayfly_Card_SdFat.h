//  TIA-Softwaref_Mayfly_Card_SdFat.h - Copyright (c) 2019 - 2020 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDFAT_VERSION 20200102

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



class TIA_SdFat : public SdFat {
  public:

    TIA_SdFat();                                                  // constructor
    
    
    // setup the SD Card
    void TIA_setup(                                               
      boolean testFlag=false                                      // true=test the file write, read and remove capabilities of the SD Card
    );
    
    
    // list the files in the dir.  Returns the number of directory names + filenames
    int TIA_dir(                                                  
      SdCardDirectory *sd_card_directory,                         // array to hold directory results
      int limit                                                   // limit on the number of directory names + file names to be returned
    );
    

    // get the profile of console.txt
    boolean getConsoleProfile(
      char (*firstRecord)[consoleRecordLength],                   // first record found in the console file
      char (*lastRecord)[consoleRecordLength],                    // last record found in the console file
      
      char (*firstDateTime_YYYY_MM_DD_HH_MM_SS)[20],              // datetime of the first console record in the console file
      char (*lastDateTime_YYYY_MM_DD_HH_MM_SS)[20],               // datetime of the last console record in the console file
      
      unsigned long int *firstTimestampSeconds,                   // timestamp for the first console record in the console file
      unsigned long int *lastTimestampSeconds,                    // timestamp for the last console record in the console file
        
      unsigned long int *firstFilePosition,                       // file position for the start of the first console record in the console file
      unsigned long int *lastFilePosition                         // file position for the start of the last console record in the console file
    );

    
    int getConsoleRecords(                                        // returns number of records read.  Error codes: -1=file didn't open, -2=end date before start date

      /* get records from console.txt, specifing starting and ending dates
       *
       *  Returns the number of bytes placed in the *destinationArray.
       *
       *  Error returns:
       *    -1 = requested start dateTime is after requested end dateTime
       *    -2 = requested start dateTime is after last console record dateTime
       *    -3 = requested end dateTime is before first console record dateTime
       */
      
      char *destinationArray,                                     // pointer to array to hold console records
      String startDateTimeString,                                 // start reading at "YYYY-MM-DD HH:MM:SS"
      String endDateTimeString,                                   // end reading at "YYYY-MM-DD HH:MM:SS"
      int byteLimit                                               // limit on the number of bytes to be returned      
    );
    
    
  protected:
    void TIA_init();                                              // initialize the SD card
    void TIA_writeTest();                                         // SD card write test
    void TIA_readTest();                                          // SD card read test
    void TIA_removeTest();                                        // SD card remove file test
    void TIA_processDirectory(     
      SdCardDirectory *sd_card_directory,                         // array of SD Card directory entries
      SdFile CFile,                                               // current file being read
      String dirName = "Root",                                    // name of the current directory
      int numTabs = 0,                                            // number of tabs for displaying sub-directories
      int limit = 100                                             // limit on the number of directory+file names to be returned
    );                                         
};

#endif