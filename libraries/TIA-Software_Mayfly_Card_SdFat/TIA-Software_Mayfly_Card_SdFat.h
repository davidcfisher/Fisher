//  TIA-Softwaref_Mayfly_Card_SdFat.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H
#define TIA_SOFTWARE_MAYFLY_CARD_SDFAT_H

#include "Arduino.h"
#include "TIA-Software_DCF_Globals.h"                         // global headers
#include "SdFat.h"                                            // library to handle SD Card

class TIA_SdFat : public SdFat {
  public:
    TIA_SdFat();                                              // constructor
    void TIA_setup(boolean debugFlag=false);                  // setup the SD Card
    void TIA_ls(boolean debugFlag=false);                     // list the files in the dir
    
  protected:
    void TIA_printDirectory(SdFile CFile, int numTabs, boolean debugFlag=false);
};

#endif