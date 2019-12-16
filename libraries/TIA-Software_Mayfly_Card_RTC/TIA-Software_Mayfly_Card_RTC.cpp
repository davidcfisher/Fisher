//  TIA-Software_Mayfly_Card_RTC.cpp - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
//  v1.0

#include "TIA-Software_Mayfly_Card_RTC.h"                       // include the header file
String months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


// CONSTRUCTOR
TIA_RTC::RTC() {}                                               // constructor


// METHOD: getYearString()
String TIA_RTC::getYearString(DateTime dT)                      // get the year as a String
{
  return String(dT.year());
}


// METHOD: getMonthString()
String TIA_RTC::getMonthString(DateTime dT, String format)      // get the month as a String
{
  if (format == "MM") {
    char myMonth[3];                                              // this will hold the current month
    sprintf(myMonth, "%02u", dT.month());                         // ensure myMonth is two characters
    return String(myMonth);
  }
  
  else if (format = "Mmm") {
    return String(months[dT.month()-1]);
  }
  
  else return ("Error: unsupported month format requested: \"" + format  + "\" in file=" + String(__FILE__) + ", line=" + String(__LINE__));    
}


// METHOD: getDayOfMonthString()
String TIA_RTC::getDayOfMonthString(DateTime dT)                // get the day of the month as a String
{
  char myDoM[3];                                                // this will hold the current day of the month
  sprintf(myDoM, "%02u", dT.date());                            // ensure myDoW is two characters
  return String(myDoM);
}


// METHOD: getHourString()
String TIA_RTC::getHourString(DateTime dT)                      // get the hour as a String
{
  char myHour[3];                                               // this will hold the current hour
  sprintf(myHour, "%02u", dT.hour());                           // ensure myHour is two characters
  return String(myHour);
}


// METHOD: getMinuteString()
String TIA_RTC::getMinuteString(DateTime dT)                    // get the minute as a String
{
  char myMinute[3];                                             // this will hold the current minute
  sprintf(myMinute, "%02u", dT.minute());                       // ensure myMinute is two characters
  return String(myMinute);
}


// METHOD: getSecondString()
String TIA_RTC::getSecondString(DateTime dT)                    // get the second as a String
{
  char mySecond[3];                                             // this will hold the current second
  sprintf(mySecond, "%02u", dT.second());                       // ensure mySecond is two characters
  return String(mySecond);
}


// METHOD: getDateTimeNow()
DateTime TIA_RTC::getDateTimeNow()
{
  DateTime dT = rtc.now();
  return dT;
}


// METHOD: getDateNowString(String format="YYYY-MM-DD")
String TIA_RTC::getDateNowString(String format)
{
  DateTime dT = rtc.now();
  
  if (format == "YYYY-MM-DD")         return getYearString(dT) +            "-" + getMonthString(dT) +      "-"   + getDayOfMonthString(dT);
  else if (format == "MM/DD/YYYY")    return getMonthString(dT) +           "/" + getDayOfMonthString(dT) + "/"   + getYearString(dT);
  else if (format == "Mmm DD, YYYY")  return String(months[dT.month()-1]) + " " + getDayOfMonthString(dT) + ", "  + getYearString(dT);
  else if (format == "Mmm DD YYYY")   return String(months[dT.month()-1]) + " " + getDayOfMonthString(dT) + " "   + getYearString(dT);
  
  else return ("Error: unsupported date format requested: \"" + format  + "\" in file=" + String(__FILE__) + ", line=" + String(__LINE__));  
}


// METHOD: String getTimeNowString(String format)
String TIA_RTC::getTimeNowString(String format)
{
  DateTime dT =  rtc.now();                                           // get the current time as a DateTime object
  String ap = "AM";                                                   // assume this is the morning
  int myHour = dT.hour();                                             // get the current hour
  
  // FORMAT: HH:MM:SS
  if (format == "HH:MM:SS") return getHourString(dT) + ":" + getMinuteString(dT) + ":" + getSecondString(dT);
  
  // FORMAT: HH:MM AP
  if (format == "HH:MM AP") {
    if (myHour > 12) {                                                // if this is the afternoon
      myHour = myHour - 12;                                           // convert hours 13-23 to 1-11
      ap = "PM";                                                      // indicate afternoon
    }  
    
    return String(myHour) + ":" + getMinuteString(dT) + " " + ap;
  }
  
  return ("Error: unsupported time format requested: \"" + format  + "\" in file=" + String(__FILE__) + ", line=" + String(__LINE__));  
}


// METHOD:  getDateTimeNowString
String TIA_RTC::getDateTimeNowString(String dateFormat, String timeFormat)
{
  return getDateNowString(dateFormat) + " " + getTimeNowString(timeFormat);
}