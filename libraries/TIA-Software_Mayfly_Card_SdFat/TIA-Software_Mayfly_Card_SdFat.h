//  TIA-Softwaref_Mayfly_Card_SdFat.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H

#include "TIA-Software_DCF_Globals.h"                         // global headers
#include "SdFat.h"                                            // library to handle SD Card

class TIA_SdFat : public SdFat {
  public:
    TIA_SdFat();                                              // constructor
    void TIA_setup(boolean testFlag=false, boolean debugFlag=false);    // setup the SD Card
    void TIA_dir(boolean debugFlag=false);                    // list the files in the dir
    
  protected:
    void TIA_init(boolean debugFlag=false);                   // initialize the SD card
    void TIA_writeTest(boolean debugFlag=false);              // SD card write test
    void TIA_readTest(boolean debugFlag=false);               // SD card read test
    void TIA_removeTest(boolean debugFlag=false);             // SD card remove file test
    void TIA_printDirectory(SdFile CFile, String dirName, int numTabs, boolean debugFlag=false);
};

#endif