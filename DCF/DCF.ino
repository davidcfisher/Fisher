#define DCF_version = 20200109

#include "TIA-Software_Mayfly_Card.h"
#include <MemoryFree.h>                                           // for memory command to see how much memory is left
#include "RTCTimer.h"                                             // Real Time Clock support
#include <Wire.h>                                                 // I2C devices 



/*** Fisher's Office ***/
/*                     */
#define DATA_HEADER "dateTime,logger,temp_mayfly,voltage_12v,depth,temp_water,voltage_lipo" // for sdi12
const char dataHeader[] = DATA_HEADER;
#define TINY_GSM_MODEM_XBEE                                       // Select for Digi brand WiFi or Cellular XBee's
const char beeModule[20]    = "DigiLTE-M";                        // module in the Bee socket
char logger_name[5]         = "Z" ;                               // SSET:  2 char limit.  A or B or z6 ... what's printed on cardreader. contained in each msmt line
int sense_period            = 15 ;                                // SSET:  Minutes between reading - should be evenly divisible into 60
char location[100]          = "Fisher's Office" ;                 // SSET: Leelanau Narrows, Crystal, GLWLC Narrows, GLWLC Dam, Culvert3 Upstream or Downstream
char msmt_file_name[50]     = "fisher.csv" ;                      // SSET: File for the msmts
char msmt_type[10]          = "sdi12" ;                           // get msmts from OTT or CS451 sensors
char sdi12_msmt_cmd[5]      = "0M!" ;                             // command for OTT  for Glen Lake and Crystal
double sdi12_conversion     = 1 ;                                 // multiply by 1 (no conversion) for Glen Lake and Crystal
char carrier[2]             = "0";                                // Autodetect cellular carrier

/*** common variables ***/
int read_buffer_size        = 1024;                               // SSET: read buffer initially 1024 but can be set by server command.  See function: "read_incoming_data"
char * read_buffer;                                               // malloc in setup.  read buffer initially 1024 but can be set by server command.  
char host[100]              = "132.148.85.45";                    // SSET: "lake-man.com" or "132.148.85.45" so we don't have to do look-ups
char resource[100]          = "/scripts/mayfly_server.pl";        // SSET: the server script on the host
char parms[200]             = "asdf=qwer";                        // the parameters I send to the server - only used for http_get
char access_point_name[20]  = "hologram";                         // SSET:  or whatever your apn is for your SIM card
int num_batches             = 10;                                 // SSET: max number of batches to send in one server connection (don't want the small battery to drain transmitting for a long time?)
long int batchsize          = 6000;                               // SSET: max size of msmt bytes to send. each day is about 5760 bytes 24*4*60 if uncompressed, or about 1/3 that if compressed
char * send_buf;                                                  // malloc in setup
float min_xbee_voltage      = 3.5;                                // SSET:  don't wake XBee up if less than this. Documentation suggests 3.3 - 4.3v
// wake_bee event times
int wake_bee_hour[24];
int wake_bee_min[24];
int enable_modem            = 1;                                  // enable the xbee.  If disabled  (=0) then msmts can be recorded but the modem never turns on and Dialog is never entered.
char wake_bee_event_string[200] = "0:0;1:0;2:0;3:0;4:0;5:0;6:0;7:0;8:0;9:0;10:0;11:0;12:0;13:0;14:0;15:0;16:0;17:0;18:0;19:0;20:0;21:0;22:0;23:0"; // SSET: 
int num_wakes               = 0;                                  // number of wake times. gets computed on boot or when event_string is set by server
int compression             = 1;                                  // SSET: if = 1, will compress transmissions of msmts
int retry_cnt_1             = 0;                                  // number of times we've done the 1st pass retry - sent in "status"
int retry_cnt_2             = 0;                                  // number of times we've done the 2nd pass retry - sent in "status"
String email_txt            = "";                                 // In dialog, if email_txt is not empty, it will send text to server to send to Cal via email.  Various things/events/failures add to email_txt
char server_lastdate[20]    = "";                                 // last msmt record date that was sent & confirmed by server.  Blank if not known

/*** used in compression. holds the essential data of a msmt line ***/
struct msmt_line {
  long seconds;                                                   // epoch time
  char id[5];                                                     // logger_id
  double parms[20];                                               // all the msmt values
  int parm_cnt              = 0;                                  // number of msmts
  int valid                 = 0;                                  // this structure has been loaded
  long last_delta;                                                // the number of seconds between this msmt and the previous msmt
};

struct msmt_line m1;                                              // previous values of msmt line in buffer to be sent ( compressed or not)
struct msmt_line m2;                                              // next msmt line to be compared to previous line for compression (delta modulation)


/*** date stuff ***/
int set_clock           = 0 ;                                     // > 0 to set the clock
// Load a 2nd time with set_clock = 0 so that a battery swap won't reset the clock
// Sun=1 Mon=2 Tue=3 Wed=4 Thu=5 Fri=6 Sat=7
// year, month, day, hour, min, sec and week-day(starts from 0 and goes to 6)
// writing any non-existent time-data or week-day screws up theRTC.
DateTime dt(2019, 1, 8, 16, 58, 0, 2);                            // no leading zeroes    it takes about 30 seconds to load so enter time 30 seconds into future
int DST;                                                          // daylight saving time
int currentyear, currentmonth, currentday, currenthour, currentminute, currentsecond, currentdow;       // day of week mon=1 .... sun=7
long currentepochtime;

float boardtemp;
float temp_water;
double pressure_water;
float temp_air;
double pressure_air;

// object initialization----------------------------------------------------------------------------------------------------------------------
//MS5803 sensor_air;                                                // Create the MS5803 sensor objects
//MS5803 sensor_water;
SdFat SD;                                                         // SD card
RTCTimer timer;                                                   // Real Time Clock timer

#include <TinyGsmClient.h>                                        // included below. This include must be included below the define of the modem name!
TinyGsm modem(SerialAT);                                          // comment this line
TinyGsmClient client(modem);
#include <xBee_TIA-Software.h>                                    // TIA-Software XBEE library - must be included AFTER the variable definitions


// following variables are set when a call is made to profile the console file:  mfc.sd.getConsoleProfile()
char firstRecord[consoleRecordLength];                            // returns first record found in the console file
char lastRecord[consoleRecordLength];                             // returns last record found in the console file 
char firstDateTime_YYYY_MM_DD_HH_MM_SS[20];                       // returns datetime of the first record in the console file
char lastDateTime_YYYY_MM_DD_HH_MM_SS[20];                        // returns datetime of the last record in the console file
unsigned long int firstTimestampSeconds;                          // returns timestamp for the first record in the console file
unsigned long int lastTimestampSeconds;                           // returns timestamp for the last record in the console file
unsigned long int firstFilePosition;                              // returns file position for the start of the first record in the console file
unsigned long int lastFilePosition;                               // returns file position for the start of the last record in the console file

Mayfly_card mfc;                                                  // establish instance of Mayfly Card


void setup()
{
  // setup the Mayfly Card
  mfc.setup(
    &logger_name[0],                                              // Mayfly ID
    &dataHeader[0],                                               // definition of contents of measurement file
    &msmt_file_name[0],                                           // measurement file name
    &beeModule[0],                                                // module in the Bee socket
    &location[0]                                                  // Mayfly deployment location
  );
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//  logAndPrint(F("Mayfly just booted")) ;
  mfc.sd.log(String("Mayfly just booted"));

  // !!!!!!!!!!!!!  CAL - LOOK AT THIS
  //rtc.setEpoch(uint32_t (1552182900L)); // sets clock to 2019-3-10 right before DST

  // initialize the variables from EEProm
  // Eeprom_Class eeprom;// create an EEPROM instance
  // eeprom.initializeProvisioningVariables() ;

//  // 2 Malloc's for variables - read_buffer and send_buf
//  read_buffer = (char *)malloc(read_buffer_size ); // read buffer initially 1024 but can be set by server command.  It won't grow automatically in case someone sends junk to this Mayfly
  char read_buffer[read_buffer_size];
  
//  send_buf = (char *)malloc(batchsize ); // get memory for the parms and the data and for a work area
//  if (!send_buf ) {
//    logAndPrint("Could not get storage for send_buf. Hanging....") ;
//    while (1) {}
//  }
  char send_buf[batchsize];

  // set the clock IF required
  if (set_clock > 0) {
    Wire.begin();
    rtc.begin();
    rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above
    SerialMon.println("Setting Clock") ;
  }

//  //Initialize the serial connection
//  SerialMon.begin(57600);   // communication to/from Mayfly over USB
//  //SerialAT.begin(9600) ;  // Baud Rate is determined in setup_xbee.  Legal values 9600 and 19200. 19200 may make it easier to attach battery without hanging the modem
  rtc.begin();
  delay(100);
//  pinMode(8, OUTPUT);
//  pinMode(9, OUTPUT);
//  greenred4flash();    //blink the LEDs to show the board is on
  mfc.greenLED.blink(4);                                          // blink the LEDs to show the board is on

/*  
  setupLogFile();
  setupTimer();        //Setup timer events
  setupSleep();        //Setup sleep mode
  SerialMon.print("Power On.  Will read sensors every ");
  SerialMon.print(sense_period) ;
  SerialMon.print(" minutes at location: ") ;
  SerialMon.println(location) ;
  showTime(getNow());
  DST = get_dst() ; // set the DST according to the date & time
  Serial.print(currentyear) ; Serial.print("-") ;
  Serial.print(currentmonth) ; Serial.print("-") ;
  Serial.print(currentday) ; Serial.print(" ") ;
  Serial.print(currenthour) ; Serial.print(":") ;
  Serial.print(currentminute) ; Serial.print(":") ;
  Serial.print(currentsecond) ; Serial.print("  DOW:") ;
  Serial.print(currentdow) ; Serial.print(" DST: ") ;
  Serial.print(DST) ; Serial.print(" Epoch: ")  ;
  Serial.println(currentepochtime) ;

  // Power the sensors;
  if (POWER_PIN > 0) {
    SerialMon.println("Powering up sensors...");
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, HIGH);
    delay(200);
  }
  
  // MS5803 setup
  // Begin class with selected I2C address of sensor and max pressure range
  //  ADDRESS_water = 0x76
  //  ADDRESS_air  = 0x77
  sensor_water.begin(0x76, 1); // Address, Max pressure range (use "1" for 01BA which is exactly 1/10 of 14BA)
  sensor_air.begin(0x77, 1);
  //Retrieve calibration constants for conversion math.
  sensor_air.reset(); 
  sensor_water.reset();
  // end MS5803
  
  mySDI12.begin();  //setup SDI12 comm to CS451  or OTT sensor
  //mySDI12.sendCommand("0XCONFIG=1,4,1.0,0.0!");     // configure sensor for Feet and F° units  - didn't seem to work

  setup_xbee() ;  // this code gets the modem ready
  // maybe put a function here that will contact the server to get latest variables on power-on.
  wake_bee_arrays() ;  // determine hour:min pairs for waking the xbee - put this inside setup_xbee

  SerialMon.print("read_buffer_size") ;   SerialMon.println(read_buffer_size) ;
  SerialMon.print("host ") ;          SerialMon.println(host ) ;
  SerialMon.print("resource ") ;        SerialMon.println(resource ) ;
  SerialMon.print("access_point_name ") ;   SerialMon.println(access_point_name ) ;
  SerialMon.print("batchsize ") ;       SerialMon.println(batchsize) ;
  SerialMon.print("num_batches ") ;     SerialMon.println(num_batches) ;
  SerialMon.print("wake_bee_event_string ");  SerialMon.println(wake_bee_event_string ) ;
  SerialMon.print("min_xbee_voltage ") ;    SerialMon.println(min_xbee_voltage ) ;
  SerialMon.print("sense_period ") ;      SerialMon.println(sense_period ) ;
  SerialMon.print("location " ) ;       SerialMon.println(location ) ;
  SerialMon.print("msmt_file_name ") ;    SerialMon.println(msmt_file_name ) ;
  SerialMon.print("logger_name ") ;       SerialMon.println(logger_name ) ;

  email_txt = "Mayfly booted up: " ;
  email_txt += getDateTime() ;
  
  logAndPrint(F("Mayfly booted")) ;
*/









  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  mfc.redLED.turnOn();                                            // turn on the Red LED
  mfc.greenLED.turnOn();                                          // turn on the Green LED
  delay(1000);                                                    // wait a second
  mfc.redLED.turnOff();                                           // turn off the red LED

  // get the console profile
  mfc.sd.getConsoleProfile(
    &firstRecord,                                                 // set with the first record found in the console file
    &lastRecord,                                                  // set with the last record found in the console file
    &firstDateTime_YYYY_MM_DD_HH_MM_SS,                           // set with the datetime of the first record in the console file
    &lastDateTime_YYYY_MM_DD_HH_MM_SS,                            // set with the datetime of the last record in the console file
    &firstTimestampSeconds,                                       // set with the timestamp of the first record (seconds since 1/1/2000)
    &lastTimestampSeconds,                                        // set with the timestamp of the last record (seconds since 1/1/2000)
    &firstFilePosition,                                           // set with the file position of the first record in the console file
    &lastFilePosition                                             // set with the file position of the last record in the console file
  );

  // get the console information
  const int byteLimit = 6000;                                     // return full console records, not to exceed this total number of bytes
  char consoleRecords[byteLimit];                                 // return the console records into this array
  char startDate[] = "2019-12-31 23:59:59";                       // return console records starting at this dateTime
  char endDate[] = "2020-09-08 12:30:00";                         // return console records ending at this dateTime
  int numberOfConsoleBytes = mfc.sd.getConsoleRecords(&consoleRecords[0], startDate, endDate, byteLimit);

  // get the directory from the SD Card
  const int sdCardDirectoryLimit = 10;                            // limit the number of directory names + file names to be displayed
  SdCardDirectory sd_card_directory[sdCardDirectoryLimit];        // define an array to hold the SD Card directory results
  int numberOfEntries = mfc.sd.TIA_dir(&sd_card_directory[0], sdCardDirectoryLimit);     // get the SD Card directory & file names




  Serial.println(F("")); Serial.println(F("===== Starting Sketch Output ====="));
 
  /***** this code to display console.txt profile *****/
  /*                                                  */
  Serial.println(F("")); Serial.println(F("<<<<< CONSOLE FILE PROFILE >>>>>"));
  Serial.println(F("\t\tDateTime\t\tTimestamp\tFile Position\tRecord"));
  Serial.print(F(" First Record:\t"));
  Serial.print(firstDateTime_YYYY_MM_DD_HH_MM_SS); Serial.print(F("\t"));
  Serial.print(firstTimestampSeconds); Serial.print(F("\t"));
  Serial.print(firstFilePosition); Serial.print(F("\t\t"));
  Serial.println(firstRecord);
  Serial.print(F("  Last Record:\t"));
  Serial.print(lastDateTime_YYYY_MM_DD_HH_MM_SS); Serial.print(F("\t"));
  Serial.print(lastTimestampSeconds); Serial.print(F("\t"));
  Serial.print(lastFilePosition); Serial.print(F("\t\t"));
  Serial.println(lastRecord);  
  
  
  /***** this code displays the console records *****/
  /*                                                */
  Serial.println(F("")); Serial.println(F("<<< CONSOLE RECORDS >>>"));
  Serial.print(numberOfConsoleBytes); Serial.println(F(" bytes returned:"));
  Serial.println(consoleRecords);


  /***** this code displays the directory information *****/
  /*                                                      */
  // process each file
  for (int i=0; i < numberOfEntries; i++) {

    if (sd_card_directory[i].directoryFlag) Serial.println(F(""));                               // print a blank line before a directory entry

    for (int j=0; j<sd_card_directory[i].folderLevel; j++) { Serial.print(F("\t")); }            // add tabs to indent sub-directory level

    // if this is a directory entry
    if (sd_card_directory[i].directoryFlag) {
      Serial.print(F("<<< ")); Serial.print(sd_card_directory[i].filename); Serial.println(F(" >>>"));
    }

    // otherwise, this is a file
    else {
      Serial.print(sd_card_directory[i].filename);        Serial.print(F("\t"));
      if (strlen(sd_card_directory[i].filename) <= 12)    Serial.print(F("\t"));
      Serial.print(sd_card_directory[i].modDateTime);     Serial.print(F("\t"));
      Serial.print(sd_card_directory[i].sizeKb);          Serial.println(F(" KB"));
    }

    // if we've reached the limit of directory+file names
    if(sd_card_directory[i].limitReached) {
      Serial.println(F(""));Serial.print(F("=== More files may exist.  Maximum display limit of ")); Serial.print(sdCardDirectoryLimit); Serial.println(F(" reached. ==="));
      break;
    }
  }
}


void loop() {
  delay(500);
  mfc.greenLED.switchState();
}
