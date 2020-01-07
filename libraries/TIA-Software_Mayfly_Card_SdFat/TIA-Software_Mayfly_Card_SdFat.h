//  TIA-Softwaref_Mayfly_Card_SdFat.h - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#ifndef TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDFAT_VERSION 20200107

#include "TIA-Software_DCF_Globals.h"                             // global headers
#include "SdFat.h"                                                // SD Card support
#include "Sodaq_DS3231.h"                                         // Real Time Clock support

const int filenameLength = 200;                                   // maximum length of directory name or filename
   
    
typedef struct {                                                  // structure of an SD Card directory return
  int folderLevel;                                                // sub-directory depth, used to determine indentation level for displaying results
  boolean directoryFlag;                                          // true=directory, false=file
  char filename[filenameLength];                                  // directory or file name
  char modDateTime[20];                                           // modification date and time
  int sizeKb;                                                     // file size in KB
  boolean limitReached;                                           // true=more files may exist, but display limit reached
} SdCardDirectory;


class TIA_SdFat : public SdFat {
  public:

    TIA_SdFat();                                                  // constructor
    
    
    // setup the SD Card
    void TIA_setup();
    
    
    // list the files in the dir.  Returns the number of directory names + filenames
    int TIA_dir(                                                  
      SdCardDirectory *sd_card_directory,                         // array to hold directory results
      int limit                                                   // limit on the number of directory names + file names to be returned
    );
    

    // get the profile of console.txt
    boolean getConsoleProfile(
      char (*firstRecord)[consoleRecordLength],                   // returns first record found in the console file
      char (*lastRecord)[consoleRecordLength],                    // returns last record found in the console file
      
      char (*firstDateTime_YYYY_MM_DD_HH_MM_SS)[20],              // returns datetime of the first record in the console file
      char (*lastDateTime_YYYY_MM_DD_HH_MM_SS)[20],               // returns datetime of the last record in the console file
      
      unsigned long int *firstTimestampSeconds,                   // returns timestamp for the first record in the console file
      unsigned long int *lastTimestampSeconds,                    // returns timestamp for the last record in the console file
        
      unsigned long int *firstFilePosition,                       // returns file position for the start of the first record in the console file
      unsigned long int *lastFilePosition                         // returns file position for the start of the last record in the console file
    );

    
    // get records from the console file
    int getConsoleRecords(                                        // returns number of records read

      /* get records from console.txt, specifing starting and ending dates
       *
       *  Returns the number of bytes placed in the *destinationArray.
       *
       *  Error returns:
       *    -1 = requested start dateTime is after requested end dateTime
       *    -2 = requested start dateTime is after last console record dateTime
       *    -3 = requested end dateTime is before first console record dateTime
       */
      
      char *destinationArray,                                     // pointer to array to hold console records returned from call
      String startDateTimeString,                                 // start reading at "YYYY-MM-DD HH:MM:SS"
      String endDateTimeString,                                   // end reading at "YYYY-MM-DD HH:MM:SS"
      int byteLimit                                               // limit on the number of bytes to be returned      
    );
    
    
  protected:
    
    void processDirectory(                                        // recursively process SD card directory
      SdCardDirectory *sd_card_directory,                         // array of SD Card directory entries
      SdFile CFile,                                               // current file being read
      const char dirName[] = "Root",                                    // name of the current directory
      int numTabs = 0,                                            // number of tabs for displaying sub-directories
      int limit = 100                                             // limit on the number of directory+file names to be returned
    );                                         
};

#endif