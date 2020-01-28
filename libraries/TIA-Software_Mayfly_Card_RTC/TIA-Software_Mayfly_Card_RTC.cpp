//  TIA-Software_Mayfly_Card_RTC.cpp - Copyright (c) 2019-2020 TIA Software, LLC.  All rights reserved.

#include "TIA-Software_Mayfly_Card_RTC.h"                         // include the header file
String months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


// CONSTRUCTOR
TIA_RTC::TIA_RTC() {}                                             // constructor


// METHOD: TIA_setup()                                            // setup the real time clock
void TIA_RTC::TIA_setup()
{
}


// METHOD: getYearString()
String TIA_RTC::getYearString(DateTime dT)                        // get the year as a String
{
  return String(dT.year());
}


// METHOD: getMonthString()
String TIA_RTC::getMonthString(DateTime dT, String format)        // get the month as a String
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

// METHOD: adjust the Mayfly clock, using seconds since 1/1/2000
boolean TIA_RTC::adjustClock(long int mayflyDtSeconds)
{
  DateTime mayflyDT;                                                            // holds DateTime of the Mayfly
  char mayflyDtArray[22];                                                       // holds the date string, format: "Jan 29, 1954 23:04:33"
  char keyboardChar;                                                            // holds a character from the keyboard
  char lastKeyboardChar;                                                        // holds the last character, so it can be repeated without re-entering
  char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};  // format used by DateTime functions
  boolean breakout = false;                                                     // true=break out of the proposed time adjustment loop

  while (!breakout) {                                                           // keep adjusting the proposed time until it what we like
    mayflyDT = DateTime(mayflyDtSeconds);                                       // get a DateTime object corresponding to the seconds
    sprintf(                                                                    // create the date string to display
      mayflyDtArray,
      "%s %02d, %4d %02d:%02d:%02d", 
      months[mayflyDT.month()-1], mayflyDT.date(), mayflyDT.year(), mayflyDT.hour(), mayflyDT.minute(), mayflyDT.second()
    );
  
    Serial.println(F("\n================================================"));
    Serial.print(F("   Enter an adjustment to: ")); Serial.println(mayflyDtArray);

    Serial.println(F("          Increment Decrement"));
    Serial.println(F("          --------- ---------"));
    Serial.println(F("     Hour:    h         r"));
    Serial.println(F("   Minute:    m         e"));
    Serial.println(F("   Second:    s         d\n"));

    Serial.println(F("   Repeat last adjustment: ENTER"));
    Serial.println(F("   Set Mayfly and continue:    c"));
    Serial.println(F("   Exit without setting clock: x"));

    while (Serial.available() > 0) char trash = Serial.read();                    // strip off any newlines
    while (Serial.available() <= 0);                                              // wait for a keyboard entry
    keyboardChar = Serial.read();                                                 // get a character from the keyboard
    
    if (keyboardChar == 10) {                                                     // if the ENTER key was pressed by itself
      keyboardChar = lastKeyboardChar;                                            // repeat the last character
    }

    lastKeyboardChar = keyboardChar;                                              // save the keyboard char so it can be reused
    
    switch (keyboardChar) {                                                       // handle the character
      case 'S': case 's': mayflyDtSeconds++;        break;                        // increase second
      case 'D': case 'd': mayflyDtSeconds--;        break;                        // decrease second
      case 'M': case 'm': mayflyDtSeconds += 60;    break;                        // increase the minute
      case 'E': case 'e': mayflyDtSeconds -= 60;    break;                        // decrease the minute
      case 'H': case 'h': mayflyDtSeconds += 3600;  break;                        // increase the hour
      case 'R': case 'r': mayflyDtSeconds -= 3600;  break;                        // decrease the hour
      case 'X': case 'x': return false;                                           // exit the method
      case 'C': case 'c':                                                         // set the Mayfly to this value
      
        rtc.setEpoch(mayflyDtSeconds + 946684800);                                // translate from Epoch-from-1/1/2000 to Epoch-from-1/1/1970; set the Mayfly date and time
        delay(100);                                                               // let the clock and serial input settle down
        
        while (Serial.available() > 0) char trash = Serial.read();                // strip off any newlines

        // display the current time every second, so it can be verified
        while (Serial.available() <= 0) {                                         // wait for a keyboard entry
          delay(1000);
          mayflyDT = getDateTimeNow();                                            // get the current DateTime
          sprintf(                                                                // create the date string to display
            mayflyDtArray,
            "%s %02d, %4d %02d:%02d:%02d", 
            months[mayflyDT.month()-1], mayflyDT.date(), mayflyDT.year(), mayflyDT.hour(), mayflyDT.minute(), mayflyDT.second()
          );
      
          Serial.print(F("\nCurrent Mayfly date & time: ")); Serial.println(mayflyDtArray);
          Serial.println(F("Press ENTER to continue"));
        }
        
        delay(100);                                                               // needed to let the serial input settle down
        while (Serial.available() > 0) char trash = Serial.read();                // strip off any newlines
      
        breakout = true;
        break;
      default:
        Serial.print(F("\nERROR: unknown request - key pressed = ")); Serial.println(keyboardChar);
        break;
    }
  }
  return true;
}