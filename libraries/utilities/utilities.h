

#define MARCH 3
#define NOVEMBER 11
#define SUNDAY 1
#define LEAP_YEAR(Y)     ( !((1970+(Y))%4) && ( ((1970+(Y))%100) || !((1970+(Y))%400) ) )
static  const int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // API starts months from 1, this array starts from 0


String getDateTime() { //Create a DateTime object from the current time & return the yyyy-mm-dd hh:mm:ss
	//Serial.println("running getDateTime") ;
	String dateTimeStr;
	DateTime dt(rtc.makeDateTime(rtc.now().getEpoch()));
	currentepochtime = (dt.getEpoch());    //Unix time in seconds
	currentyear = (dt.year()) ;
	currentmonth = dt.month() ;
	currentday = dt.date() ;	
	currenthour = (dt.hour());
	currentminute = (dt.minute());
	currentsecond = dt.second() ;
	currentdow = dt.dayOfWeek() ;
	//Convert it to a String
	dt.addToString(dateTimeStr); 
	return dateTimeStr;  
}

// gets the epoch time
uint32_t getNow() {
	currentepochtime = rtc.now().getEpoch();
	return currentepochtime;
}

// function get_dst  - does not change the RTClock, just determines the DST value
int get_dst() {
  // find any case where DST shouldbe set to 1 otherwise return 0
  if (currentmonth > MARCH && currentmonth < NOVEMBER) return 1 ;
  if (currentmonth == MARCH ) {
    if ( currentday > 16) return 1 ;
    else { // day is 1-15
      if (currentdow == SUNDAY) {
        if (currentday >= 8  && currenthour >= 2 ) return 1 ;
      }
      else if (currentday - currentdow >= 7 ) return 1;
    }
  }
  else if (currentmonth == NOVEMBER ) {
    if (currentday < 8) { //day is 1-7
      if (currentdow == SUNDAY) {
        if ( currenthour < 2 ) return 1 ;
      }
      else if (currentday - currentdow < 0) return 1 ;
    }
  }
  return 0 ;
}

// function check_dst_change Check for DST change - requires checking at least every hour, adjusts the clock appropriately
void check_dst_change(){
	int newDST = get_dst() ;
	if (DST != newDST) {
		DST = newDST ;
		if (DST == 1) { // Springing Forward
			 // add 1 hour to clock
			Serial.println("adding an hour") ;
			rtc.setEpoch(uint32_t ( rtc.now().getEpoch() + 3600 )); // add 3600 seconds to now
		}
		else {  // Fall Back
			Serial.println("subtracting an hour") ;
			rtc.setEpoch(uint32_t (rtc.now().getEpoch() - 3600)); // subtract 3600 seconds from now
		}
	}
}

// function to get epoch seconds from a date timestamp //////////////////////////////////////////////////////////////////////////////////
//uint32_t date_to_time(int year, int month, int day, int hr, int min, int sec) {	
uint32_t date_to_time(char * date) {
	char copy_date[20] ;  // don't let strtok modify "date"
	strlcpy(copy_date,date,20) ;
	char *r = strtok(copy_date, "-") ;
	int year = atoi(r) ;
	r = strtok(NULL, "-") ;
	int month = atoi(r) ;
	r = strtok(NULL, " ") ;
	int day = atoi(r) ;
	r = strtok(NULL, ":") ;
	int hour = atoi(r) ;
	r = strtok(NULL, ":") ;
	int min = atoi(r) ;
	r = strtok(NULL, " ") ;
	int sec = atoi(r) ;
	year = year - 1970 ;
	// seconds from 1970 till 1 jan 00:00:00 of the given year
	uint32_t seconds = 0L ; 
	for (int i = 0; i < year; i++) { // account for leap years
		seconds +=  31536000L ;
		if (LEAP_YEAR(i)) {
//Serial.print("leap Year seconds added ") ; Serial.println(i) ;
			seconds +=  86400L ;   // add extra days for leap years  3600 * 24
		}
	}
//Serial.print(" intermediate seconds: ") ;Serial.println(seconds) ;
	// add days for this year, months start from 1.  
	for (int i = 1; i < month; i++) { // add the previous months seconds
		if ( (i == 2) && LEAP_YEAR(year)) seconds += 2505600L ; // 3600 * 24 * 29; Special case for february
		else seconds += 86400L * monthDays[i - 1]; //   previous month - monthDay array starts from 0
	}
  seconds += (day-1) * 86400L ; // 3600 * 24 = seconds perday * every day of this monmth.
  seconds += hour * 3600L;  // for every hour
  seconds += min * 60L;   // for every minute
  seconds += sec ;       // for every second
  return seconds;
}

//// function to get timestamp from epoch seconds
// I don't use this and it gives compiler warnings, so....
//void time_to_date( char * timestamp , uint32_t epoch) {
//	int  month,day ;
//	uint32_t sec = epoch%60L;
////Serial.print(" sec: ") ; Serial.print(sec) ;
//	epoch /= 60L;  // epoch is now in minutes
//    uint32_t min = epoch%60L;
////Serial.print(" min: ") ; Serial.print(min) ;
//	epoch /= 60L;  // epoch is now in hours
//    uint32_t hour = epoch%24;
////Serial.print(" hour: ") ; Serial.print(hour) ;
//	epoch /= 24L;  // epoch is now in days
//	uint32_t days = epoch ;
//    uint32_t year = days/(365*4+1); // integer arith - year is  groups of 4 years
//	year *= 4 ; // real years
//	days %= 365*4+1; // epoch is stil in days after taking out groups of 4 years
//    for (int y=0; y<4; y++) { // take out the remaining years
//	   for (month=0; month<12; month++)  {
//			int test = days - monthDays[month] ; // subtract the days of this month 0=jan
//			if (month == 1 &&  LEAP_YEAR(year)) test-- ; // feb of leap year  
//			if (test < 0) goto NOMONTHS ;
//			days = test ; 
//	   }
//	   year++ ;
//	}
//NOMONTHS:
//	month++ ;
//	day = 1+days ;
//	year = year + 1970  ;
//	sprintf(timestamp,"%d-%02d-%02d %02d:%02d:%02d",year,month,day,hour,min,sec);
//}
//// function to get timestamp from epoch seconds - returns values in the call statement
// I don't use this and it gives compiler warnings, so....
//void time_to_date(int* year, int* month, int* day, int* hour, int* min, int* sec, uint32_t epoch) {
//	// the compiler doesn't like indexing an array with int* or doing arithmatic with int* and int as in LEAP_YEAR. So I had to int(month) and create int yr
//    sec = epoch%60;
//	epoch /= 60;  // epoch is now in minutes
//    min = epoch%60;
//	epoch /= 60;  // epoch is now in hours
//    hour = epoch%24;
//	epoch /= 24;  // epoch is now in days
//    int yr = epoch/(365*4+1); // integer arith - year is  groups of 4 years
//	yr *= 4 ; // real years
//	epoch %= 365*4+1; // epoch is stil in days after taking out groups of 4 years
//    for (int y=0; y<4; y++) { // take out the remaining years
//	   for (month=0; month<12; month++)  {
//			int test = epoch - monthDays[int(month)] ; // subtract the days of this month 0=jan
//			if (month == 1 &&  LEAP_YEAR(yr)) test-- ; // feb of leap year  
//			if (test <= 0) goto NOMONTHS ;
//			epoch = test ; 
//	   }
//	   yr++ ;
//	}
//NOMONTHS:
//	month++ ;
//	day = 1+epoch ;
//	year = yr + 1970  ;
//}


// get the lipo battery voltage *****************************************************************************************************************
float get_voltage(void) {
    float rawBattery = analogRead(A6);
    float voltage = (3.3 / 1023.) * 4.7 * rawBattery; // only works on Mayfly v0.5
    return voltage ;
}

// get the 12v battery voltage *****************************************************************************************************************
float get_12v_voltage() { // reads pin A0 and multiplies V12_FACTOR (100K - 10K  or 1M - 100K resistor divider)
	// get 10 samples
	int sample_count = 0 ;
	int sum = 0 ;
	while (sample_count < 3) {
		int read =  analogRead(A0);
		sum += read ;
		sample_count++;
		delay(10);
		//Serial.println(read) ;
	}
	float ave = float(sum)/3. ;
	//Serial.println(ave) ;
    //float voltage = (ave * 11. * 3.3) / 1024.   ;
    float voltage = ave * V12_FACTOR   ;
    return voltage ;
}

// FUNCTION to check if a date is valid.*****************************************************************************************************************
// returns 0 if valid, <0 if invalid
// // has to be a yyyy-mm-dd hh:mm:ss format,  does not check things like 12 months, days in month, year is reasonable
int validate_timestamp (char *ts, int length) {   
    if (length != 19)   return -1 ;
    for (int i = 0; i < 19; i++) {
        //Serial.println(ts[i]) ;
        if( i == 4 || i == 7){
          if (ts[i] != '-')  return -2 ; // bad date separator
        }
        else if( i == 10){
          if (ts[i] != ' ') return -3 ;  // no space between date and time
        }
        else if ( i == 13 || i == 16 ) {
          if ( ts[i] != ':')  return -4 ; // bad time separator
        }
		//else if( 0 != isdigit(ts+i))  return -4 ;  // not a number digit 
		//else if( 48 > ts+i || 57 < ts+i)  return -4 ;  // not a number digit
        else if( 48 > ts[i] || 57 < ts[i])  return -5 ;  // not a number digit 
    }
    return 0 ;
}

// FUNCTION to compare 2 dates*****************************************************************************************************************
//returns -n if date1 is < date2, 0 if dates are same and +n if date1 > date2 ( n is some int value)
int compare_timestamps (char *ts1, char *ts2) {
    int x ;
    if (0 == strncmp(ts1,ts2,4)) 
        if (0 == strncmp(ts1+5,ts2+5,2)) 
            if (0 == strncmp(ts1+8,ts2+8,2)) 
                if (0 == strncmp(ts1+11,ts2+11,2)) 
                    if (0 == strncmp(ts1+14,ts2+14,2)) 
                        if (0 == strncmp(ts1+17,ts2+17,2)) return 0 ;
                        else x = strncmp(ts1+17,ts2+17,2) ;
                    else x = strncmp(ts1+14,ts2+14,2) ;
                else x = strncmp(ts1+11,ts2+11,2) ;
            else x = strncmp(ts1+8,ts2+8,2) ;
        else x = strncmp(ts1+5,ts2+5,2) ;
    else x =  strncmp(ts1,ts2,4) ;
    if (x>0) return 1;
    else return -1 ;
}

///////////////////////////////////////////////////////////////////////////////
// function to convert float to character eliminating leading/trailing zeros
void float_to_char (float v, char * buf, int decimals) {
  char b[10] ;
  if (v == 0) {
    strcpy(buf, "0") ; // I can save space by sending back null string instead of zero. line could be 1,,,2,,
    return ;
  }
  dtostrf(v, 10, decimals, b);
  // eliminate spaces
  int i = 0 ;
  int j = 0 ;
  while (b[i] != '\0') {
    i++;;
    if (b[i] != ' ') {
      b[j] = b[i] ;
      j++ ;
    }
  }
  b[j] = '\0' ;

  // eliminate leading zeros
  int neg = 0 ;  // must skip over negative size
  if (b[0] == '-') {
    neg = 1 ;
  }
  int cnt = neg ;
  if (b[cnt] == '0') {
    while (b[cnt] != '\0') { // shift everything left to get rid of leading zero
      b[cnt] = b[cnt + 1] ;
      cnt++ ;
    }
  }

  // eliminate trailing zeros
  cnt = strlen(b) - 1 ;
  while (b[cnt] == '0') {
    b[cnt] = '\0' ;
    cnt-- ;
  }
  strcpy(buf, b) ;
}

// function to identify/remove non-numeric characters from a string //////////////////////////////////////////////////////////////////////////////////
int digits_only(const char *s){
    while (*s) {
        if (isdigit(*s) == 0) {
          Serial.print("Non-digit found here<") ;
          Serial.print(s) ;
          Serial.println(">") ;
          return 0;
        }
        *s++ ;
    }
    return 1;
}