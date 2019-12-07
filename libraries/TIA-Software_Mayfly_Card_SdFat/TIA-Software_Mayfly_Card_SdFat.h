//  TIA-Softwaref_Mayfly_Card_SdFat.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDFAT_VERSION 20191205

#include "TIA-Software_DCF_Globals.h"                         // global headers
#include "SdFat.h"                                            // library to handle SD Card

typedef struct {                                              // structure of an SD Card directory entry
  int folderLevel;                                            // sub-directory depth
  boolean directoryFlag;                                      // true=directory, false=file
  String filename;                                            // directory or file name
  String modDateTime;                                         // modification date and time
  int sizeKb;                                                 // file size in kb
} SdCardDirectory;

class TIA_SdFat : public SdFat {
  public:
    TIA_SdFat();                                              // constructor
    void TIA_setup(boolean testFlag=false, boolean debugFlag=false);    // setup the SD Card
    int TIA_dir(SdCardDirectory *sd_card_directory, boolean debugFlag=false);                     // list the files in the dir
    
  protected:
    void TIA_init(boolean debugFlag=false);                   // initialize the SD card
    void TIA_writeTest(boolean debugFlag=false);              // SD card write test
    void TIA_readTest(boolean debugFlag=false);               // SD card read test
    void TIA_removeTest(boolean debugFlag=false);             // SD card remove file test
    void TIA_printDirectory(
      SdCardDirectory *sd_card_directory,                     // array of SD Card directory entries
      SdFile CFile,                                           // current file being read
      String dirName="Root",                                  // name of the current directory
      int numTabs=0,                                          // number of tabs to be indented for readability
      boolean debugFlag=false);                               // true=print out debug information
};

#endif