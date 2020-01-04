//  TIA-Softwaref_Mayfly_Card_SdTest.h - Copyright (c) 2020 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_SDTEST_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDTEST_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDTEST_VERSION 20200104

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
    void TIA_setup();
    
    
  protected:
};

#endif