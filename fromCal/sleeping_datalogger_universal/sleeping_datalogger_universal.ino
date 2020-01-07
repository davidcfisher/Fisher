//This sketch puts the Mayfly board into sleep mode.  It wakes up at specific times, records the pressures, temperatures
//and battery voltage onto the microSD card, prints the data string to the serial port, and goes back to sleep.
// supports XBee communication
// supports BOTH ott and cs451 - but you have to declare the sdi12_type
// 		also have to set the sdi12_conversion to "psi_to_feet" or "none"
// 		both Ott and CS451 use 12v power and digital pin 7 for communication
// 		reads 12v * 1/11 on analog pin A0
// supports culverts - 2 MS5803 pressure transducers - 1 in the box and 1 in the water

// carrier will probably be set to CP0 (let modelm/SIM select AT&T or Verizon)
// 		you can change Carrier between Verizon and AT&T via the mayfly_command.pl
// 		just issue :  	CP3,AC,WR,FR for Verizon or 	CP2,AC,WR,FR for AT&T

// you can change the modem-Mayfly serialAT Baud rate between 9600 and 19200 via the mayfly_command.pl
// 		just issue: BD3 for 9600 or BD4 for 19200 and the Mayfly will adjust


// includes  ----------------------------------------------------------------------------------------------------------------------
//#include <SDI12_PCINT3.h> // SDI 12 modified to play nice with PCInt - latest
#include <SDI12_Mod.h> // SDI 12 modified to play nice with PCInt - this module creates warning in compile for util/parity.h.  I don't know if an updated version exists/works.
//#include <SDI12.h> // SDI 12 
#include <Wire.h>       // I2C devices 
#include <avr/sleep.h>  // Sleep - micro controller support
#include <avr/wdt.h>    // Watch Dog Timer
#include <SPI.h>        // Serial Peripheral Devices (
#include <SdFat.h>      // SD Card
#include <RTCTimer.h>   // Scheduler using Real Time Clock
#include <Sodaq_DS3231.h> // RTC Chip
//#include <Sodaq_PcInt_PCINT0.h>  // Pin Change Interrupt
#include <Sodaq_PcInt_Mod.h>  // Pin Change Interrupt
//#include <Sodaq_PcInt.h>  // Pin Change Interrupt
#include <MS5803.h>    // MS5803 support
#include <MemoryFree.h> // for memory command to see how much memory is left

// defines ----------------------------------------------------------------------------------------------------------------------
#define TINY_GSM_MODEM_XBEE  		// Select for Digi brand WiFi or Cellular XBee's
//#define TINY_GSM_RX_BUFFER 1024  	// internal receive buffer size - I can't see where this is used anywhere
#define SerialMon Serial  			// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialAT Serial1 			// Set serial for AT commands (to the module)
#define SD_CS_PIN SS				// I2C for SD Card Reader ??
#define SD_SS_PIN 12				// SD Card
#define POWER_PIN -1       	 		//Don't try this .FAILS - The I2C power pin (22) for controlling sensor power drain -1 if disabled - doesn't work right
#define RTC_PIN A7					//RTC Interrupt pin
#define RTC_INT_PERIOD EveryMinute
#define V12_FACTOR 0.0361			// multiply this * A0 pin reading from 10:1 resistor divider to get 12v battery voltage.
// This may have to be tuned if the resistors aren't accurate.  Ideally, the formula would be voltage = (ATDRreading * 11. * 3.3) / 1024.   ;
#define DATA_PIN 7         			// The pin for reading the SDI-12 data wire
SDI12 mySDI12(DATA_PIN);			// Setup SDI12
/*
  '?' is a wildcard character which asks any and all sensors to respond - doesn't work with CS451. Assume address 0
  'I' indicates that the command wants information about the sensor
  '!' finishes the command
  
  OTT - Glen Lake and Crystal
  '0M!' asks for pressure, temperature in units defined in configuration for sensor with address 0
  '0XCONFIG=tt,pp,mmm.mm,ooo.oo!' sets the configuration
  tt: temperature units, 0=C 1=F
  pp: msmt units, 0 = psig 1 = kPa 2 = Bar 3 = feet 4 = meter 5 = inch 6 = millimeter
  mmm.mm = multiplier to pressure
  ooo.oo = offset to pressure
  so: 'XCONFIG=1,4,1,0!' should give me FEET and FAHRENHEIT
  
  CS451 - Leelanau
  '0M2!' asks for pressure & temp in (PSIG pounds per square inch gage)and F°
  you can convert PSIG to Feet by multiplying by 2.3059
  The designation "gauge" indicates the readings are already adjusted or biased to ignore the surrounding atmospheric pressure which is 14.7 psi at sea level.
  */

// deployment specific variables ----------------------------------------------------------------------------------------------------------------------
// items marked with "SSET" (Server Set) mean they can be set/changed/reviewed over the internet by the server
char logger_name[5] = "F" ; 					// SSET:  2 char limit.  A or B or z6 ... what's printed on cardreader. contained in each msmt line
int sense_period = 15 ;							  // SSET:  Minutes between reading - should be evenly divisible into 60

// make sure you set mayfly_server to the same type of sensors and tables

// Glen Lake Narrows id:L
/*
char location[100] = "Glen Lake Narrows" ;		// SSET: Leelanau Narrows, Crystal, GLWLC Narrows, GLWLC Dam, Culvert3 Upstream or Downstream
char msmt_file_name[50] = "glwlc_narrows.csv" ;	// SSET: File for the msmts
char msmt_type[10] = "sdi12" ;					// get msmts from OTT or CS451 sensors
char sdi12_msmt_cmd[5] = "0M!" ;   				// command for OTT	for Glen Lake and Crystal
double sdi12_conversion = 1 ;					// multiply by 1 (no conversion) for Glen Lake and Crystal
#define DATA_HEADER "dateTime,logger,temp_mayfly,voltage_12v,depth,temp_water,voltage_lipo" // for sdi12
*/

// Glen Lake Dam - id:M
/*
char location[100] = "Glen Lake Dam" ;		// SSET: Leelanau Narrows, Crystal, GLWLC Narrows, GLWLC Dam, Culvert3 Upstream or Downstream
char msmt_file_name[50] = "glwlc_dam.csv" ;	// SSET: File for the msmts
char msmt_type[10] = "sdi12" ;					// get msmts from OTT or CS451 sensors
char sdi12_msmt_cmd[5] = "0M!" ;   				// command for OTT	for Glen Lake and Crystal
double sdi12_conversion = 1 ;					// multiply by 1 (no conversion) for Glen Lake and Crystal
#define DATA_HEADER "dateTime,logger,temp_mayfly,voltage_12v,depth,temp_water,voltage_lipo" // for sdi12
*/

// Leelanau
/*
char location[100] = "Leelanau Narrows" ;		// SSET: Leelanau Narrows, Crystal, GLWLC Narrows, GLWLC Dam, Culvert3 Upstream or Downstream
char msmt_file_name[50] = "leelanau_narrows.csv" ;	 	// SSET: File for the msmts
char msmt_type[10] = "sdi12" ;					// get msmts from OTT or CS451 sensors
char sdi12_msmt_cmd[5] = "0M2!" ;   			// command for CS451 for Leelanau
double sdi12_conversion = 2.307 ;				// multiply by 2.307 to convert psi_to_feet for Leelanau.
#define DATA_HEADER "dateTime,logger,temp_mayfly,voltage_12v,depth,temp_water,voltage_lipo" // for sdi12
*/

// Crystal
/*
char location[100] = "Crystal" ;				// SSET: Leelanau Narrows, Crystal, GLWLC Narrows, GLWLC Dam, Culvert3 Upstream or Downstream
char msmt_file_name[50] = "cyrstal.csv" ;	 	// SSET: File for the msmts
char msmt_type[10] = "sdi12" ;					// get msmts from OTT or CS451 sensors
char sdi12_msmt_cmd[5] = "0M!" ;   				// command for OTT	for Glen Lake and Crystal
double sdi12_conversion = 1 ;					// multiply by 1 (no conversion) for Glen Lake and Crystal
#define DATA_HEADER "dateTime,logger,temp_mayfly,voltage_12v,depth,temp_water,voltage_lipo" // for sdi12
*/

// Culverts Upstream
/*
char location[100] = "Culvert3" ;				// SSET: Leelanau Narrows, Crystal, GLWLC Narrows, GLWLC Dam, Culvert3 Upstream or Downstream
char msmt_file_name[50] = "culvt_up.csv" ;	 	// SSET: File for the msmts
char msmt_type[10] = "ms5803" ;					// get msmts from MS5803
char sdi12_msmt_cmd[5] = "0M1" ;   				// doesn't matter for culverts
double sdi12_conversion = 1 ;					// doesn't matter if type is culvert
#define DATA_HEADER "dateTime,logger,temp_mayfly,voltage,pressure_water,temp_water,Pressure_air,temp_air,voltage_lipo" // for culverts
*/

///*
// Culverts Downstream
char location[100] = "Culvert3" ;				// SSET: Leelanau Narrows, Crystal, GLWLC Narrows, GLWLC Dam, Culvert3 Upstream or Downstream
char msmt_file_name[50] = "culvt_dn.csv" ;	 	// SSET: File for the msmts
char msmt_type[10] = "ms5803" ;					// get msmts from MS5803
char sdi12_msmt_cmd[5] = "0M!" ;   				// doesn't matter for culverts
double sdi12_conversion = 1 ;					// doesn't matter if type is culvert
#define DATA_HEADER "dateTime,logger,temp_mayfly,voltage,pressure_water,temp_water,Pressure_air,temp_air,voltage_lipo" // for culverts
//*/

// choose 1 of 3								// don't set carrier to 1, "no profile".  It doesn't work
char carrier[2] = "0";							// Autodetect for all
//char carrier[2] = "2";            			// AT&T - preferable at Crystal Lake 
//char carrier[2] = "3";						// Verizon - preferable at Leelanau Narrows
												
// common variables
int read_buffer_size = 1024 ;					// SSET: read buffer initially 1024 but can be set by server command.  See function: "read_incoming_data"
char * read_buffer; 							// malloc in setup.  read buffer initially 1024 but can be set by server command.  
char host[100] = "132.148.85.45" ; 				// SSET: "lake-man.com" or "132.148.85.45" so we don't have to do look-ups
char resource[100] = "/scripts/mayfly_server.pl"; // SSET: the server script on the host
char parms[200] = "asdf=qwer" ;        // the parameters I send to the server - only used for http_get
char access_point_name[20] = "hologram" ; 		// SSET:  or whatever your apn is for your SIM card
int num_batches = 10 ;							// SSET: max number of batches to send in one server connection (don't want the small battery to drain transmitting for a long time?)
long int batchsize = 6000 ; 					// SSET: max size of msmt bytes to send. each day is about 5760 bytes 24*4*60 if uncompressed, or about 1/3 that if compressed
char * send_buf ; // malloc in setup
float min_xbee_voltage = 3.5 ;					// SSET:  don't wake XBee up if less than this. Documentation suggests 3.3 - 4.3v
// wake_bee event times
int wake_bee_hour[24] ;
int wake_bee_min[24] ;
int enable_modem = 1 ; 							// enable the xbee.  If disabled  (=0) then msmts can be recorded but the modem never turns on and Dialog is never entered.
char wake_bee_event_string[200] = "0:0;1:0;2:0;3:0;4:0;5:0;6:0;7:0;8:0;9:0;10:0;11:0;12:0;13:0;14:0;15:0;16:0;17:0;18:0;19:0;20:0;21:0;22:0;23:0" ; // SSET: 
int num_wakes = 0 ; 							// number of wake times. gets computed on boot or when event_string is set by server
int compression = 1 ;							// SSET: if = 1, will compress transmissions of msmts
int retry_cnt_1 = 0 ; 							// number of times we've done the 1st pass retry - sent in "status"
int retry_cnt_2 = 0 ; 							// number of times we've done the 2nd pass retry - sent in "status"
String email_txt = "" ;							// In dialog, if email_txt is not empty, it will send text to server to send to Cal via email.  Various things/events/failures add to email_txt
char server_lastdate[20] = "" ;					// last msmt record date that was sent & confirmed by server.  Blank if not known

struct msmt_line {								// used in compression. holds the essential data of a msmt line
  long seconds; 		// epoch time
  char id[5];			// logger_id
  double parms[20];	// all the msmt values
  int parm_cnt = 0;	// number of msmts
  int valid = 0 ;		// this structure has been loaded
  long last_delta ;	// the number of seconds between this msmt and the previous msmt
} ;
struct msmt_line m1 ;							// previous values of msmt line in buffer to be sent ( compressed or not)
struct msmt_line m2 ;							// next msmt line to be compared to previous line for compression (delta modulation)

// date stuff
int set_clock = 0 ;             // > 0 to set the clock
// Load a 2nd time with set_clock = 0 so that a battery swap won't reset the clock
// Sun=1 Mon=2 Tue=3 Wed=4 Thu=5 Fri=6 Sat=7
//year, month, day, hour, min, sec and week-day(starts from 0 and goes to 6)
//writing any non-existent time-data or week-day screws up theRTC.
DateTime dt(2019, 1, 8, 16, 58, 0, 2);	// no leading zeroes    it takes about 30 seconds to load so enter time 30 seconds into future
int DST ; //daylight saving time
int currentyear, currentmonth, currentday, currenthour, currentminute, currentsecond, currentdow ; // day of week mon=1 .... sun=7
long currentepochtime ;

float boardtemp;
//int batteryPin = A6;    // select the input pin for the potentiometer
//int batterysenseValue = 0;  // variable to store the value coming from the sensor
//float batteryvoltage;
float temp_water ;
double pressure_water ;
float temp_air ;
double pressure_air ;

// object initialization----------------------------------------------------------------------------------------------------------------------
MS5803 sensor_air ;		// Create the MS5803 sensor objects
MS5803 sensor_water ;
SdFat SD;				// SD card
RTCTimer timer;			// Real Time Clock timer

#include <TinyGsmClient.h>// included below. This include must be included below the define of the modem name!
TinyGsm modem(SerialAT); // comment this line
TinyGsmClient client(modem);
#include <xBee_TIA-Software.h> // TIA-Software XBEE library - must be included AFTER the variable definitions
//#include <Mayfly_EEPROM_Class_TIA-Software.h>

// setup ----------------------------------------------------------------------------------------------------------------------

void setup() {
	
	logAndPrint(F("Mayfly just booted")) ;

  // !!!!!!!!!!!!!  CAL - LOOK AT THIS
  //rtc.setEpoch(uint32_t (1552182900L)); // sets clock to 2019-3-10 right before DST

  // initialize the variables from EEProm
  // Eeprom_Class eeprom;// create an EEPROM instance
  // eeprom.initializeProvisioningVariables() ;

  // 2 Malloc's for variables - read_buffer and send_buf
  read_buffer = (char *)malloc(read_buffer_size ); // read buffer initially 1024 but can be set by server command.  It won't grow automatically in case someone sends junk to this Mayfly

  send_buf = (char *)malloc(batchsize ); // get memory for the parms and the data and for a work area
  if (!send_buf ) {
    logAndPrint("Could not get storage for send_buf. Hanging....") ;
    while (1) {}
  }

  // set the clock IF required
  if (set_clock > 0) {
    Wire.begin();
    rtc.begin();
    rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above
    SerialMon.println("Setting Clock") ;
  }

  //Initialize the serial connection
  SerialMon.begin(57600);   // communication to/from Mayfly over USB
  //SerialAT.begin(9600) ;  // Baud Rate is determined in setup_xbee.  Legal values 9600 and 19200. 19200 may make it easier to attach battery without hanging the modem
  rtc.begin();
  delay(100);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  greenred4flash();    //blink the LEDs to show the board is on
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

  SerialMon.print("read_buffer_size") ;		SerialMon.println(read_buffer_size) ;
  SerialMon.print("host ") ;					SerialMon.println(host ) ;
  SerialMon.print("resource ") ;				SerialMon.println(resource ) ;
  SerialMon.print("access_point_name ") ;		SerialMon.println(access_point_name ) ;
  SerialMon.print("batchsize ") ;				SerialMon.println(batchsize) ;
  SerialMon.print("num_batches ") ;			SerialMon.println(num_batches) ;
  SerialMon.print("wake_bee_event_string ");	SerialMon.println(wake_bee_event_string ) ;
  SerialMon.print("min_xbee_voltage ") ;		SerialMon.println(min_xbee_voltage ) ;
  SerialMon.print("sense_period ") ;			SerialMon.println(sense_period ) ;
  SerialMon.print("location " ) ;				SerialMon.println(location ) ;
  SerialMon.print("msmt_file_name ") ;		SerialMon.println(msmt_file_name ) ;
  SerialMon.print("logger_name ") ;  			SerialMon.println(logger_name ) ;

  email_txt = "Mayfly booted up: " ;
  email_txt += getDateTime() ;
  
  logAndPrint(F("Mayfly booted")) ;
}

// loop   ----------------------------------------------------------------------------------------------------------------------
void loop() {
  //Update the timer

  timer.update(); // This updates all the currentxxx variables to now: year, month, day, hour, minute, second, dow, dst

  //Serial.print(currentyear) ; Serial.print("-") ; Serial.print(currentmonth) ;Serial.print("-") ; Serial.print(currentday) ;Serial.print(" ") ;
  //Serial.print(currenthour) ;Serial.print(":") ; Serial.print(currentminute) ;Serial.print(":") ; Serial.print(currentsecond) ;
  // Serial.print("  DOW:") ; Serial.print(currentdow) ;Serial.print(" DST: ") ; Serial.print(DST) ; Serial.print(" Epoch: ")  ; Serial.println(currentepochtime) ;

  //check_dst_change() ; // adjust the clock for DST change - don't need to do this since we have DT. It also causes a problem conversation on Fall

  // measurement wake-up
  if (currentminute % sense_period == 0 ) { //  sense_period = 2 or 5 or 10 or 20 or 30 ... to wake up. 1 wakes up each minute
	if ( currentsecond < 2) { 	// I'm checking for seconds<2 because the SDI12 sensor can give me spurious interrupts that wake me up at any time.
	
		logAndPrint(F("Starting msmt")) ;
	
		// Power the sensors;
		if (POWER_PIN > 0) {
		  SerialMon.println("Powering up sensors...");
		  digitalWrite(POWER_PIN, HIGH);
		  delay(200); // give time for sensors to wake up???
		}
		
		// build the msmt record
		//datetime, logger_id, temp_mayfly_C, voltage_12v, depth_Ft, temp_water_F, voltage_lipo
	
		//Date
		String data = getDateTime();
		data += ",";	

		// Logger
		data += logger_name ;
		data += "," ;
		//SerialMon.println(data) ;
	
		//Board Temp C
		rtc.convertTemperature();          //convert current temperature into registers
		boardtemp = rtc.getTemperature(); //Read temperature sensor value
		//SerialMon.println(boardtemp) ;
		addFloatToString(data, boardtemp, 3, 1);
		data += ",";
		
		// Voltage - 12v
		addFloatToString(data, get_12v_voltage(), 4, 2);	
		data += ",";
		
		if (0==strcmp(msmt_type,"sdi12")) { // OTT or CS451 Sensors
			// Get data from SDI-12 water level sensor - depth_Ft AND temp_c
			data += getSDI12Data() ;
			data += ",";
		}
		
		if (0==strcmp(msmt_type,"ms5803")) { // ms5803 sensors		
			//MS5803 code for sensor in water
			// Read pressure from the sensor in mbar.
			pressure_water = sensor_water.getPressure(ADC_4096);  // 4096 is the highest precision
			//Serial.print("pressure_water = ");
			//Serial.println(pressure_water); 
			data += pressure_water ;
			data += ","; 
	   
			// Read temperature from the sensor in deg C.
			temp_water = sensor_water.getTemperature(CELSIUS, ADC_4096);
			//Serial.print("temp_water = ");
			//Serial.println(temp_water);	
			addFloatToString(data, temp_water, 4, 2);
			data += ",";  	
			
			// MS5803 code for sensor in box
			// Read pressure from the sensor in mbar.
			pressure_air = sensor_air.getPressure(ADC_4096);   
			//Serial.print("pressure_water = ");
			//Serial.println(pressure_water); 
			data += pressure_air ;
			data += ","; 
	   
			// Read temperature from the sensor in deg C.
			temp_air = sensor_air.getTemperature(CELSIUS, ADC_4096);
			//Serial.print("temp_water = ");
			//Serial.println(temp_water);	
			addFloatToString(data, temp_air, 4, 2);
			data += ",";
			// end MS5803
		}

		// Voltage Lipo - 3.7  - always the last msmt
		addFloatToString(data, get_voltage(), 4, 2);
		//data += ",";
				
		//Save the data record to the log file
		logData(data);
	
		//Echo the data to the serial connection
		//SerialMon.print("Data Record: ");
		SerialMon.println(data);
		data = "";
	
		//digitalWrite(8, LOW);// turn off LED to show I'm done capturing data
	
		// Power down sensors;
		if (POWER_PIN > 0) {
		  SerialMon.println("Powering down sensors...");
		  digitalWrite(POWER_PIN, LOW);
		}
		logAndPrint(F("End msmt")) ;
	} // end seconds<2
	else {
		SerialMon.println(F("Spurious Interrupt")) ; // this can be caused from SDI12 sensor if you don't have the right SODAQ_PCINT_MOD and SDI12_Mod libraries

// This appears to reset the sensor - same as sending "break" signal which is just 1's.
// or it simply clears the buffer.  I don't know
// it seems to get the sensor out of the middle of a msmt 

		for (int i=0; i<20; i++ ){
			char a = mySDI12.read() ;
			Serial.print(a) ;
		}
		
		delay(900) ; // don't delay too long or you could miss a real interrupt
	   // see https://www.envirodiy.org/topic/low-power-project/ #1880
	}
  } // end measurement wake-up period

  // xBee wake-up
  if (enable_modem || (currenthour == 12 && currentminute == 0)) { // If XBee disabled (=0) then msmts can be recorded but the modem never turns on and Dialog is never entered.
																	// an exception is to try to communicate at noon even if disabled				
	
	for (int i = 0; i < num_wakes; i++) {//if hour & minute == wake-up time in the array/list OR the button is pushed, then wake up the xbee and start a dialogue with server
	  if ((currenthour == wake_bee_hour[i] && currentminute == wake_bee_min[i] )
		  || digitalRead(21)) { // you can push the button to force a dialog with the server. push for 1 full minute
		// check voltage
		float v = get_voltage() ; // get the battery voltage
		if (v >= min_xbee_voltage) {
		  SerialMon.println("Waking xBee") ;
		  digitalWrite(23, LOW);  // this should wake the modem
		  if (digitalRead(21)) SerialMon.println("Let go of the button") ;
		  while (digitalRead(21)) {} // hang until button unpressed.
		  
		  // dialog
		  dialog() ;	// won't return until there is no more to do.
		  
		  // shuddown modem.  I don't think this action helps the tower or the modem and it delays the sleep mode by 35 seconds. But the Digi manual says to do it
		  modem.commandMode() ; // make sure in command mode. 
		  modem.sendAT("SD");    // send shut down to disconnect from tower
		  String at_out = "Shutdown: " ;
		  at_out += modem.readResponseString(120000);  // it can take as long as 2 minutes to shutdown - normal around 35 seconds
		  Serial.println(at_out) ;
		  
		}
		else {
		  String x = F("Voltage: ") ;
		  x += v;
		  x += F(" is too low to wake. Skipping this wake time.") ;
		 logAndPrint(x) ;
		}
		break ;
	  }
	}
  }

  //Sleep
  digitalWrite(23, HIGH);  // this should sleep the modem
  systemSleep();  // must hang here until next minute

} // end LOOP

void showTime(uint32_t ts) {
  //Retrieve and display the current date/time
  String dateTime = getDateTime();
  //SerialMon.println(dateTime);
}

void setupTimer() {
  //Schedule the wakeup every minute
  timer.every(1, showTime);

  //Instruct the RTCTimer how to get the current time reading
  timer.setNowCallback(getNow);
}

void wakeISR() {
  //Leave this blank
}

void setupSleep() {
  pinMode(RTC_PIN, INPUT_PULLUP);
  PcInt::attachInterrupt(RTC_PIN, wakeISR);

  //Setup the RTC in interrupt mode
  rtc.enableInterrupts(RTC_INT_PERIOD);

  //Set the sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void systemSleep() {
  //This method handles any sensor specific sleep setup
  sensorsSleep();

  //Wait until the serial ports have finished transmitting
  SerialMon.flush();
  Serial1.flush();

  //The next timed interrupt will not be sent until this is cleared
  rtc.clearINTStatus();

  //Disable ADC
  ADCSRA &= ~_BV(ADEN);

  //Sleep time
  noInterrupts(); // noInterrupts() globally disables interrupts (of all types)
  sleep_enable();
  interrupts();
  sleep_cpu();
  sleep_disable();

  //Enbale ADC
  ADCSRA |= _BV(ADEN);

  //This method handles any sensor specific wake setup
  sensorsWake();
}

void sensorsSleep() {
  //Add any code which your sensors require before sleep
  //SerialMon.println("Sleep the Sensors") ;
  // turn off the pull-up resister.  Does not turn off power to device
  //Wire.end() ; // starting and ending this doesn't do anything to current draw
}

void sensorsWake() {
  //Add any code which your sensors require after waking
  //SerialMon.println("Wake the Sensors") ;
  // turn on pin pull-up resistor.
  //Wire.begin() ;// starting and ending this doesn't do anything to current draw
}

void greenred4flash() {
  for (int i = 1; i <= 4; i++) {
    digitalWrite(8, HIGH);
    digitalWrite(9, LOW);
    delay(50);
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);
    delay(50);
  }
  digitalWrite(9, LOW);
}


void setupLogFile() {		//Initialize the SD card
  int begin_rc = SD.begin(SD_SS_PIN) ;
  if (!begin_rc)	{
    SerialMon.print("Error: SD card failed to initialize or is missing. RC: ");
    SerialMon.println(begin_rc) ;
  }
  else {
    chk_file_exists() ;
  }
}

void chk_file_exists() {
  bool oldFile = SD.exists(msmt_file_name);        //Check if the file already exists
  if (!oldFile) {                 //Add header information if the file did not already exist
    File logFile = SD.open(msmt_file_name, FILE_WRITE);  //Open the file in write mode
    logFile.print("Logger Name: ") ;
    logFile.print(logger_name) ;
    logFile.print("  Location: ") ;
    logFile.println(location) ;
    logFile.println(DATA_HEADER);
    setFileTimestamp(logFile, T_CREATE);
    logFile.close();  //Close the file to save it
  }
}

void logData(String rec) {
  //pinMode(8, OUTPUT); // to show LED when capturing data

  //Initialize the SD card again
  // Cal added this.  Initialize the SD card again because somebody could have taken out or replaced the card
  // Trying to write to a card that isn't there will hang the IO system and it won't try to write again.
  //setupLogFile();  // perform everything that setupLogFile does but no serial.print - this won't work because it keeps printing an error
  SD.begin(SD_SS_PIN) ;    // simply doing this will allow the SD card processor to recover - but the Return Code is always bad (0)
  chk_file_exists() ;  // user could have inserted a blank SD Card, so create the file and write the header

  //Re-open the file
  File logFile = SD.open(msmt_file_name, FILE_WRITE);

  //Write the CSV data
  logFile.println(rec);
  setFileTimestamp(logFile, T_WRITE);
  logFile.close();  //Close the file to save it
}

static void addFloatToString(String & str, float val, char width, unsigned char precision) {
  char buffer[10];
  dtostrf(val, width, precision, buffer);
  str += buffer;
}

// moved to xbee_tia-software.h
//void setFileTimestamp(File fileToStamp, uint8_t stampFlag) {
//  DateTime now = rtc.now();
//  fileToStamp.timestamp(stampFlag, now.year(), now.month(), now.date(), now.hour(), now.minute(), now.second()) ;
//}

// function to send command to SDI12 sensor
void sdi12_cmd(const char * cmd) {
	Serial.print("->") ;
	Serial.print(cmd) ;
	Serial.print(" <- ") ;
	mySDI12.sendCommand(cmd); 
	delay(300);
	unsigned long timeout = millis();
	while(mySDI12.available() && millis() - timeout < 5000L){    // received characters 
		char x = mySDI12.read() ; // read returns an integer, but the data(string) needs a character
		Serial.print(x) ;
	}
	Serial.println("") ;
}

//////////////////////////////////////////
// function to get the values from the water level sensor
// read documents in Notes on SDI-12.docx
String getSDI12Data() {

//sdi12_cmd("0OSU2!") ; // set feet // read level/pressure msmt units
//sdi12_cmd("0OST1!")  ; // set temp units to F
//sdi12_cmd ("0OXG!") ;// read gravity
//sdi12_cmd("0OXR!") ;// read water density
//sdi12_cmd("0OAA!") ;// read depth msmt mode
//sdi12_cmd("0OAB!") ;// read offset

// Crystal
// the OTT sensor at Crystal was set by me to read FEET and Fahrenheit using the commands "0OSU2!" and "0OST1!"
// the wiring is: Gray-signal, Blue-ground, Red-12v
//

// Leelanau
// the narrows CS451 sensor at Leelanau reads psi, so the sensor value needs to be multiplied by psi_to_feet
// the wiring is: White-signal, Black-ground, Red-12v

  logAndPrint(sdi12_msmt_cmd) ;
  mySDI12.sendCommand(sdi12_msmt_cmd);     // Start a measurement - gets default/configured units hopefully set above to feet and Fahrenheit 
  delay(300);                     // wait a while for a response
// Serial.println("receiving time to wait");
  int time = 0 ;
  char values ;
  // get the time to wait // should be about 3 seconds for 2 values e.g. These character bytes are sent: 00032 
  // converts the chars to numbers (character '0' is byte x'30' or decimal 48
  for (int i=0; i<4; i++) {
	int asdf = mySDI12.read() ;
	time = 10 * time + asdf - 48 ;
  }
  values = mySDI12.read() ;
  String response = "<- time: " ;  response += time ;  response += " values: " ;  response += values ;  logAndPrint(response) ; // putting this here messes up the timing
  //Serial.print("time: ") ; Serial.println(time) ; Serial.print("values: ") ; Serial.println(values) ;
  if (time > 10 || values != '2' || time <0) { // This can't be.  The sensor must not be responding...maybe because it hasn't warmed up yet.  Just return 0,0 and skip this sense period
	//time=3 ; // tried to give it 3 seconds and go on, but got crazy interrupts seconds or minutes later.
	//values=2;
	logAndPrint(F("Bad response from sensor. Returning 0,0")) ;
	//email_txt += F("\n Bad response from sensor at ") ; 	email_txt += String(currenthour) + ":" + String(currentminute) ; 
	mySDI12.flush(); // don't know if this helps clear out any responses from the sensor that will cause an interrupt or further garbage in.
	//for (int i=0; i<20; i++) {    // Tried this but it doesn't help. get rid of any junk coming in.  Is there some way to flush this?  
	//	mySDI12.read() ;
	//}
	return String ("0,0") ;
  }
  delay(time*1000);                     // wait "time" seconds for a response ( seconds * 1000 ) - probably 3
  response = "<- " ;
  unsigned long timeout = millis();
  while(mySDI12.available() && millis() - timeout < 5000L){    // wait for '\r\n0\r\n' ready to send
	response += mySDI12.read() ;  // should be 3 characters: \n0\n 
  }
// Serial.println(" ") ;
  delay(300);                     // wait a while 
  logAndPrint(response) ; // \r\n0\r\n' will be 1310481310
  logAndPrint(F("-> 0D0!")) ;
  // tell sensor to send data
  mySDI12.sendCommand("0D0!");     // Send the data
  delay(300);                     // wait a while for a response
// Serial.println("receiving data");
  int seq = 0 ;
  //String psi = "" ;
  String value = "" ;
  String temp = "" ;
  String all = "" ;
  timeout = millis();
  while(mySDI12.available() && millis() - timeout < 5000L){    // received characters should be 0SnnnnnnnnSmmmmmmmm where S= '+' or '-', nnn is psi and mmm is temp
    char x = mySDI12.read() ; // read returns an integer, but the data(string) needs a character
	all += x ;
	//Serial.println(all) ;
    if ( x == 10 || x == 13) { continue ;} // get rid of \r \n
    if (seq == 0 && (x == '-' || x == '+')) { // values start with a + or - sign, so look for "-" or '+' separator.
      seq = 1 ;
	  //value = x ;  // can skip this since water level can never be negative
    }
    else if (seq == 1) { //getting depth until '+' or '-' of next value
      if (x == '-' || x == '+') { // end of depth and start of temp
        seq = 2 ;
		temp = x ;
      }
      //else psi += x ; // add depth char
      else value += x ; // add depth char
    }
    else if (seq == 2) temp += x ;  // getting temp
	// else throw away this char probably 0 starting char
  }
  response = "<- Value: " ; response += value ; response += " tempF: " ; response += temp ; logAndPrint(response) ;
  double feet = sdi12_conversion * value.toDouble() ;  // convert psi to feet if needed
  String data = String(feet, 4);                  // using a float and only 4 decimal places since that's all I keep in the tables
  data += "," + String(temp.toDouble(),4) ;		// just get the 4 decimal places
  return data ;
} // end getSDI12Data
