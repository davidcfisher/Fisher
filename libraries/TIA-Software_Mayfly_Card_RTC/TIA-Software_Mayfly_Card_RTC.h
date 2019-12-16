//  TIA-Softwaref_Mayfly_Card_RTC.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#ifndef TIA_SOFTWARE_MAYFLY_CARD_RTC_H
#define TIA_SOFTWARE_MAYFLY_CARD_RTC_H
#define TIA_SOFTWARE_MAYFLY_CARD_RTC_VERSION 20191216

#include "Arduino.h"
#include "Sodaq_DS3231.h"                                   // Real Time Clock support


class TIA_RTC {
  public:
    
    RTC();                                                  // constructor
    
    DateTime getDateTimeNow();                              // get the current date and time as a DateTime object
    String getYearString(DateTime dT);                      // get the year as a String
    String getMonthString(DateTime dT, String format="MM");        // get the month as a String. formats: "MM", "Mmm"
    String getDayOfMonthString(DateTime dT);                // get the day of the month as a String
    String getHourString(DateTime dT);                      // get the hour as a String
    String getMinuteString(DateTime dT);                    // get the minute as a String
    String getSecondString(DateTime dT);                    // get the second as a String
    String getDateNowString(String format="YYYY-MM-DD");    // formats: "YYYY-MM-DD", "MM/DD/YYYY", "Mmm DD, YYYY", "Mmm DD YYYY"
    String getTimeNowString(String format="HH:MM:SS");      // formats: "HH:MM:SS", "HH:MM AP"
    String getDateTimeNowString(                            // get a string for the current date and time
      String dateFormat="YYYY-MM-DD",
      String timeFormat="HH:MM:SS"
    );
    
  protected:
    int _pin;                                               // LED pin number
};

#endif