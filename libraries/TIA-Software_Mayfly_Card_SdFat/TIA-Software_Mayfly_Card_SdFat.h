//  TIA-Softwaref_Mayfly_Card_SdFat.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDFAT_VERSION 20191209

#include "TIA-Software_DCF_Globals.h"                             // global headers
#include "SdFat.h"                                                // library to handle SD Card
    
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
    void TIA_setup(                                               // setup the SD Card
      boolean testFlag                                            // true=test the file write, read and remove capabilities of the SD Card
    );
    int TIA_dir(                                                  // list the files in the dir.  Returns the number of directory names + filenames
      SdCardDirectory *sd_card_directory,                         // array to hold directory results
      int limit                                                   // limit on the number of directory names + file names to be returned
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