#define DCF_version = 20200107

#include "TIA-Software_Mayfly_Card.h"
#include "RTCTimer.h"                                             // Real Time Clock support
#include <MemoryFree.h>                                           // for memory command to see how much memory is left


/*** Fisher's Office, id=Z ***/
#define DATA_HEADER "dateTime,logger,temp_mayfly,voltage_12v,depth,temp_water,voltage_lipo" // for sdi12
#define TINY_GSM_MODEM_XBEE                                       // Select for Digi brand WiFi or Cellular XBee's
const char beeModule[]  = "DigiLTE-M";                            // module in the Bee socket
char logger_name[5]     = "Z" ;                                   // SSET:  2 char limit.  A or B or z6 ... what's printed on cardreader. contained in each msmt line
int sense_period        = 15 ;                                    // SSET:  Minutes between reading - should be evenly divisible into 60
char location[100]      = "Fisher's Office" ;                     // SSET: Leelanau Narrows, Crystal, GLWLC Narrows, GLWLC Dam, Culvert3 Upstream or Downstream
char msmt_file_name[50] = "fisher.csv" ;                          // SSET: File for the msmts
char msmt_type[10]      = "sdi12" ;                               // get msmts from OTT or CS451 sensors
char sdi12_msmt_cmd[5]  = "0M!" ;                                 // command for OTT  for Glen Lake and Crystal
double sdi12_conversion = 1 ;                                     // multiply by 1 (no conversion) for Glen Lake and Crystal
char carrier[2]         = "0";                                    // Autodetect cellular carrier
Mayfly_card mayflyCard;                                           // establish instance of Mayfly Card
                        
/*** common variables ***/
int read_buffer_size        = 1024;         // SSET: read buffer initially 1024 but can be set by server command.  See function: "read_incoming_data"
char * read_buffer;               // malloc in setup.  read buffer initially 1024 but can be set by server command.  
char host[100]              = "132.148.85.45";        // SSET: "lake-man.com" or "132.148.85.45" so we don't have to do look-ups
char resource[100]          = "/scripts/mayfly_server.pl"; // SSET: the server script on the host
char parms[200]             = "asdf=qwer";        // the parameters I send to the server - only used for http_get
char access_point_name[20]  = "hologram";     // SSET:  or whatever your apn is for your SIM card
int num_batches             = 10;              // SSET: max number of batches to send in one server connection (don't want the small battery to drain transmitting for a long time?)
long int batchsize          = 6000;           // SSET: max size of msmt bytes to send. each day is about 5760 bytes 24*4*60 if uncompressed, or about 1/3 that if compressed
char * send_buf ; // malloc in setup
float min_xbee_voltage      = 3.5;          // SSET:  don't wake XBee up if less than this. Documentation suggests 3.3 - 4.3v
// wake_bee event times
int wake_bee_hour[24] ;
int wake_bee_min[24] ;
int enable_modem            = 1;              // enable the xbee.  If disabled  (=0) then msmts can be recorded but the modem never turns on and Dialog is never entered.
char wake_bee_event_string[200] = "0:0;1:0;2:0;3:0;4:0;5:0;6:0;7:0;8:0;9:0;10:0;11:0;12:0;13:0;14:0;15:0;16:0;17:0;18:0;19:0;20:0;21:0;22:0;23:0"; // SSET: 
int num_wakes               = 0;               // number of wake times. gets computed on boot or when event_string is set by server
int compression             = 1;             // SSET: if = 1, will compress transmissions of msmts
int retry_cnt_1             = 0;               // number of times we've done the 1st pass retry - sent in "status"
int retry_cnt_2             = 0;               // number of times we've done the 2nd pass retry - sent in "status"
String email_txt            = "";             // In dialog, if email_txt is not empty, it will send text to server to send to Cal via email.  Various things/events/failures add to email_txt
char server_lastdate[20]    = "";         // last msmt record date that was sent & confirmed by server.  Blank if not known

/*** used in compression. holds the essential data of a msmt line ***/
struct msmt_line {
  long seconds;                                                   // epoch time
  char id[5];                                                     // logger_id
  double parms[20];                                               // all the msmt values
  int parm_cnt          = 0;                                      // number of msmts
  int valid             = 0;                                      // this structure has been loaded
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


void setup()
{
  mayflyCard.setup(&beeModule[0]);                                // setup the Mayfly Card with a BeeModule
  
  mayflyCard.redLED.turnOn();                                     // turn on the Red LED
  mayflyCard.greenLED.turnOn();                                   // turn on the Green LED
  delay(1000);                                                    // wait a second
  mayflyCard.redLED.turnOff();                                    // turn off the red LED

  // get the console information
  const int byteLimit = 6000;                                     // return full console records, not to exceed this total number of bytes
  char consoleRecords[byteLimit];                                 // return the console records into this array
  char startDate[] = "2019-09-08 12:00:00";                       // return console records starting at this dateTime
  char endDate[] = "2019-09-08 12:30:00";                         // return console records ending at this dateTime
  int numberOfConsoleBytes = mayflyCard.sdCard.getConsoleRecords(&consoleRecords[0], startDate, endDate, byteLimit);

  // get the directory from the SD Card
  const int sdCardDirectoryLimit = 10;                            // limit the number of directory names + file names to be displayed
  SdCardDirectory sd_card_directory[sdCardDirectoryLimit];        // define an array to hold the SD Card directory results
  int numberOfEntries = mayflyCard.sdCard.TIA_dir(&sd_card_directory[0], sdCardDirectoryLimit);     // get the SD Card directory & file names




  SerialMon.println(F("")); SerialMon.println(F("===== Starting Sketch Output ====="));
  
  /***** this code displays the console records *****/
  /*                                                */
  SerialMon.println(F("")); SerialMon.println(F("<<< CONSOLE RECORDS >>>"));
  SerialMon.print(numberOfConsoleBytes); SerialMon.println(F(" bytes returned:"));
  SerialMon.println(consoleRecords);


  /***** this code displays the directory information *****/
  /*                                                      */
  // process each file
  for (int i=0; i < numberOfEntries; i++) {

    if (sd_card_directory[i].directoryFlag) SerialMon.println(F(""));                               // print a blank line before a directory entry

    for (int j=0; j<sd_card_directory[i].folderLevel; j++) { SerialMon.print(F("\t")); }            // add tabs to indent sub-directory level

    // if this is a directory entry
    if (sd_card_directory[i].directoryFlag) {
      SerialMon.print(F("<<< ")); SerialMon.print(sd_card_directory[i].filename); SerialMon.println(F(" >>>"));
    }

    // otherwise, this is a file
    else {
      SerialMon.print(sd_card_directory[i].filename);     SerialMon.print(F("\t"));
      if (strlen(sd_card_directory[i].filename) <= 12)    SerialMon.print(F("\t"));
      SerialMon.print(sd_card_directory[i].modDateTime);  SerialMon.print(F("\t"));
      SerialMon.print(sd_card_directory[i].sizeKb);       SerialMon.println(F(" KB"));
    }

    // if we've reached the limit of directory+file names
    if(sd_card_directory[i].limitReached) {
      SerialMon.println(F(""));SerialMon.print(F("=== More files may exist.  Maximum display limit of ")); SerialMon.print(sdCardDirectoryLimit); SerialMon.println(F(" reached. ==="));
      break;
    }
  }
}


void loop() {
  delay(500);
  mayflyCard.greenLED.switchState();
}
