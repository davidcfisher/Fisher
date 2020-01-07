// ==========================================================================
//    Send AT cmds right from serial monitor
//    Cal Killen - TIA Software - January 2019
//    Just enter AT commands via the monitor - you don't have to capitalize commands
// ==========================================================================

// includes
#include <SdFat.h>      // SD Card
#include <Sodaq_DS3231.h> // RTC Chip
#include <Sodaq_PcInt_Mod.h>  // Pin Change Interrupt
#include <MS5803.h>    // MS5803 support
#include <MemoryFree.h> // for memory command to see how much memory is left

// defines
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's
#define SerialMon Serial  			// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialAT Serial1  // Set serial for AT commands (to the module)
#define SD_CS_PIN SS
#define SD_SS_PIN 12				// SD Card
#define V12_FACTOR 0.0361  // multiply this * A0 pin reading from 10:1 resistor divider to get 12v battery voltage.

// variables ----------------------------------------------------------------------------------------------------------------------
char logger_name[5] = "X" ; 					//SSET:  2 char limit.  A or B or z6 ... what's printed on cardreader. contained in each msmt line
char location[100] = "Cal's Desk" ;			//SSET:  Culvert 3 up or down stream
int sense_period = 15 ;							//SSET:  Minutes between reading - should be evenly divisible into 60
char msmt_file_name[50] = "test.csv" ;		 	//SSET: file name can't be more than 8 chars!!!
char carrier[2] = "0";						// Autodetect 
//char carrier[2] = "2";            // AT&T - preferable at Crystal Lake 
//char carrier[2] = "3";							// Verizon - preferable at Leelanau Narrows
												// don't set carrier to 1, "no profile".  It doesn't work

int read_buffer_size = 1024 ;
char * read_buffer; // malloc in setup.  read buffer initially 1024 but can be set by server command.  It won't grow automatically in case someone sends junk to this Mayfly
 
char host[100] = "132.148.85.45" ; 				// or "lake-man.com" or "132.148.85.45" so we don't have to do look-ups
//char host[100] = "lake-man.com" ;

//char resource[100] = "/currence/mayfly_server.pl";
char resource[100] = "/scripts/mayfly_server.pl"; 
char parms[200] = "ex. asdf=qwer" ;				// the parameters I send to the server - only used for http_get
char access_point_name[20] = "hologram" ; 		// or whatever your apn is for your SIM card
int num_batches = 10 ; // max number of batches to send in 1 dialog
int batchsize = 6000 ; // max size of msmt bytes to send. each day is about 5760 bytes 24*4*60 uncompressed or about 1/3 that compressed
char * send_buf ; // malloc in setup

float min_xbee_voltage = 3.5 ;					// don't wake up if less than this
int enable_modem = 1 ; // enable the xbee.  If disabled  (=0) then msmts can be recorded but the modem never turns on and Dialog is never entered.
// wake_bee event times
int wake_bee_hour[24] ;
int wake_bee_min[24] ;
char wake_bee_event_string[200] = "6:01;7:23;9:33" ;
int num_wakes =0 ;
int compression = 1 ;
int retry_cnt_1 = 0 ; // number of times we've done the 1st pass retry
int retry_cnt_2 = 0 ; // number of times we've done the 2nd pass retry
String email_txt = "Mayfly TestTool booted up." ;							// any value here gets sent to the server which sends an email to Cal
struct msmt_line {
  void test_method(){	Serial.println("method in a structure") ; }
  long seconds;
  char id[5];
  double parms[20];
  int parm_cnt = 0;
  int valid = 0 ;
  long last_delta ;
} ;
struct msmt_line m1 ;
struct msmt_line m2 ;


char server_lastdate[20] = "" ;	// last msmt record date that was sent & confirmed by server.  Blank if not known
int DST ; //daylight saving time
int currentyear, currentmonth, currentday, currenthour, currentminute, currentsecond, currentdow ; // day of week mon=1 .... sun=7
long currentepochtime ;

#include <TinyGsmClient.h>// This include must be included below the define of the modem name!
// Use this if you want to spy on modem communication.  Uncomment next 3 lines, and comment the next
//#include <StreamDebugger.h>
//StreamDebugger debugger(SerialAT, SerialMon);
SdFat SD;				// SD card
//TinyGsm modem(debugger);
TinyGsm modem(SerialAT); // define the modem
TinyGsmClient client(modem);
#include <xBee_TIA-Software.h> // TIA-Software XBEE library - must be included AFTER the variable definitions

void setup()
{
  SerialMon.begin(57600);    // communication to/from Mayfly over USB
  //SerialAT.begin(9600) ;  // Baud Rate is determined in setup_xbee.  Legal values 9600 and 19200. 19200 may make it easier to attach battery without hanging the modem
  
  SerialMon.println("SerialAT must be 9600 or 19200 & NEWLINE must be selected on for line-ending at the bottom of the COM window") ;
  SerialMon.print(F("Logger Name: ")) ; SerialMon.println(logger_name) ;
  SerialMon.println("Enter \"HELP\" to see how to run AT commands etc.") ;

// Malloc's for variables
  read_buffer = (char *)malloc(read_buffer_size ); // read buffer initially 1024 but can be set by server command.  It won't grow automatically in case someone sends junk to this Mayfly
	if (!read_buffer ) {
		SerialMon.println("Could not get storage for read_buffer. Hanging....") ;
		while (1) {}
	}
	send_buf = (char *)malloc(batchsize ); // get memory for the parms and the data and for a work area
	if (!send_buf ) {
		SerialMon.println("Could not get storage for send_buf. Hanging....") ;
		while (1) {}
	}

  
  setup_xbee() ;  // this code gets the modem ready
} // end setup
  char buf[100] ; // input buffer - I think the max input is 64
  char bufsave[100] ; // save input buffer for repeating
void loop()
{
 
  // enter AT commands on the monitor.

  SerialMon.print(">") ; // show that I'm ready for input

  // process command entered from monitor
  int i = 0 ;
  while (!SerialMon.available()) {} // wait for something to by typed and sent
  while (SerialMon.available() > 0) { // it's ready
    delay(20) ; // don't know if this is needed
    buf[i] = SerialMon.read(); // Read a character
    i++ ;

    if (i > 66) {
      SerialMon.println("exceeded input buffer size. exiting...") ;
      return ;
    }
  }
  if (i>0) buf[i-1] = '\0' ; // terminate char string and overwrite the \n
  
  // Special Commands - LastCommand, SLEEP / WAKE the modem, GET http, VOLTAGE, connect, sethost, setresource, setparms, showurl
  if (0 == strcasecmp("lc", buf)) { // repeat last command
    strcpy(buf, bufsave) ;
  }
  else { // or save this new command
    strcpy(bufsave, buf) ;
  }
  // help text
  if (0 == strcasecmp("help", buf)) {
    SerialMon.println("Enter any 2 character AT command on the monitor.  Make sure  \"No line ending\" is set.") ;
    SerialMon.println("  some useful AT commands:\n  AM (airplane mode)\n  SM (sleep mode)\n  MY (current ip address)\n  DB (RSSI)\n  AI (connection status)\n  IM (IMEI)\n  S# (SIM card #)") ;
    SerialMon.println("To SET a VALUE for an AT command, enter the AT command + VALUE (with or without a space between).") ;
    SerialMon.println("  ex. SM1          (sets sleep mode to 1)\n      SM 1         (also sets sleep mode to 1)\n      AN hologram  (sets Access Point Name to \"hologram\"") ;
    SerialMon.println("Macro commands:\n  lc (repeats last command\n  sleep (sleeps the modem if SM is 1)\n  wake (wakes the modem)\n  ram (shows free memory)") ;
    SerialMon.println("  voltage (displays v)\n  connect (connects to network)\n  sethost xxx (sets host=xxx)\n  setresource xxx (sets resource=xxx)") ;
    SerialMon.println("  setparms xxx (sets paramaters=xxx)\n  showurl (shows the entire URL of host-resource-parms)\n  seturl (sets host,resource & pamrs all in one line\n  get (does an HTTP GET with current URL)") ;
    SerialMon.println("  post (does an HTTP POST with current URL)\n  sendmsmts (sends all new measurements from SD card to server)\n  sendserver xxxx (ex. sendserver status 2019-01-01 00:00:00,asdf,qwer)") ;
	  SerialMon.println("  autoclock (sets clock from tower -requires FW 11411 or later)") ;
	  SerialMon.println("  ALSO - you can push the button on the Mayfly for 10 seconds to get out of a CONNECT that's failing.");
    return;
  }
  // Special Commands
  // Power XBee down - not sleep - OFF
  if (0 == strcasecmp("poweroff", buf)) {
    SerialMon.println("Power Down XBee") ;
    digitalWrite(23, HIGH);  // LOW is not sleep, HIGH is sleep, make sure you set sleep mode SM: 0=never sleep, 1=sleep by pin 23
    return;
  }
   // Power XBee up - not wake - poweron
  if (0 == strcasecmp("poweron", buf)) {
    SerialMon.println("Power Up XBee") ;
    digitalWrite(23, LOW);  // LOW is not sleep, HIGH is sleep, make sure you set sleep mode SM: 0=never sleep, 1=sleep by pin 23
    return;
  }
  // Dialog
  if (0 == strcasecmp("dialog", buf)) {
    SerialMon.println("Dialog") ;
    dialog() ;
    return;
  }
  // Sleep
  if (0 == strcasecmp("sleep", buf)) {
    SerialMon.println("sleeping") ;
    digitalWrite(23, HIGH);  // for this to work, sm must be 1
    return;
  }
  if (0 == strcasecmp("wake", buf)) {
    SerialMon.println("waking") ;
    digitalWrite(23, LOW);  // this should wake the modem
    return;
  }
  if (0 == strcasecmp("voltage", buf)) {
    float v = get_voltage() ;
    SerialMon.print("Voltage: ") ;
    SerialMon.println(v) ;
    digitalWrite(23, LOW);  // this should wake the modem
    return;
  }
  // HTTP GET
  if (0 == strcasecmp("get", buf)) {
    SerialMon.println("GET") ;
    http_get(host, resource, parms,0) ;
    SerialMon.println("Data recieved:") ;
    SerialMon.println(read_buffer) ;
    SerialMon.println("End Data Received") ;
    return ;
  }
  // HTTP POST
  if (0 == strcasecmp("post", buf)) {
    SerialMon.println("POST") ;
    http_post(host, resource, parms,0) ;
    SerialMon.println("Data recieved:") ;
    SerialMon.println(read_buffer) ;
    SerialMon.println("End Data Received") ;
    return ;
  }
  // HTTP TEST
  if (0 == strcasecmp("test", buf)) {
    SerialMon.println("test") ;
    //http_post(host, resource, parms,0) ;
		// connect to network
	//	int i=1 ;
	//    while (i < 200) {  // writes a line of status every 10 seconds until it connects. Will try for 720 seconds or 12 minutes then returns 0
	//		modem.commandMode() ;  // make sure we are still in command mode
	//		SerialMon.print(i) ;
	//		modem.sendAT(GF("AI")); // registration status
	//		String at_response = modem.readResponseString(200);
	//		SerialMon.print(F(". AI: ")) ;
	//		SerialMon.print(at_response) ;
	//		modem.sendAT(GF("DB")); // Cell Strength
	//		//at_out = modem.readResponseString(200);
	//		SerialMon.print(F(", DB: ")) ;
	//		SerialMon.println(modem.readResponseString(200)) ;
	//		if (at_response == "0") {
	//			SerialMon.println(F("Connected to network")) ;
	//			break ;
	//		}
	//	}
		// connect to host
		IPAddress ip = TinyGsmIpFromString(String("132.148.85.45")) ;
        client.connect(ip, 80) ; // does a IP, DL, DE, WR, AC, CN.  the LA is skipped if we give an ip address.  This can't fail so don't check for failure
for (int j=0; j<10; j++) {
		// post
		
		Serial.print("post: "); Serial.println(j) ;
		client.print(F("POST "));
		client.print("/scripts/mayfly_server.pl") ;
		client.println(F(" HTTP/1.1")) ;
		client.print(F("Host: "));
		client.println("132.148.85.45") ;
		client.print(F("Content-Length: "));
		client.println(4); // length of data
		client.println(F("Connection: close"));
		client.println();
		client.println("qwer");
		
		// read response
		int cycles = 200 ;// Cal - probably should allow the server to set the number of cycles to wait or the timeframe
		i = 0;
		while (client.connected() && !client.available() && i < cycles) { // wait for 200 * 100 ms or 20 seconds for stuff to come back printing "."
			delay(100);
			SerialMon.print('.');
			i++ ;
		};
		
		// Read data including headers
		unsigned long timeout = millis();
		while (client.connected() && millis() - timeout < 5000L) {
			while (client.available()) {
				char c = client.read();
				SerialMon.print(c);
				timeout = millis();
			}
		}
}
    return ;
  } // end of test
  
  
  // Special Command do a connect to network then gets the IP of the URL
  if (0 == strcasecmp("connect", buf)) {
    SerialMon.println("CONNECT") ;
    int rc = connect_to_network() ;
	if (rc < 0)  return ;
    connect_to_host(host, 80) ;
    return ;
  }
  // Special Command sethost
  if (0 == strncasecmp("sethost", buf, 7)) {
    char *ps = buf + 7 ;
    if (0 == strncmp(" ", ps, 1))  ps++ ; // space after command

    char *pd = host ;
    strcpy(pd, ps);
    SerialMon.print("Set host to: ") ;
    SerialMon.println(host) ;
    return ;
  }
  // Special Command setresource
  if (0 == strncasecmp("setresource", buf, 11)) {
    char *ps = buf + 11 ;
    if (0 == strncmp(" ", ps, 1)) {
      ps++ ;   // space after command
    }
    char *pd = resource ;
    strcpy(pd, ps);
    SerialMon.print("Set resource to: ") ;
    SerialMon.println(resource) ;
    return ;
  }
  // Special Command setparms
  if (0 == strncasecmp("setparms", buf, 8)) {
    char *ps = buf + 8 ;
    if (0 == strncmp(" ", ps, 1)) {
      ps++ ;   // space after command
    }
    char *pd = parms ;
    strcpy(pd, ps);
    SerialMon.print("Set parms to: ") ;
    SerialMon.println(parms) ;
    return ;
  }
 // Special Command setname
  if (0 == strncasecmp("setname", buf, 7)) {
    char *ps = buf + 7 ;
    if (0 == strncmp(" ", ps, 1)) {
      ps++ ;   // space after command
    }
    char *pd = logger_name ;
    strcpy(pd, ps);
    SerialMon.print("Set logger_name to: ") ;
    SerialMon.println(logger_name) ;
    return ;
  }

  // Special Command seturl
  if (0 == strncasecmp("seturl", buf, 6)) {
    char *s = buf + 6 ;  //s = start
    char *e ;           //e = end
    int size ;
    if (0 == strncmp(" ", s, 1)) {
      s++ ;  // space after command
    }
    e = strchr(s, '/') ;// look for first "/" or end
    strcpy(host, s) ;  // copy entire buffer to host
    if (e == NULL) { // no resource or parms
      resource[0] = '\0' ;
      parms[0] = '\0' ;
    }
    else {
      host[e - s] = '\0' ;
      strcpy(resource, e ) ; // copy whats left in buff to resource
      s = e ; // start at resource and look for '?'
      e = strchr(s, '?') ;
      if (e == NULL) { // no parms
        parms[0] = '\0' ;
      }
      else {
        resource[e - s] = '\0' ;
        s = e + 1 ; //  skip the ?
        strcpy(parms, s) ;
      }
    }
    SerialMon.print("Set host= \"") ;
    SerialMon.print(host) ;
    SerialMon.print("\" resource= \"") ;
    SerialMon.print(resource) ;
    SerialMon.print("\" parms= \"") ;
    SerialMon.print(parms) ;
    SerialMon.println("\"" ) ;
    return ;
  }

  // Special Command showurl
  if (0 == strncasecmp("showurl", buf, 7)) {
    SerialMon.print("URL: ") ;
    SerialMon.print(host) ;
    SerialMon.print(resource) ;
    SerialMon.print("?") ;
    SerialMon.println(parms) ;
    return ;
  }

  // Special Command sendserver ( send a SERVER command to the mayfly server ex. sendserver status 2019-01-01 00:00:00,v,3.7 )
  if (0 == strncasecmp("sendserver", buf, 10)) {//if (0 == strncmp(" ", ps, 1))  ps++ ; // space after command
    SerialMon.print("sendserver ") ;
    char *cmd = strtok(buf + 11," ") ; // get the command after a space
	SerialMon.print(cmd); SerialMon.print(" ") ;
	char *data = strtok(NULL,"") ; // get the data after a space
    SerialMon.print(data) ; SerialMon.print(" ") ;
    send_server(cmd, data) ; // "" is the data I sent
    SerialMon.println(read_buffer) ;
    return ;
  }

  // Special Command ram
  if (0 == strncasecmp("ram", buf, 3)) {
    SerialMon.print("Free Memory (bytes): ");
    SerialMon.println(freeMemory());
    return ;
  }

  // Special Command sendmsmts
  if (0 == strncasecmp("sendmsmts", buf, 9)) {
    SerialMon.println("sendmsmnts") ;
	  // get lastdate from server
    //char *lastdate = send_server("lastdate","") ; //"2018-10-22 08:29:00" ; // last record in server that we are looking
	  String status_string = collect_status() ;
	  const char * status = status_string.c_str() ;
    send_server("status", status) ; // get server's last date
	  if (0== strncmp("lastdate=",read_buffer,9)) { // Server gave us his last date
			strncpy(server_lastdate, read_buffer + 9, 19) ; // save server_lastdate
	  }
	  SerialMon.print("Server's lastdate: ") ; 	SerialMon.println(server_lastdate) ;
    send_msmts() ;
  	if (0== strncmp("lastdate=",read_buffer,9)) { // Server gave us his last date
  			strncpy(server_lastdate, read_buffer + 9, 19) ; // save server_lastdate
  	}
    return ;
  }

  // Special Command set clock from tower
  if (0 == strncasecmp("autoclock", buf, 9)) {
    set_clock_from_tower() ;
    return ;
  }
  
  // Special Command BYPASS - puts the modem in bypass mode
  if (0 == strncasecmp("bypass", buf, 6)) {
	  modem.commandMode() ; // make sure in command mode. same as +++ AT command.  only stays in AT mode for 10 seconds
	  modem.sendAT("AP5");    // bypass mode
	  modem.readResponseString(200);
	  modem.sendAT("WR");    // write
	  modem.readResponseString(200);
	  modem.sendAT("FR");    // reset
	  modem.readResponseString(200);
	  SerialMon.println("Bypass Mode - get out with \"TRANS\"");
    return ;
  }
  
  // Special Command TRANS - transparent mode, gets out of bypass
  if (0 == strncasecmp("trans", buf, 5)) {
	  modem.commandMode() ; // make sure in command mode. same as +++ AT command.  only stays in AT mode for 10 seconds
	  modem.sendAT("AP0");    // bypass mode
	  modem.readResponseString(200);
	  modem.sendAT("WR");    // write
	  modem.readResponseString(200);
	  modem.sendAT("FR");    // reset
	  modem.readResponseString(200);
	  SerialMon.println("Transparent Mode");
    return ;
  }
  
    
  // if we got here, assume we are executing an AT command
  // if the AT command is preceded by a backtick (`), then this is a byppass mode AT command and we don't enter commandMode

  // for normal AT commands, lower case is OK
  buf[0] = toupper(buf[0]) ; // make sure first 2 chars are uppercase
  buf[1] = toupper(buf[1]) ;
  char at_cmd[3] ;
  memcpy(at_cmd, &buf[0], 2) ; // just look at the 2 character at command
  at_cmd[2] = '\0' ;

  int timeout = 200 ; // change timeout for slow commands
  if (0 == strcmp("LA", at_cmd)) { // lookup address is slow
    SerialMon.println("Timeout 5 seconds") ;
    timeout = 5000 ;
  }

  // print the AT you typed
  //SerialMon.print("AT Command: ") ;
  SerialMon.print(buf) ;
  
  // is this a bypass mode command? - does it start with AT?  If so, delete the AT and don't go into commandMode
    if (0 == strncmp("AT", buf, 2)) { // Bypass At Command
		 modem.sendAT(buf+2);    // send the command - skip the AT because AT is added by sendAT
		 for (int i=0; i<10; i++){ // this doesn't always work - sometimes I don't get the response.  Don't know why - probably some timing issue
			String at_out = modem.readResponseString(2000);
			SerialMon.println(at_out) ;
			if (at_out == String("OK")) break ;
		 }
	}
	else { // trasparent mode AT command
		modem.commandMode() ; // make sure in command mode. same as +++ AT command.  only stays in AT mode for 10 seconds
		modem.sendAT(buf);    // send the command
	
		// print the response to the AT command
		String at_out = modem.readResponseString(timeout);
		//SerialMon.print(", Response:) ;
		SerialMon.print(" \""); SerialMon.print(at_out) ; SerialMon.print("\" ") ;
	
		// check command to see if I want to display additional information about the response
		//SerialMon.println(at_cmd) ;
		if (0 == strcmp("AI", at_cmd)) {
		  if (at_out == "2C") {
			SerialMon.print("Power Saving Mode" ) ;
		  }
		  else if (at_out == "0") {
			SerialMon.print("Connected - Ready" ) ;
		  }
		  else if (at_out == "22") {
			SerialMon.print("Registering..." ) ;
		  }
		  else if (at_out == "23") {
			SerialMon.print("Connecting..." ) ;
		  }
		  else if (at_out == "25") {
			SerialMon.print("Registration denied..." ) ;
		  }
		  else if (at_out == "2A") {
			SerialMon.print("Airplane Mode" ) ;
		  }
		  else if (at_out == "2F") {
			SerialMon.print("Bypass Mode" ) ;
		  }
		  else if (at_out == "FF") {
			SerialMon.print("Initializing..." ) ;
		  }
		}
		SerialMon.println("") ;
	}
}
