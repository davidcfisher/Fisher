/* xBee_TIA-Software.h - Copyright (c) 2019 TIA Software, LLC.  All rights reserved.
  XBee library
  
  Bugs:

*/

#ifndef XBeeLib_h  // if multiple includes, it won't blow up
#define XBeeLib_h
#include <Sodaq_DS3231.h> // RTC Chip - OK even if it's already included  - take this out
#include <utilities.h> // date, time and data manipulations
void setFileTimestamp(File fileToStamp, uint8_t stampFlag) {
  DateTime now = rtc.now();
  fileToStamp.timestamp(stampFlag, now.year(), now.month(), now.date(), now.hour(), now.minute(), now.second()) ;
}

void logConsole(String text) { // log  comments.  Just put this inline: logConsole(F("comment")) ;
  SD.begin(SD_SS_PIN) ;    // simply doing this will allow the SD card processor to recover - but the Return Code is always bad (0)
  File consoleFile = SD.open("console.txt", FILE_WRITE);//Re-open the file
  String textString = getDateTime(); 	//Pre-pend with Date
  textString += " ";
  textString += text ;
  consoleFile.println(textString);//Write the text
  setFileTimestamp(consoleFile, T_WRITE);
  consoleFile.close();  //Close the file to save it
}

void logAndPrint(String text) { // print to serial monitor AND log in console stream
	SerialMon.println(text) ;
	logConsole(text) ;
}

//#include <Mayfly_EEPROM_Class_TIA-Software.h>
//Eeprom_Class eeprom;// create an EEPROM instance

// FUNCTION to execute and display an AT command *****************************************************************************************************************
String run_at(const char * at_cmd, const char *at_long) { // ex: run_at("FR", "This will Force Reset XBee") ;
	modem.commandMode() ;  // make sure we are still in command mode
    modem.sendAT(at_cmd);
    String at_out = modem.readResponseString(200);
    SerialMon.print(at_long) ;
    SerialMon.print(F(": ")) ;
    SerialMon.println(at_out) ;
	return at_out ;
}

// FUNCTION to test and set a modem parameter *****************************************************************************************************************
int test_and_set(const char * at_cmd, const char * value, const char * at_long) { //  ex: test_and_set("AM", "0","Airplane Mode")
	modem.commandMode() ;  // make sure we are still in command mode
    String value_string= String (value) ;
	modem.sendAT(at_cmd); // test the existing value
    String at_out = modem.readResponseString(200);
    if (at_out == value_string) {
        SerialMon.print(F("No need to set ")) ; SerialMon.print(at_long) ; SerialMon.print(F(" already set to: ")) ; SerialMon.println(value_string) ;
        return 0 ;
    }
    else {
        //SerialMon.print(F("  Setting " ) ;
        modem.sendAT(at_cmd + value_string); // set it to value	
        modem.waitResponse(200) ;
        SerialMon.print(F(" - we have set ")) ; SerialMon.print(at_long) ; SerialMon.print(F(" to ")) ; SerialMon.println(value_string) ;
        return 1 ;
    }
}

// function to compare XBee signature of AT commands to known good sig //////////////////////////////////////////////////////////////////////////////////
void check_signature(void) { // not sure HV and DI and CK give the right answers
	char * at;
	char * value ;
	// Changes from before: CP=1 (no profile) instead of 2, VR(firmware)=11413,  CP-1 does NOT appear to work. went back to CP-2 AT&T and will try CP-3 Verizon
	char sig[] = "AN-hologram,CP-2,BM-FFFFFFFFFFFFFFFF,BN-FFFFFFFFFFFFFFFF,AM-0,N#-2,IP-1,TL-3,$0-;;,$1-;;,$2-;;,TM-BB8,TS-BB8,DO-0,DL-132.148.85.45,OD-0.0.0.0,DE-50,C0-50,BD-3,NB-0,SB-0,RO-3,TD-0,FT-681,AP-0,D0-0,D1-0,D2-0,D3-0,D4-0,D5-1,D6-0,D7-1,D8-1,D9-1,P0-0,P1-0,P2-0,P3-1,P4-1,PD-7FFF,PR-7FFF,M0-0,BT-0,SM-1,SP-7530,ST-EA60,PA-A,PU-8CA00,CC-2B,CT-64,GT-64,PS-0,VR-11413,DI-4,CK-9B45" ;
	// following is what we used for FW 11410
	//char sig[] = "AN-hologram,CP-2,BM-FFFFFFFFFFFFFFFF,BN-FFFFFFFFFFFFFFFF,AM-0,N#-2,IP-1,TL-3,$0-;;,$1-;;,$2-;;,TM-BB8,TS-BB8,DO-0,DL-132.148.85.45,OD-0.0.0.0,DE-50,C0-50,BD-3,NB-0,SB-0,RO-3,TD-0,FT-681,AP-0,D0-0,D1-0,D2-0,D3-0,D4-0,D5-1,D6-0,D7-1,D8-1,D9-1,P0-0,P1-0,P2-0,P3-1,P4-1,PD-7FFF,PR-7FFF,M0-0,BT-0,SM-1,SP-7530,ST-EA60,PA-A,PU-8CA00,CC-2B,CT-64,GT-64,PS-0,VR-11410,DI-4,CK-4B42" ;
	// original "AN-hologram,CP-2,BM-FFFFFFFFFFFFFFFF,BN-FFFFFFFFFFFFFFFF,AM-0,N#-2,IP-1,TL-3,$0-;;,$1-;;,$2-;;,TM-BB8,TS-BB8,DO-0,DL-132.148.85.45,OD-0.0.0.0,DE-50,C0-50,BD-3,NB-0,SB-0,RO-3,TD-0,FT-681,AP-0,D0-0,D1-0,D2-0,D3-0,D4-0,D5-1,D6-0,D7-1,D8-1,D9-1,P0-0,P1-0,P2-0,P3-1,P4-1,PD-7FFF,PR-7FFF,M0-0,BT-0,SM-1,SP-7530,ST-EA60,PA-A,PU-8CA00,CC-2B,CT-64,GT-64,PS-0,VR-11410,VL-FW ver 11410  Prod,HV-Bootloader: 167,DI-Build: Nov  8 2018 13:24:07,CK-4B42" ;
	String save_sig = String(sig) ;
	at = strtok(sig,"-");  // strtok returns first token in string delimited by "-"
	modem.commandMode() ;
	while(at != NULL){
		modem.sendAT(at);    // send the command
		String at_out = modem.readResponseString(5000); // give more time for this: timeout typically 200.  HV takes a long time
		//Serial.print(at) ; Serial.print(" "); Serial.print(at_out) ;
		value = strtok(NULL,",") ;
		// special case for CP which is dependent on deployment.  Compare setting NOT to the signature but to the carrier variable
		if (0==strcmp(at, "CP")) {
			//Serial.println("comparing CP to carrier variable") ;
			value = carrier ;
		}		
		//Serial.println(value) ;
		if (strcmp(at_out.c_str(),value)) {
			Serial.print(at) ; Serial.print(" is set to "); Serial.print(at_out) ; Serial.print(" not ") ; Serial.println(value) ;
		}
		at = strtok(NULL,"-"); // get next AT
	}
	strcpy(sig,save_sig.c_str()) ; // restore the signature in case we want to run check_sig again
}

// function to initialize XBee's various settings
void set_xbee_modes() {
	modem.commandMode() ;
	//modem.sendAT(GF("FR")); // apply changes
	int change_cnt = 0 ;
    change_cnt += test_and_set("SM", "1", "Sleep mode") ; 		// enabled to sleep with pin 23. You can set SM=0 via mayfly_server and the modem won't sleep but it will use more battery
    change_cnt += test_and_set("AM", "0", "Airplane mode") ;	// off
    change_cnt += test_and_set("CP", carrier, "Carrier Profile") ; 	// Set for AT&T or Verizon  - Changes in CP are only affected after reboot
    change_cnt += test_and_set("AN", access_point_name, "Access Point Name") ; // should be hologram or whatever you set above
    change_cnt += test_and_set("C0", "50", "Source Port Name") ; // Source port - it may be required for communication - I don't know
    //change_cnt += test_and_set("DO", "8", "Device Options (no RM, yes PSM)") ; // I think this screws up the modem  - don't do this until you figure out how to get out of PSM cleanly
    change_cnt += test_and_set("DO", "0", "Device Options (no Remote Manager, no PSM)") ; // disable/disable bit 0=RM, bit 1=not used, bit 2=USB direct mode, bit 3=Power Saving Mode
    change_cnt += test_and_set("N#", "2", "Preferred Network Technology (no NB-IOT, just LTE-M)") ; // 0= LTE-M with NB-IoT fallback, 1= NB-IoT with LTE-M fallback, 2= LTE-M only, 3 NB-IoT only.
    change_cnt += test_and_set("AP", "0", "API Disabled - operate in Transparent mode (not Bypass=5)") ;
    change_cnt += test_and_set("BL", "0", "Bluetooth Disable") ; 

    // apply changes
    //if (change_cnt > 0) { // always apply changes.  e.g. You don't know if CP was changed but modem was never reset with FR
        modem.sendAT(GF("AC")); // apply changes
        modem.waitResponse(200) ;
		modem.sendAT(GF("WR")); // write changes so modem will remember these settings on next boot
        modem.waitResponse(200) ;
		modem.sendAT(GF("FR")); // Reset modem - really needed if CP changed
        modem.waitResponse(200) ;		
        SerialMon.println(F("Applied changes from above")) ;
    //}
}

//FUNCTION - Detect Baud Rate - it won't return if it can't communicate; blinking red green
int detect_baud_rate() {
	// figure out baud rate for communication to/from Modem - must set BD3 for 9600, BD4 for 19200  
	SerialAT.begin(9600) ;// try 9600 - preferred
	Serial.println(F("Trying 9600 Baud Rate")) ;
	int State8 = LOW;
	for (int i=0; i<2; i++) {
	  if (i==1)	{
		  SerialAT.begin(19200) ;// try 19200
		  Serial.println(F("Trying 19200")) ;
	  }
	  if ( modem.testAT(7000L)) {// testAT does a commandMode() and a loop for 7 seconds testing for any return from the XBee
		enable_modem = 1 ;  // in case it was disabled but allowed to try at noon
		return 1; 
	  }
	}
	// With the old code,  if the modem has a brown-out issue, the retry #2 will call this code and could hang here .... draining the battery... and losing measurements.
	// the only known way to get the modem out of this state is for a MANUAL full power off then on.  Multiple cycles may be needed.
	// so we needed to keep the modem in low power state and allow the Mayfly to continue to collect data until a human shows up or the battery cycles somehow.
	// an easy way to do that is to set num_wakes = 0
	// another idea is to set wake_time to just noon.  If somehow the modem get's out of trouble it can wake up and send/receive.  Server will need to send new wake time
	// an easier way to do that is to allow dialog if hour is 12 and enable_modem=0
		
	logAndPrint(F("Modem is unresponsive with these baud rates. Continuing but setting enable_modem to 0 so modem never turns on - never enter Dialog.")) ;
	enable_modem = 0 ;  // permanently disable the modem
	
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	//int State8 = LOW;
	// 
	for (int i=0; i<10; i++){ // 10 seconds of loops should be enough for human to see
		if (State8 == LOW) State8 = HIGH;
		else State8 = LOW; 
		digitalWrite(8, State8);
		digitalWrite(9, !State8);
		//digitalWrite(23,!State8) ; // try to get it out of sleep?  this doesn't work
		delay(1000) ;
	}
	digitalWrite(8, LOW); // don't leave any LED on
	digitalWrite(9, LOW);
	return 0;
}

// FUNCTION to setup the XBee.  Will NOT return if XBee is dead (flashing red/green) ******************************************************
void setup_xbee() {
	
    // modem pins
    pinMode(19, INPUT);   //to BEE pin 12 - BEE_CTS or ATTN in SPI mode. Don't mess with this
    pinMode(20, INPUT);   // to BEE pin 16 - RTS. Don't mess with this either
    pinMode(23, OUTPUT);    // to BEE pin 9 - Sleep control. This is the only pin that requires setting
    digitalWrite(23, LOW);  // LOW is not sleep, HIGH is sleep, make sure you set sleep mode SM:  0=never sleep, 1=sleep by pin 23

    // The system must be brought up by applying power to board via the USB, THEN power to the xbee via the battery
    // I think I solved this problem by switching to 19200 baud rate to modem. If that doesn't work:

    // Sequence to cause xbee unresponsive: (don't know why you would want to do this) - doesn't always work
    //   unplug xbee from USB
    //   connect battery or leave battery connected
    //   plug back into USB

    // Sequence to bring up successfully:
    //    Mayfly slide switch off (not necessary)
    //    unplug from USB
    //    unplug battery
    //    Mayfly slide switch on (necessary)
    //    plug into USB
    //    plug in battery quickly

    // Print the battery voltage
    //logAndPrint(F("Voltage: ")) ;
    //float v = get_voltage() ;
    //logAndPrint(String(v)) ;
	
	//  Missing SD card causes hang with solid red light - why is this here?
	int first_time = 1 ;
	while(1){
		if (SD.begin(SD_CS_PIN)) break ;
		if (first_time) {
			logAndPrint(F("SD Card failure! Hang here until inserted."));
			digitalWrite(9, HIGH);
		}
		first_time = 0 ;
    }
	digitalWrite(9, LOW) ;
	
    // see if xbee is responsive now that pin 23 is low and the modem is not sleeping
	enable_modem = detect_baud_rate() ;
	
	// only do these things if modem is responsive
	if (enable_modem) { 
		// blink green 2 times and go on
		logAndPrint(F("XBee is operational (blink green 2x).")) ;
		pinMode(8, OUTPUT);
		digitalWrite(8, HIGH);            delay(1000) ;
		digitalWrite(8, LOW);             delay(1000) ;
		digitalWrite(8, HIGH);            delay(1000) ;
		digitalWrite(8, LOW);
	
		
		// put the xbee in command mode for the next series - it probably has reached 10 second timeout by code above
		modem.commandMode() ;
		
		set_xbee_modes() ; //initialize XBee's various settings: 
		
		// check all AT values against signature
		check_signature() ;
	
		// Just display a bunch of things that are nice to know on bring-up
		// run_at just executes the AT command and displays the text: then output of AT command
		run_at("VR","Firmware version") ; // Firmware: I'm at 11413.   Use Remote Manager or XCTU from Digi to download and install updates
		//run_at("IM","IMEI") ;// IMEI  - probably won't display until the modem connects. we just get "ok"
		//run_at("S#","SIM") ;  // SIM card - probably won't display until the modem connects
		// Show Remote Manager Status - it should be a "4" because we disconnected it above
		//run_at("DI","Digi Remote Manager Status (1 = before internet, 4 = off)") ; // 0=connected 1=before internet 2=in progress 3=disconnected 4=not configured. Won't show right until modem connects.
	}

} // end setup_xbee

// function to connect to cellular.  *****************************************************************************************************************
int connect_to_network() { // Returns # seconds to connect or <0 if failed
    
    // digitalWrite(23, LOW); // wake up bee if asleep I think this might cause XBee to lose connection when we restart
    // do until we are connected (AI=0) - it can take 10 minutes. If it goes 14 minutes, give up.  We may have some measurements to make every 15 minutes
    int i = 0;
    int sec_delay = 10 ;
    int loops = 2*60/sec_delay ; // 2 minutes * 60 seconds / 10 second delay = 12 loops - far more than we need
    while (i < loops) {  // writes a line of status every 10 seconds until it connects. Will try for 120 seconds or 2 minutes then returns -1
		if (digitalRead(21)) {
			SerialMon.println(F("Let go of the button")) ;
			while (digitalRead(21)) {} // don't want to jump out of the connect_to_network
			return -1 ; // push the button to get out of this loop
		}
        modem.commandMode() ;  // make sure we are still in command mode
        SerialMon.print(i) ;
        modem.sendAT(GF("AI")); // registration status
        String at_response = modem.readResponseString(200); // save this result for testing=0
        SerialMon.print(F(". AI: ")) ;
        SerialMon.print(at_response) ;
		if (at_response == "2C") {  // in power saving mode.  Wake it up with any network command
			modem.sendAT(GF("LAlake-man.com")) ;
			//at_out =
			modem.readResponseString(200);
			SerialMon.print(F(" Getting out of PSM with AT=LA lake-man.com ")) ;
		}
        //modem.sendAT(GF("CI")); // Connection Indication
        //SerialMon.print(F(", CI: ")) ;
        //SerialMon.print(modem.readResponseString(200)) ;
		modem.sendAT(GF("DB")); // Cell Strength
        SerialMon.print(F(", DB: ")) ;
        SerialMon.print(modem.readResponseString(200)) ;
		modem.sendAT(GF("MY")); // My Address
		SerialMon.print(F(", MY: ")) ;
		SerialMon.print( modem.readResponseString(200));
        SerialMon.print(F(", Voltage: ")) ;
        SerialMon.println( get_voltage()) ;// Print the battery voltage
        if (at_response == "0") { // test the AI result NOW 
			String a = F("Connected to network in ") ;
			a += i*10 ;
			a += F(" seconds") ;
            logAndPrint(a) ;
			break ;
		}
        delay(sec_delay*1000) ;  // We'll replace this with a Mayfly sleep in the future
        i++ ;
        if (i == loops) {
            logAndPrint(F("Failed to connect to network. Exiting..")) ;
            return -1 ;
        }
    }
    int seconds_to_connect = i * sec_delay ;

    //// other stuff I might want to display
	// run_at("MY","My Address") ;  // internet address
	// run_at("N1","DNS 1") ;  // DNS Server 1
	// run_at("N2","DNS 2") ;  // DNS Server 2    
    return seconds_to_connect ;
} // end connect_to_network

// function to connect to the host  *****************************************************************************************************************
int connect_to_host(char * host, int port) { //  returns 0 if connected, -1 = failed
    // first check if we've been given a URL or an IP@
    char buf[100] ;
    strcpy(buf,host) ;
    char *x[30]; // could be a long domain name
    x[0] = strtok(buf,".");  // strtok returns first token in string delimited by "."   
    if (0 != atoi(x[0])) {  // atoi returns zero if it has non-numeric characters - so if this check is true, we have an integer and evidently an IP@
        SerialMon.print(F("Connecting to IP@:")) ; SerialMon.println(host) ;
        // convert host to String then to ip
        IPAddress ip = TinyGsmIpFromString(String(host)) ;
        client.connect(ip, port) ; // does a IP, DL, DE, WR, AC, CN.  the LA is skipped if we give an ip address.  This can't fail so don't check for failure
    }
    else {// Domain Lookup Address needs to happen
        SerialMon.print(F("Looking-up address of URL: ")) ; SerialMon.print(host) ;
        if (!client.connect(host, port)) { // does a LA then IP, DL, DE, WR, AC, CN.
            SerialMon.println(F(" [fail]"));
            return -1 ;
        }
    }
    //SerialMon.print(F("Ready to send to "));
    return 0 ;

    // Cal's version of modemConnect - not complete but it does send
    //  modem.sendAT(GF("LA"), "132.148.85.45");  // set destination
    //  modem.waitResponse(200) ;
    //  modem.sendAT(GF("IP1")); // go to TCP - will be done by connect below
    //  modem.waitResponse(200) ;
    //  modem.sendAT(GF("DL"), "132.148.85.45");  // set destination
    //  modem.waitResponse(200) ;
    //  modem.sendAT(GF("DEC0"));  // set destination  ERROR HERE: I think it should be DE50 not DEC0!
    //  modem.waitResponse(200) ;
    //  modem.sendAT(GF("WR"));  // write"
    //  modem.waitResponse(200) ;
    //  modem.sendAT(GF("AC"));  // apply changes
    //  modem.waitResponse(200) ;
    //  modem.sendAT(GF("CN"));  // exit command mode
    //  modem.waitResponse(200) ;

}	// end connect_to_host

//static int sim_ctr = 0 ;
// function to read_incoming_data *****************************************************************************************************************
int read_incoming_data(int output = 2) { // returns #bytes read  or negative value if failed
	// specify output = 0 to get header and data
	//			output = 1 to get just data
	//			output = 2 to get data enclosed in ~~ from mayfly_server (default)
	// returns bytes read if successful which includes 0
	//	= -4 if failed to receive anything (timeout 5 seconds)
	//	= -5 if no data inside ~~
	// 	= -6 if buffer overrun
	
////SIMULATION used to debug communication - bypasses read and just returns "lastdate".  helps debug whether or not server received a transmission
//	Serial.println("Simulating read_incoming_data") ;
//	if (sim_ctr < 3) {
//			strcpy(read_buffer,"lastdate=2019-04-25 10:15:00") ;
//	}
//	else {
//		strcpy(read_buffer,"lastdate=2019-04-25 11:15:00") ;
//	}
//	Serial.println(read_buffer) ;
//	sim_ctr++ ;
//	return strlen(read_buffer) ; // the output is in the global variable "read_buffer"
//////////////////////////////////////////////////	
//Serial.print("CI: ") ;
//modem.commandMode() ; // make sure in command mode.
//for (int i=0; i<10; i++) {
//	modem.sendAT("CI");    // see if transmission was successful
//	Serial.println( modem.readResponseString(200));
//	delay(2000) ;
//}
	

    // Wait for data to arrive
	//strcpy(read_buffer,"") ; // wipe out the buffer so we don't ever see previous responses - this is already done at the top of http_post	
	unsigned long timeout = millis();
	//unsigned long seconds = 0 ;

	// I think timing here may be crutial.  So take out the old seconds = xxxx/1000 division and Serial printing.
	// The mayfly may still be transmitting when we get here.
	// the larger the data out & the larger the data in increases the time to process. That's why we need 40 seconds here.
    while (client.connected() && !client.available()) { // wait 40 seconds for stuff to come back not printing "." every second
	
        unsigned long elapsed_seconds = millis() - timeout ;
		if (elapsed_seconds > 40000L) { // quit after 40 seconds
		    logAndPrint(F("Failed to receive - client never came available.")) ;
			return -4 ;
		}
		//if (elapsed_seconds > seconds) { // print every second
		//	SerialMon.print('.');
		//	seconds = elapsed_seconds + 1L ;
		//}
    };
    //SerialMon.println(F("Print received data:")); // I think putting things here will cause buffer overrun. So don't...
	Serial.println("client avail") ;

    // Read data including headers
	// client.connected goes away once all the data is read
    timeout = millis();
    unsigned long bytesReceived = 0;
    while (client.connected() && millis() - timeout < 1000L) { // This line means it will always take 1 seconds before we go on
        while (client.available()) {
            char c = client.read();
            read_buffer[bytesReceived] = c ;
            //SerialMon.print(c);
            bytesReceived += 1;
			if (bytesReceived >= read_buffer_size) {
				strcpy(read_buffer,"") ;
				logAndPrint(F("Read Buffer Overrun")) ;
				return -6 ;
			}
            timeout = millis();  // Each time the client becomes unavailable the timeout is reset to give me another second for client to become available again.
        }
    }
    read_buffer[bytesReceived] = '\0' ;   // Done reading
	
//	Serial.print("CI: ") ;
//modem.commandMode() ; // make sure in command mode.
//for (int i=0; i<10; i++) {
//	modem.sendAT("CI");    // see if transmission was successful
//	Serial.println( modem.readResponseString(200));
//	delay(5000) ;
//}	
	
    logAndPrint(F("RECEIVED:")) ;
	if (bytesReceived == 0) {  // never got a response 
		logAndPrint(F("NOTHING RECEIVED from server.")) ;
		return -4 ;
	}
	if (output == 0) { // return header and data
		logAndPrint(read_buffer) ;
		return bytesReceived ;
	}
	char * r ;
	if (output == 1) { // return data only
		r = strtok(read_buffer,"Content-Length:") ;
		r = strtok(NULL,"\n") ; // number of bytes as in "Content-Length: 345\n\n"
		r = strtok(NULL,"\n") ; 
		r = strtok(NULL,"") ;   // should be all the data
	}
	else { // return only stuff inside ~~
		r = strtok(read_buffer,"~~") ; // first stuff will be the header then "~~" then the response, then "~~"
		r = strtok(NULL,"~~") ;  // this is the real response;
		if (1 >= strlen(r)) { // something wrong. The server should send something.  print the header
			logAndPrint(F("Data from not enclosed in ~~:")) ; // probably NOT a response from mayfly_server
			logAndPrint(read_buffer) ; // probably nothing there now after strtok
			strcpy(read_buffer,"") ;
			return -5 ;
		}
	}
	strcpy(read_buffer,r) ;  // put the response at the front of the read_buffer
	logAndPrint(read_buffer) ;	
    return strlen(read_buffer) ; // the output is in the global variable "read_buffer"
} // end read_incoming_data

// function to make an HTTP GET request: *****************************************************************************************************************
int http_get(char * host, char * resource, char * parms, int output = 2 ) { // response will be in static "read_buffer" if successful. Returns <0 if failed
	// specify output = 0 to return received header and data
	//			output = 1 to return just data
	//			output = 2 to return data enclosed in ~~ from mayfly_server ( default)	
	// rc >= bytesReceived (which includes 0) if successfully connected sent and received
	//	= -1 if couldn't connect to network
	//	= -2 if couldn't connect to host
	// 	= -3 if illegal request
	//	= -4 if failed to receive anything (timeout 40 seconds) . client never came available in read_incoming_data
	//	= -5 if no data inside ~~
	// 	= -6 if buffer overrun

    if (0 > connect_to_network() ) return -1 ; // failed to connect - Returns # seconds to connect or 0 if failed
    if (0 > connect_to_host(host, 80)) return -2 ; // failed to connect - Returns 0 if connected, -1 = failed
	strcpy(read_buffer,"") ; // wipe out the buffer so we don't ever see previous responses	
    String Sparms = String(parms) ; // make the parms character array easy to work with
    if (Sparms.length() > 2048)  {
        SerialMon.println(F("Get is restricted to 2048 bytes. You are trying to send more than that.  Exiting...")) ;
        return -3 ;
    }
    Sparms.replace(" ","%20") ; // URL encode - probably need to do more here
    String line1 = String("GET "); line1 += resource; line1 += "?" ; line1 += Sparms ; line1 += " HTTP/1.1\r\n" ;
    String line2 = String("Host: "); line2 += host ; line2 += "\r\n" ;
    String line3 = "Connection: close\r\n\r\n";
    SerialMon.print(line1) ; 
    SerialMon.print(line2) ;
    SerialMon.print(line3);
    client.print(line1) ;
    client.print(line2) ;
    client.print(line3) ;
    int bytesReceived = read_incoming_data(output) ; // returns #bytes read  or negative value if failed
	// response will be in read_buffer.  failed response: buffer = ""
	return bytesReceived;
}// end http_get

// function to make an HTTP POST request: *****************************************************************************************************************
int http_post(char *host, char *resource, char *data, int output = 2) { // returns bytesReceived or negative number if failed.  Data will be in static "read_buffer"
	// specify output = 0 to get header and data
	//			output = 1 to get just data
	//			output = 2 to get data enclosed in ~~ from mayfly_server ( default)	
	// rc = bytesReceived (which includes 0) if successfully connected and sent
	//	= -1 if couldn't connect to network
	//	= -2 if couldn't connect to host
	//	= -4 if failed to receive anything (timeout 40 seconds). client never came available in read_incoming_data
	//	= -5 if no data inside ~~
	// 	= -6 if buffer overrun
	int bytesReceived = 0 ; // this is also used as the return code so consider changing it's name to "rc".
	strcpy(read_buffer,"") ; // wipe out the buffer so we don't ever see previous responses
	
	for (int i=0; i<3; i++ ) { // allow 2 retries
		
		if (i==2) { // go back to original settings - it changes any settings we've made via server_command.  instead, we could do this if we've had several failures then reset the count
			logAndPrint(F("Failed 2nd HTTP_POST. Executing setup_xbee() and retrying...")) ;
			retry_cnt_2++ ;
			setup_xbee() ;
		}
		
		if (i==1) { // just do a modem reset....  If this fixes the problem, then we should always FR before HTTP_POST
			logAndPrint(F("Failed 1st HTTP_POST. Executing FR and retrying...")) ;
			modem.commandMode() ;
			modem.sendAT(GF("FR")); // Reset modem - really needed if CP changed
			modem.waitResponse(200) ;
			retry_cnt_1++ ;
		}		
		//if (i==1) {
		//	// At leelanau, this NEVER worked because I would see a response between xx:05:00 and xx:10:00.  So try something different here like a modem reset or power cycle
		//	// try to resend but reset the modem
		//	logAndPrint(F("First retry")) ;
		//	modem.commandMode() ;
		//	// could change carriers here? if cp=2 change to cp=3?
		//	modem.sendAT("FR");    // Reset the modem
		//	//set_xbee_modes() ; // I suppose if we had a failed xmission, it could be because something got set wrong. No harm in setting values again.
		//				// wrong - if I set the CP to some other carrier, then I don't want it to be changed.
		//	//check_signature() ;
		//	delay(1000) ; // get out of commandMode
		//}
		
		//
		//if (i==1) {// This is a retry of the post.  On 2nd try, recycle the XBee or do a FR
		//	// We connected and sent but the post did not receive any response from the server. Try to send the post again and increment the retry_cnt.
		//	retry_cnt++ ; // put this in status so I know if the Mayfly is having trouble
		//	logAndPrint(F("Power cycle the XBee")) ;
		//	digitalWrite(23, HIGH);  //  HIGH is off, make sure you set sleep mode SM: 0=never sleep, 1=sleep by pin 23
		//	for (int j=0; j<50; j++) {
		//	    modem.commandMode() ; // make sure in command mode. same as +++ AT command.  only stays in AT mode for 10 seconds
		//		modem.sendAT("AI");    // send the command
		//		String at_out = modem.readResponseString(200);
		//		Serial.print(at_out) ;
		//		if (at_out != "0") break ;  // maybe this should be !=0 ?  We'll still be in commandMode after we break
		//		delay(1000) ;
		//	}
		//	digitalWrite(23, LOW);  // LOW is on
		//	//set_xbee_modes() ; // I suppose if we had a failed xmission, it could be because something got set wrong. No harm in setting values again.
		//		// wrong - if I set the CP to some other carrier, then I don't want it to be changed.
		//}
		
		if (0 > connect_to_network() ) { // failed to connect. We tried for 5 minutes to connect.  
			bytesReceived = -1 ;
			continue ; // iterate this loop
		}
		else {
			bytesReceived = 0 ; // so far so good
		}
		// on last iteration, HTTP_POST will return bytesReceived which is = 0 ;

		//if (0 > connect_to_network()) {
		//	if ( i==0 ) continue ; // failed to connect on first try. We tried for 5 minutes to connect.  Give it another 5 minutes
		//}
		//else return-1 ; // we've given it 10 minutes. Quit and wait for next hour

		if (0 > connect_to_host(host, 80)) { // failed to connect 
			bytesReceived = -2 ;
			continue ; // iterate this loop
		}
		else {
			bytesReceived = 0 ; // so far so good
		}
		
		delay(1000) ; // get out of commandMode
			
		// no need to URL encode on Post
		// build up all the stuff and print in 1 line
		
		int size = strlen(data) ;
		
		String post = F("POST ");
		post += resource ;
		post += F(" HTTP/1.1\r\nHost: ") ;
		post += host ;
		post += F("\r\nContent-Length: ") ;
		post += size ; 
		//post += F("\r\nConnection: close\r\n\r\n") ;
		post += F("\r\n\r\n") ;
		//post += data ;   //this may cause us to run out of memory.
		client.print(post) ;
		client.print(data) ;
		Serial.print(post) ;
		logAndPrint(data) ;		
		
// code inserted here seemed to cause the post to fail.  Why? 
				
		//client.print(F("POST "));
		//client.print(resource) ;
		//client.println(F(" HTTP/1.1")) ;
		//client.print(F("Host: "));
		//client.println(host) ;
		//client.print(F("Content-Length: "));
		//client.println(size); // length of data
		//client.println(F("Connection: close"));
		//client.println();
		//client.print(data); // use print not println so that size matches
		//client.println() ;  // Adding these 2 lines made it work reliably - I don't know why.  Maybe only 1 line is needed?
		//client.println() ;
		
		bytesReceived = read_incoming_data(output) ; // returns #bytes read  or negative value if failed
		// returns bytes read if successful which includes 0
		//	= -4 if failed to receive anything (timeout 5 seconds)
		//	= -5 if no data inside ~~
		// 	= -6 if buffer overrun
		if (bytesReceived>0 || bytesReceived < -4) break ; // Leave this loop if we got a response or the response didn't make sense. We should never get a response of 0 bytes from the server.
		
	} // end retries loop
	
	return bytesReceived ;
}// end http_post

// function to collect status  //////////////////////////////////////////////////////////////////////////////////
String collect_status() {		// collect all the status in csv form.  
	int seconds = connect_to_network() ; // do this now before we send status (DB needs to be set and time-to-connect). We'll be in commandMode after this
	String status = "" ;
	DateTime dt(rtc.makeDateTime(rtc.now().getEpoch()));
	dt.addToString(status); 
	
	// add DB
	status += ", DB " ;
	modem.sendAT("DB") ;
	String db = modem.readResponseString(200) ;
    status += db ;

	// add voltages
	status += ", v. " ;
	float v = get_voltage() ;
	status += v ;
	
	// add memory
	status += ", mem: " ;
	long mem = freeMemory() ;
	status += mem ;
	
	// add failures to connect to tower
	status += ", retry1: " ;
	status += retry_cnt_1 ;
	
	// add send retries of http_post
	status += ", retry2: " ;
	status += retry_cnt_2 ;
	return status ;
}

// send_server ######################################################################################
//  Consider getting rid of it - nobody uses it now 
//Sends some request to host and expects response  enclosed in "~~" in read_buffer
//			the request is sent as: request="whatever you put in *request"  and  data is sent as data="whatever you put in *data"
// requests that the server understands:
//		"status"	date-time, AI=0, ....
//		"complete"	#server_cmd in the table.  [ + reply=whatever if this request gets a reply ]
//		"email"		e.g. "Mayfly H just booted..."
// could be used to reply to a command, e.g.
//		send_server("complete", "23", "OK")  which tells the server I completed command ID# 23 in the command table with reply OK.  
//		send_server("complete", "23")  completed with no reply  
// if the reply buffer is not null, it will also send reply=xxxx.  

int send_server(const char *request, const char *data, const char *reply="") { // returns negative number if failed

    strcpy(send_buf, "request=") ;
    strcat(send_buf,request) ;
    strcat(send_buf,"&mayfly_id=") ;
    strcat(send_buf,logger_name) ;
    strcat(send_buf,"&data=" );
    strcat(send_buf,data) ;
	if (0 != strlen(reply)){    	// add reply=xxxx if reply is not null
		strcat(send_buf,"&reply=" );
		strcat(send_buf,reply) ;
	}
    SerialMon.print(F("Sending: ")) ;
	SerialMon.print(host) ;
	SerialMon.print(resource) ;
	SerialMon.print(F(" as POST with parms: ")) ;
	SerialMon.println(send_buf) ;

    // call the host
    int rc = http_post(host, resource, send_buf) ; // returns negative number if failed
	return rc;
} // end send_server

//// function to get the value of a key-value pair - probably not needed,  not tested ######################################################################################
//// find value of given key in array of key-value pairs
//char get_value(char *key, char *data) {
//    char *mykey = strtok(data,"=") ; // get the first key
//    char *myvalue = strtok(NULL,"&") ; // and the first value
//    while(mykey != NULL) {
//        SerialMon.print(F("key: ")) ;
//        SerialMon.println(mykey) ;
//        SerialMon.println(myvalue) ;
//        if (0 == strcmp(mykey,key)) {  // this is the key I want
//            SerialMon.println(F("FOUND")) ;
//            return myvalue ;
//        }
//        mykey = strtok(NULL,"=") ; // get the first key
//        myvalue = strtok(NULL,"&") ; // and the first value
//    }
//    return "" ;
//}

/////////////////////////////////////////////////////////////////////////////////////
// function to translate a msmt line to a structure  (date in seconds, id, parms[], parm_cnt).  Used in compression
char * get_parms(char *p,struct msmt_line * m) {
	
  // input: p points to the start of this line
  // output:epoch_seconds  and parms[] gets loaded with the values
  // also returns the ptr to the start of the next line
 
  // get the date
  int i=0 ;     // index into line - skips over date time:  yyyy-mm-dd hh:mm:ss,Zzzz,123.456,...
  while(p[i] != ',') { // look for end of date
	if (p[i] == '~' || p[i] == '\0') {return 0 ;} // this line is not a legal record
	i++ ;
  }
  if (i<15) {return 0 ;} // this line is not a legal record - datetime too small
  m->seconds = date_to_time(p) ;
//Serial.print(F("seconds in get_parms: ")) ;   Serial.println(m->seconds) ;
  if (m->seconds == 0) {return 0 ;} // this line is not a legal record - datetime invalid
  
  // get the id
  i++ ;
  int id_cnt = 0 ;
  while(p[i] != ',') { // look for end of id
	if (p[i] == '~' || p[i] == '\0') {return 0 ;} // this line is not a legal record
	//Serial.print(p[i]) ;
	m->id[id_cnt] = p[i] ;
	id_cnt++ ;
	m->id[id_cnt] = '\0' ;
	i++ ;
  }
  //Serial.print(F("mayfly id: ")) ; Serial.println(m->id) ; 

  // now get the parms by filling a buf with each and storing it's floating point value in the structure
  i++ ;
  m->parm_cnt = 0 ; // number of parms  so far
  char buf[20] = "";
  int bufcnt = 0 ;
  while (p[i] != '\0' ) { // step through line until we see an end char. Really don't need this test since we break out anyway
	//Serial.print("p[i]") ; Serial.println(p[i]) ;
    if ( p[i] == ',' || p[i] == '~') { // found a parm
      m->parms[m->parm_cnt] = atof(buf) ;
	//Serial.print(F("parm#: ")) ; Serial.print(m->parm_cnt) ; Serial.print(F(", buf: ")) ; Serial.print(buf) ;Serial.print(F(", parms")) ;Serial.println(m->parms[m->parm_cnt]) ;
	  m->parm_cnt++ ; 
      bufcnt = 0 ;
      if (p[i] == '~') return p + i + 1 ; // end of this line. 
    }
    else { // fill a buffer with this parm
      buf[bufcnt] = p[i] ; // get another character
      bufcnt++ ;
    }
    i++ ;
    buf[bufcnt] = '\0' ; // keep terminating this parm as we fill it
  }
  return p + i ;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// function to compress a line of data in the buffer to transmit
 // note: if anything (besides parm values) changed from last line, I reset stuff by sending an uncompressed line.  That saves the server from getting confused.
 char * compress_line(char * start_ptr) { 
	// start_ptr is the start of this line to compress
	// returns the ptr to the last character of the compressed or uncompressed line
	char * insert_ptr = start_ptr ;  // where I will insert a new compressed line
	m1 = m2 ; // move the last struct to the first struct
	char *next_ptr = get_parms(start_ptr, &m2) ; // process this line to 2nd struct,  and get ptr to next line
	m2.last_delta = -999999 ; // start with NO DEFINED last_delta for m2
	*next_ptr = '\0' ; // terminate this line so I can print it.  We are compressing as we fill the send_buf, so there isn't a line after this yet and the '\0' will get overwritten by next line
	Serial.print(F("Compress this line: ")) ; Serial.println(start_ptr) ; 
	m2.valid=1 ; // this will become m1 on the next go around
	if (m1.valid == 0 ) { // just starting on a fresh transmit
		//	m1.valid = 1 ;  // why did I do this?  I should get rid of it
		Serial.print(F("*** First Line ***")) ;
		goto SEND2 ;// we always send the first line so it can stay right where it is
	}
	
	// compare this line with the last
   if (0 != strcmp(m1.id,m2.id)) {
    Serial.print(F("different mayfly id's: ")) ; Serial.print(m1.id) ; Serial.print(F(" ")) ; Serial.println(m2.id) ;
    goto SEND2 ;
   }
    
    if (m1.parm_cnt != m2.parm_cnt) { //  Must have changed # of sensors or started with the wrong # of sensors.
      Serial.println(F("Parameter counts don't match")) ;
      goto SEND2 ;
    }   

    // store the interval in code or in seconds.  I'm just going to check for a few common values
    //intervals = ( 1, 2, 3, 4, 5, 6, 10, 12, 15, 20, 30, 60, 120, 480, 360, 480, 720, 1440 ) ;   minutes
	// code         a  b  c  d  e  f  g   h   i   j   k   l   m    n    o    p    q    r
	// "m ... r" cases won't be used for regular msmts until I change from a modulo 60 test to something else.
	// however, if msmts were skipped for some reason, I can still use any of these codes to indicate the diff from last to next msmt
    long int delta_seconds;	
	delta_seconds = m2.seconds - m1.seconds ;
//Serial.print(F("m2 sec: ")); Serial.print(m2.seconds) ; Serial.print(F(" m1 sec: ")); Serial.print(m1.seconds); Serial.print(F(" Diff: ")); Serial.println(delta_seconds) ;
    if (delta_seconds < 0L) { // negative value - shouldn't happen
      Serial.println(F("Date of this msmt is before last.")) ;
      goto SEND2 ;
    }
	// now we are committed to a compression
    if (m1.last_delta == delta_seconds) {
      // do nothing.  we'll get a comma in the parms section so our line will start with ,
    }
	// I'm only coding a few interval codes for common cases.  If I wanted to do more, I'd build a 2-dim table and do a loop
    else if (delta_seconds == 900L) {
      strcpy(insert_ptr, "i") ;  // i is 15 minutes = 900 seconds
      insert_ptr++ ;    }
    else if (delta_seconds == 1800L) {
      strcpy(insert_ptr, "k") ;  // k is 30 minutes = 1800 seconds
      insert_ptr++ ;
    }
	else if (delta_seconds == 3600L) {
      strcpy(insert_ptr, "l") ;  // k is 30 minutes = 1800 seconds
      insert_ptr++ ;
    }
    else { // just enter the number of seconds
      char interval[20] = "";
      //itoa(delta_seconds, interval, 10) ;
	  sprintf(interval, "%lu",delta_seconds); // using ul (unsigned long) notation to get this to work! itoa won't!!!  Negative values never get here.
//Serial.print(F("interval: ")) ; Serial.println(interval) ;
      strcpy(insert_ptr, interval);
      insert_ptr += strlen(interval) ;
    }
    m2.last_delta = delta_seconds ;  // save this delta seconds in m2 so it will be in m1 next time around
    for (int i = 0; i < m1.parm_cnt; i++) { // go through each parm  
      strcpy(insert_ptr, ",") ; // start putting parm in the buffer - also adds '\0'
      insert_ptr++ ;
      float diff = m2.parms[i] - m1.parms[i] ;
//Serial.print(i) ; Serial.print(F(". ")) ; Serial.print(m2.parms[i]) ;   Serial.print(F(" - ")) ; Serial.print(m1.parms[i]) ; Serial.print(F(" = ")) ; Serial.println(diff) ;
      if (diff == 0) {             //Serial.println(F("zero")) ;
        continue ; // don't have to put anything there for a zero
      }
      char diff_char[10] = "" ;
      float_to_char (diff, diff_char, 4) ;  // diff_char now is nn.nnnn  the MySQL tables only allow 4 decimal points
      strcpy(insert_ptr, diff_char) ;
      insert_ptr += strlen(diff_char) ;
    }
//Serial.println(F(""))) ;
    strcpy(insert_ptr, "~") ; // terminate this line
    insert_ptr++ ;
	strcpy(insert_ptr,"") ;// terminate this line so I can print it
	Serial.print(F("Compressed: ")) ; 	Serial.println(start_ptr) ; // print this compressed line
	return insert_ptr-1 ;

SEND2: // leave this line uncompressed 
    Serial.print(F("No compression")) ; 	Serial.println(start_ptr) ; // print this uncompressed line
	return next_ptr-1 ;
 } // end compress_line

//function  to check_id on SD card msmt vs loggername (allows any length loggername). 
bool same_id(char *buff) {
	int i = 0 ;
	while (1){
		int j = i+20;
		if(buff[j] != logger_name[i]) { // this character doesn't match
			if (buff[j]==',' && logger_name[i]== 0)  // end of id on the SD card and end of logger_name
				return 1 ; // they match
			else {
				logAndPrint(F("Garbage on SD Card. Id's don't match.")) ;
				return 0; // don't match
			}
		}
		i++ ; // so far so good
	}
}

// send_msmts
// function to send new measurements from SD Card  *****************************************************************************************************************
/*  process:
    - read backwards through the SD Card until we find lastdate
    - move forward to the next record
    - set up a HTTP Post, and start sending
    - read forward sending each byte of data
    - server may respond with date of last record received (and processed I assume)
*/

int send_msmts() { 	// assumes we have a valid timestamp in server_lastdate global variable. Returns negative number if failed
	//    return codes:
	//    0  - nothing more to send - server_lastdate same as my lastdate OR msmt file doesn't exist OR SD card missing.
	//    > 0  - a valid batch of msmts sent and received
	//	  < 0 - http_post failed

    File msmt_file = SD.open(msmt_file_name);
    if (!msmt_file) { // 
        logAndPrint(F("Can't find measurement file")) ;
		// I used to return -1 (comm failed) but that can shut down the dialog permanently if no measurements are being stored in the SD card.
		// This could happen if I just changed the name of the msmt file via server command OR the SD card was removed OR...
		// I could send email to server about missing SD card but that could get into a loop.
		// So, best thing is just to proceed as if we have nothing to send.
		return 0 ;
    }
	m2.valid = 0 ; // If compressing msmts,  when we are starting send_msmts, we need to invalidate any data that was left over in the m2 structure from a previous send.
    long int filesize = msmt_file.size();
    SerialMon.print(F("filesize: ")) ; SerialMon.println(filesize) ;

    byte x ;      // 1 byte we read from SD Car
	long int i ;  // index of byte in msmt_file
	long int previous_line = 0 ; // pointer to a byte in msmt_file
	char * bufptr = send_buf ;  // this is the buffer I'll send with a http_post

	// go from end of file backwards, counting lines with valid timestamps
	int msmt_count = 0 ;
	for (i=filesize-3; i>0; i--) { // start before lineend chars
		msmt_file.seek(i) ;
		x = msmt_file.read();
		
		// found the end:start of a line
		if (x == 10) {      // \n found which is the end of the next line
			// We are now in the file of lines: Aaaaaaa\nBbbbbbb\nCccccccc\n~Dddddddd
			//                                          ^
			// The line buffer contains line B and we are pointing at the end of A.
			// We want to compare the date_time in B to what the server sent us.
			// If matches, we want to send data starting with line C.
			// Also, if the mayfly id in B is NOT the same as this loggername, them we quit going backwards here.
			*bufptr = '\0' ; // terminate this line
			strrev(send_buf) ; // reverse the line 
			long int num_bytes = strlen(send_buf) ;
			previous_line = i+num_bytes + 2 ;   // remember pointer to line C - if we match with B, we start sending from here (+ 2 for \n and next byte)
			// break out of this loop if this line has a valid timestamp equal or less than the given timestamp or the id's don't match

			
			if (!same_id(send_buf)) break ; // 
			
			if (( num_bytes >= 19) && (0 == validate_timestamp(send_buf, 19))) { // valid timestamp
				// compare timestamps(a,b): a<b returns -1,  a==b returns 0, a>b returns +1, invalid b returns -2
				int y = compare_timestamps(server_lastdate,send_buf) ; // -1 SDcard is later, +1 SDcard is earlier, 0 is =
				if (y == 0 || y == 1 ) {
					SerialMon.println (F("Found same or earlier date.")) ;
					break;
				}
				msmt_count++ ;  // only count valid dates older than what I'm looking for
			}
			bufptr = send_buf ; // start filling next line at beginning of send_buf
		}
		else {
			*bufptr = x ; // add this char to buffer and point to next  ;
			bufptr++ ;
		}
	} // end going backwards
	if (msmt_count == 0) { 
		SerialMon.println(F("Nothing to do - server is up-to-date")) ;
		return 0 ;
	}
	if (i <= 0) SerialMon.print(F("Trying to send the entire file. #lines: ")) ;
	else SerialMon.print(F("Total lines to send: ")) ;
	SerialMon.println(msmt_count) ;
		
	// build the post 
	strcpy(send_buf, "request=savemsmts&mayfly_id=") ; // load the request and ID parms 
	strcat(send_buf,logger_name) ;
	strcat(send_buf,"&data=" ); // start the data parm
	long int data_size = filesize-previous_line ; // size of uncompressed msmts to send
	int parm_size = strlen(send_buf)  ;  // size of data in send_buf so far
	char *p = send_buf + parm_size ;
	char *end_buf = send_buf + batchsize -1 ;  // batchsize is the size of the send_buf. so end_buf is the address of the last byte in the buffer
	char * last_start = p ;  // last_start is the address of the start of the last line in the buffer. 	
	
	// read in the msmts
	msmt_file.seek(previous_line) ;
	int line_cnt = 0 ;
	for(i=0; i<data_size; i++) { // send only data_size bytes
		*p = msmt_file.read();
		if (*p == 13) *p = ' ' ; // replace \r with blank - so that the byte count remains correct
		if (*p == 10) { // end of this line
			*p = '~' ; // replace \n with line separators
			
			// here is where we can compress the last line
			if (compression == 1) {
				p = compress_line(last_start) ; // Will build a compressed line here, or leave it as is. Returns ptr to the last character in the line.
			}
			last_start = p+1 ;
			line_cnt++ ;
		}
		p++ ;
		if (p >= end_buf) break ;	// ran out of buffer		
	}
	*last_start =  '\0' ; // terminate buf at last full line
	SerialMon.print(F("Actual lines to send: ")); SerialMon.println(line_cnt) ;
	int rc = http_post(host, resource, send_buf) ; // SEND THE POST
	return rc ; // = bytesReceived if successful or negative number if not
} // end send_msmts


//function to determine hour[] min[] arrays from array of multiple h1:m1;h2:m2... event times //////////////////////////////////////////////////////////////////////////////////
int wake_bee_arrays(){
  int temp_wakes=0 ; // work with temp variables in case we have a problem.  Existing vars will still be valid
  int temp_hour[24] ; 
  int temp_min[24] ;
  // save the wake_bee_event_string in a String
	String save_string = wake_bee_event_string ; 	// save the string because strtok destroys it.  restore it later.
	char *r = strtok(wake_bee_event_string,":") ; // Start tokenizing with ":" then ";" Can't use"," because that separates key=val pairs in setvars
	while(temp_wakes<24) {  // we only accept 24 values
		if (r == NULL ) break ;
		if (!digits_only(r)) return 0 ; // non-digits found - fail this attempt. Leave it up to the server to warn users
		temp_hour[temp_wakes] = atoi(r) ;
		r = strtok(NULL,";") ;  // search for ";"
		if (!digits_only(r)) return 0 ; // non-digits found. fail this attempt
		temp_min[temp_wakes] = atoi(r) ;
		//SerialMon.print(String(wake_bee_hour[num_wakes]) + ":" + String(wake_bee_min[num_wakes]) + " ") ;  
		r = strtok(NULL,":") ;  // now get the next token
		temp_wakes++ ;
	}
  strcpy(wake_bee_event_string, save_string.c_str()) ; // restore wake string

 //SerialMon.print(F("Computing xBee wake times: "));
 num_wakes = temp_wakes ;
 for (int i=0; i<num_wakes; i++) {
	wake_bee_hour[i] = temp_hour[i] ;
	wake_bee_min[i] = temp_min[i] ;
	//SerialMon.print(wake_bee_hour[i]);   SerialMon.print(F(":")) ;  SerialMon.print(wake_bee_min[i]) ;  SerialMon.print(F(";")) ;  
 }
 //SerialMon.println("") ;
 return 1 ; // everything OK
} // end wake_bee_arrays

// function to execute a (unsolicited) command from the "mayfly_command" table that the server sent  //////////////////////////////////////////////////////////////////////////////////
int server_command() {  // specific command is in read_buffer. Format: command=ccc & id=### & data=dddddddddddddddddddddddd
	// executes the command and sends post which includes:
	//	id=idnum ( number of the server command to complete)
	//	reply=xxx	(optional: any reply that is expected like the result of an AT cmd)
	char * p = read_buffer+8 ; 			 // skip over "command=
	char * command_name = strtok(p,"&") ; // get the command name
	strtok(NULL,"=") ;	// skip over id=
	char * idchar = strtok(NULL,"&") ; 		 // get the id of the command table
    char * data = strtok(NULL,"") ; 	 // get the data which could be anything (use NULL delimiter to get the rest of the data)
	data +=5 ; // skip over "data="
	SerialMon.print(F("Server Command: ")) ;
	SerialMon.print(command_name) ;
	SerialMon.print(F(", data: ")) ;
	SerialMon.println(data) ;
	
	// set up for the http_post by filling the send_buf.  Last thing will "be reply=" so each command below can just add replies to the buffer
	strcpy(send_buf,"mayfly_id=") ; // Start filling the send_buf
	strcat(send_buf,logger_name) ;
	strcat(send_buf, "&request=complete&id=") ;  // old sketches used data=xx instead of id=xx.  The server code will handle either case.
	strcat(send_buf,idchar) ;
	strcat(send_buf,"&reply=") ; // start the reply - stuff below may or may not add to the reply
	char failed[] = "Failed " ;  // used often so to save space
	
Serial.println("send_buf so far:") ;
Serial.println(send_buf) ;
	
	// wake_string - can also set wake string using setvars
	if (0 == strcmp(command_name,"wake_string")) { 		
		strcpy(wake_bee_event_string, data) ;
		// convert string to arrays
		if (!wake_bee_arrays()) {
			SerialMon.println(F("Failed setting wake_array variable!")) ;
			strcat(send_buf,failed) ; // put failure in reply 
		}
		else {
			strcat(send_buf,"w-s OK ") ; // reply 
		}
	} // end wake_string
	
	// at_cmds
	else if (0 == strcmp(command_name,"at_cmds")) {
		// go through data pairs: cmd1, cmd2, ...
		char * at_cmd = strtok(data,",") ; // get the first var name
		strcat(send_buf,"AT CMDS: ") ;
		String at_out ;
		while(true) {
			if (at_cmd == NULL ) break ;
SerialMon.print(F("Sending AT: ")) ; SerialMon.println(at_cmd) ;
			modem.commandMode() ; // make sure in command mode. same as +++ AT command.  only stays in AT mode for 10 seconds
			modem.sendAT(at_cmd);// execute at_cmd
			at_out = modem.readResponseString(200);
SerialMon.print(F("Receiving: ")) ; SerialMon.println(at_out) ;
			strcat(send_buf,at_cmd) ;
			strcat(send_buf,":") ;
			strcat(send_buf,at_out.c_str()) ;
			strcat(send_buf,", ") ;
			
			// special case for changing baud rate - issue this command by itself instead of in a string of AT commands.
			if (0== strncmp(at_cmd,"BD",2) || (0== strncmp(at_cmd,"bd",2))) {
				modem.sendAT("AC");// apply changes
				modem.readResponseString(200);
				enable_modem = detect_baud_rate() ;  // go here so the Mayfly can reset it's serial baud rate. 
			}
			at_cmd = strtok(NULL,",") ; // get the next cmd
		}
	}
	
	// setvars - handles setting any number of vars
	else if (0 == strcmp(command_name,"setvars")) {
		// there is a special case for the setvars=read_buffer_size. While I'm parsing the var=val in this read_buffer, I can't free it and re-malloc it. So, I'll schedule the free/malloc after the loop
		// we can have a number of vars to set in 1 command.  Each command will reply OK or insert NOT before the OK on a failure
		int save_read_buffer_size = 0 ; // no resize of read_buffer is scheduled yet
		// go through data pairs: var1=val1, var2=val2,
		char * var = strtok(data,"=") ; // get the first var name
		while(1) {
			if (var == NULL ) break ; // var is set before entering this loop and again just before the end of this loop
			char * val = strtok(NULL,",") ; // get the value
			// set var=val ;
			SerialMon.print(F("Setting: ")) ; SerialMon.print(var) ; SerialMon.print(F(" = ")) ; SerialMon.println(val) ;
			if      (0 == strcmp(var,"host")) strcpy(host,val) ; 
			else if (0 == strcmp(var,"resource")) strcpy(resource,val) ;
			else if (0 == strcmp(var,"access_point_name")) strcpy(access_point_name,val) ; 
			else if (0 == strcmp(var,"batchsize")) {  // always do a showvars after a buffer resize to see if it really happened
				int old_batchsize = batchsize ;
				batchsize = atoi(val) ; // get new batchsize
				String save_send_buf = send_buf ; // save what I've loaded so far
				free(send_buf) ;
				SerialMon.print(F("free then malloc for send_buf: ")) ; SerialMon.println(batchsize) ;
				send_buf = (char *)malloc(batchsize ); // get memory for the parms and the data and for a work area
				if (!send_buf ) {
					SerialMon.println(F("Could not get storage for send_buf. Reverting to last size.")) ;
					batchsize = old_batchsize ;
					send_buf = (char *)malloc(batchsize ); // get memory for the parms and the data and for a work area
					if (!send_buf ) { // don't know how this can happen
						logAndPrint(F("Could not get storage for send_buf. Communication will be shut down. Logging will continue.")) ;
					}
					else {
						strcat(send_buf,"NOT ") ; // put failure in reply (NOT OK)
					}
				}
				strcpy(send_buf, save_send_buf.c_str()) ; // reload the data
			}
			else if (0 == strcmp(var,"num_batches")) num_batches = atoi(val) ;
			else if (0 == strcmp(var,"min_xbee_voltage")) min_xbee_voltage = atoi(val) ; 
			else if (0 == strcmp(var,"sense_period")) sense_period = atoi(val) ;
			else if (0 == strcmp(var,"compression")) compression = atoi(val) ;
			else if (0 == strcmp(var,"msmt_file_name")) strcpy(msmt_file_name,val) ;  // can change the msmt_file_name when it gets big
			else if (0 == strcmp(var,"location")) strcpy(location,val) ;
			else if (0 == strcmp(var,"logger_name")) strcpy(logger_name,val) ;
			else if (0 == strcmp(var,"read_buffer_size")) save_read_buffer_size = atoi(val) + 1 ; // this schedules a resize of read_buffer for after this loop
			
			// we can't allow wake_string to be set by setvars because it will destroy the strtok command above. learn how to use strtok_r or don't allow this.
			// good example at: https://stackoverflow.com/questions/51525070/split-string-with-strtok-nested
			//else if (0 == strcmp(var,"wake_string")) {  // setvars expects var1=val1,var2=val2 and wake_string=1:0;2:0;3:0;..., so everything OK
			//	strcpy(wake_bee_event_string, data+12) ; // +12 skips over "wake_string="
			//	if (!wake_bee_arrays()) { // routine to convert string to arrays
			//		SerialMon.println(F("Failed setting wake_array variable!")) ;
			//		strcat(send_buf,"NOT ") ; // put failure in reply 
			//	}	
			//}
			
			else {
				Serial.print(F("unknown variable: ")) ;
				Serial.print(var) ;
				Serial.println("") ;
				strcat(send_buf,"NOT ") ; // put failure in reply 			
			}
			strcat(send_buf,"OK, ") ; // reply for the above action
			var = strtok(NULL,"=") ; // get the next var if there is one
		}
		
		// postponed read_buffer resize from above.  You should always ask for "showvars" after a change of buffer size to see if it really happened
		if (save_read_buffer_size != 0) {
			SerialMon.println(F("free for read_buffer")) ; 
			free(read_buffer) ;
			SerialMon.print(F("malloc for read_buffer_size: ")) ; SerialMon.println(read_buffer_size) ;
			read_buffer = (char *)malloc(save_read_buffer_size );
			if (!read_buffer ) {
				SerialMon.println(F("Could not get storage for read_buffer.")) ;  //go back to original read buffer size
				read_buffer = (char *)malloc(read_buffer_size );
			}
			else {
				read_buffer_size = save_read_buffer_size ;
			}
		}		
	} // end setvars
	
	// set_clock - old code we can probably delete since we have DT now
	else if (0 == strcmp(command_name,"set_clock")) {
			// data will be: nnnnnnnnn where nnn is epoch seconds
			char * junk ; // junk will contain a ptr after data - I don't use this
			long time = strtoul(data, &junk, 10); // convert large number to base 10 - couldn't get atoi to work
			time += 4 ; // it will take about 4 seconds elapsed time to get this command.
			Serial.print(F("Setting clock to: "))  ;
			Serial.println(time) ;
			rtc.setEpoch(uint32_t (time));			
			String timestamp = getDateTime();  // sets the currentyear,,, vars
			Serial.println(timestamp) ;
			DST = get_dst() ; // set the DST to agree with what server sent us.
			strcat(send_buf,"OK") ; // successful
	} 
	
//	else if (0 == strcmp(command_name,"write_eeprom")) {
//		 // get json string from server
//		 char * json = strtok(data,"=") ; // get the first var name
//Serial.print(F("json: ")) ; Serial.println(json) ;
//		 eeprom.writeStringToEEPROM( 0, json) ;
//	}
//	else if (0 == strcmp(command_name,"read_eeprom")) {  // reads and sets the variables
//		 eeprom.initializeProvisioningVariables() ;
//	}

	// send_status
	else if (0 == strcmp(command_name,"send_status")) {  
		String status_string = collect_status() ;
		strcat(send_buf,status_string.c_str()) ;
	}
	else if (0 == strcmp(command_name,"erase_file")) {
		if (SD.remove(data)) strcat(send_buf,"OK") ; // successful
		else strcat(send_buf,failed) ; // unsuccessful
	}
	else if (0 == strcmp(command_name,"rename_file")) {
		char *from = strtok(data,",") ;
		Serial.print(F("From: ")) ; Serial.println(from) ;
		char *to = strtok(NULL,"") ;
		Serial.print(F("To: ")) ; Serial.println(to) ;
		File fromFile = SD.open(from, FILE_READ);
		if (!fromFile){
			Serial.println(F("failed to open FROM")) ;
			strcat(send_buf,failed) ; // unsuccessful
		}
		else {
			if (!fromFile.rename(to)) strcat(send_buf,failed) ; // unsuccessful rename
			else strcat(send_buf,"OK") ; // successful
			fromFile.close();  //Close the file to save it
		}
	}
//	else if (0 == strcmp(command_name,"list_files")) {  // list all files - can't get this to work.
//		while(1) {
//			File entry = SD.openNextFile();
//			if (! entry)  break ;
//			//String list = SD.ls(LS_R) ;
//			Serial.println(entry.name()) ;
//		}
//		//list.replace("\r\n","~") ;
//Serial.println(list) ;
//		strcat(send_buf,list.c_str()); //put list in reply
//	}
	else if (0 == strcmp(command_name,"showvars")) {
		
		// this will only be used for testing.  I'll have to have the Mayfly on my desk to see what it's printing.
		//SerialMon.print(F("read_buffer_size ")) ;		SerialMon.println(read_buffer_size) ;
		//SerialMon.print(F("host ")) ;					SerialMon.println(host ) ;
		//SerialMon.print(F("resource ")) ;				SerialMon.println(resource ) ;
		//SerialMon.print(F("access_point_name ")) ;		SerialMon.println(access_point_name ) ;
		//SerialMon.print(F("batchsize ")) ;				SerialMon.println(batchsize) ;
		//SerialMon.print(F("num_batches ")) ;			SerialMon.println(num_batches) ;
		//SerialMon.print(F("wake_string "));	SerialMon.println(wake_bee_event_string ) ;
		//SerialMon.print(F("min_xbee_voltage ")) ;		SerialMon.println(min_xbee_voltage ) ;
		//SerialMon.print(F("sense_period ")) ;			SerialMon.println(sense_period ) ;
		//SerialMon.print(F("location " ) ;				SerialMon.println(location ) ;
		//SerialMon.print(F("msmt_file_name ")) ;		SerialMon.println(msmt_file_name ) ;
		//SerialMon.print(F("logger_name ")) ;  			SerialMon.println(logger_name ) ;
		//SerialMon.print(F("compression ")) ;  			SerialMon.println(compression ) ;  

		
		// reply to this command. data must be in the format: reply=var1:val1;var2:val2;...  
		String showvars = String( "read_buffer_size:") ;
		showvars +=  String(read_buffer_size)  +
		String(",host:") 					+ String(host) + 
		String(",resource:") 				+ String(resource) + 
		String(",access_point_name:")		+ String(access_point_name) + 
		String(",batchsize:")				+ String(batchsize)  +
		String(",num_batches:")  			+ String(num_batches)  + 
		String(",wake_string:")				+ String(wake_bee_event_string) + 
		String(",min_xbee_voltage:") 		+ String(min_xbee_voltage) + 
		String(",sense_period:") 			+ String(sense_period) + 
		String(",location:")				+ String(location) + 
		String(",msmt_file_name:") 		    + String(msmt_file_name) + 
		String(",logger_name:")			    + String(logger_name)  +
		String(",compression:")			    + String(compression)  ;
		strcat(send_buf,showvars.c_str()) ; // successful		
	}
	// send_console
	else if (0 == strcmp(command_name,"send_console")) { // send the last 1000 bytes of the console file		
		File cons_file = SD.open("console.txt");	// open the console file
		long int filesize = cons_file.size();
		long int start = filesize - 1000L - 225L ; 			// go to the end of the file and backup ~1000 bytes and an addition 225 bytes to get to this send_console command
		if (start < 0L) { start = 0L; }			// console is not that big
		long int end = filesize - 225L ;
		if (end < start) {end = filesize ;}  // just send what we have
		cons_file.seek(start) ;
		int index = strlen(send_buf) ;	//  end of send_buf
		for (long int i=start; i<end; i++) { // copy each character checking for \n \r and &
			byte x = cons_file.read();
			if (x == '&') {x = '@' ;} // get rid of any &, \r, \n
			else if (x == '\r') {x = '\\' ;} 
			else if (x == '\n') {x = 'n' ;} 
			else if (x == '=') {x = '~' ;} 
			send_buf[index] = x ;
			index++ ;
			send_buf[index] = '\0' ; // terminate buff as I go
			if (index >= batchsize-1) break ;
		}
Serial.println("done loading console") ;	
	}
	
	// if we didn't find it by now, it's an illegal command
	else {
		Serial.print(F("Illegal server command: ")) ;
		Serial.println(command_name) ;
		strcat(send_buf,"illegal") ; 
	}
	
	// now send response to this command so the server can mark it complete and go to the next command or lastdate
	// there may be a random "&" in the send_buf from replies above.  We should probably do some sort of encoding...	
	logAndPrint(F("SENDING response to server command:")) ;
	int rc = http_post(host, resource, send_buf) ; // returns negative number if failed
	return rc ;
	
} //end server_command

// function to send status to server //////////////////////////////////////////////////////////////////////////////////
int send_status() { // returns negative number if failed
	logAndPrint(F("SENDING status")) ;
	String status = collect_status() ;
	strcpy(send_buf,"mayfly_id=") ;
	strcat(send_buf,logger_name) ;
	strcat(send_buf, "&request=status&data=") ;
	strcat(send_buf,status.c_str()) ;
	int rc = http_post(host, resource, send_buf) ; // returns negative number if failed
	//SerialMon.println(r) ;
	return rc ;
} // end send status

// function to set clock from tower
void set_clock_from_tower() {
	modem.commandMode() ; // make sure in command mode. same as +++ AT command.  only stays in AT mode for CT msec which is probably 64
	uint32_t my_clock =  rtc.now().getEpoch() ;
	modem.sendAT("DT");    // send daytime command. The response is the current date and time of the local tower in seconds since 2000-01-01 00:00:00
	String at_out = modem.readResponseString(200);
	if (at_out.length() != 8) { // something wrong - should be a 8 digit hex number. Either firmware doesn't support or we aren't connected
		Serial.println("Can't set clock from tower") ;
		return ;
	}
	uint32_t dt = strtol(at_out.c_str(), NULL, 16);  // convert hex to int
	dt += 946684800L ;  // from 1972 to 2000 
	uint32_t diff = labs(my_clock - dt) ;
	if (diff > 5L ) { // allow 5 second diff
		Serial.print("myclock: ") ; Serial.println(my_clock) ;
		//Serial.print("dt in hex: ") ; Serial.println(at_out) ;
		Serial.print("dt: ") ; Serial.println(dt) ;
		Serial.print("diff: ") ; Serial.println(diff) ;
		Serial.print("Clock drifted more than 5 seconds.\nDateTime before: ") ; Serial.println(getDateTime()) ;
		rtc.setEpoch(dt) ;
		Serial.print("DateTime after:  ") ; Serial.println(getDateTime()) ;
	}
	else {
		Serial.println(F("Clock OK")) ;
	}
	return ;
} // end set_clock_from_tower


// function dialog - to converse with server //////////////////////////////////////////////////////////////////////////////////
// send my status and check responses for my following actions
int dialog() { // returns negative value if failed
	logAndPrint(F("Dialog starting")) ;
	
	// Start the dialog with: sending any new msmts.  If I don't have any new msmts, send status.
	//		1. email_cal if scheduled, or if not
	//		2. sending any new msmts, or none
	//		3. send status
	// don't wake/sleep the XBee here because it will make problems for the Mayfly_testtool which allows running "dialog".  Put wake/sleep in the sleeping_datalogger loop
	
	int rc ; // common return code		
	int batch_cnt = 0 ;
	if (email_txt != "") { // we have email to send
		logAndPrint(F("SENDING email")) ;
		strcpy(send_buf,"mayfly_id=") ;
		strcat(send_buf,logger_name) ;
		strcat(send_buf, "&request=email&data=") ;
		strcat(send_buf,email_txt.c_str()) ;
		int rc = http_post(host, resource, send_buf) ; // returns negative number if failed
		if (rc < 0) {//  if http_post failed, leave dialog and return negative number. We'll have to wait for next hour to connect again.
			goto FAILED_TO_SEND ;
		}
		email_txt = "" ;
	}
	else if (0 == validate_timestamp(server_lastdate,strlen(server_lastdate))) { // we have a valid saved timestamp
		logAndPrint(F("SENDING msmts")) ;
		rc = send_msmts() ;  // will send any new msmts and return >0 if sent, <0 if failed.  Returns 0 if server lastdate is up to date,  without ever sending any data
		if (rc == 0) { // Server is up-to-date and we didn't send data.  But since it's time to report, I send status in case the server wants to tell me something.
			SerialMon.println(F("Server is up-to-date, but I will send status since I'm just entering a dialog.")) ;
			rc = send_status() ; //  Give the server a chance to respond and [ optionally send some commands] then sends his lastdate
		}
		if (rc < 0) { // return if either the send_msmts or send_status failed
			goto FAILED_TO_SEND ;
		}
		batch_cnt++ ; // we sent a batch
	}
	else {
		logAndPrint(F("SENDING status")) ;
		SerialMon.println(F("No valid lastdate saved for server. Will send status first before any msmts.")) ;
		rc = send_status() ; //  Give the server a chance to respond and optionally send some commands. Finally, server will send his lastdate
		if (rc < 0) { // return if send_status failed
			goto FAILED_TO_SEND ;
		}
	}

	// Now respond to the server				
	while (true) {
		// keep doing what the server asks.  The response is always in read_buffer and is ready to be checked.  read_buffer will be blank if there was an error
		logAndPrint(F("Inside the Dialog loop...")) ;
		modem.commandMode() ; // make sure in command mode. same as +++ AT command.  only stays in AT mode for 10 seconds		
		if (0== strncmp("command=",read_buffer,8)) { // these commands come from the command table queued up in the server.
			// We need to respond with the identifier so the server knows the command was acknowledge and can be taken off the queue
			// Server tells Mayfly to execute a command (set variable, clock, etc).
			int rc = server_command() ;	// executes the command,  returns id =  identifying number of the of command in the command table. id = 0 if there is an error			
			if (rc < 0) { // return if post failed
				goto FAILED_TO_SEND ;
			}
			// falls through
		}
		else if (0== strncmp("lastdate=",read_buffer,9)) { // Server gave us his last date
			strncpy(server_lastdate, read_buffer + 9, 19) ; // save server_lastdate
			if (batch_cnt < num_batches) {
				rc = send_msmts() ; 	// send msmts if we have any.  rc = 1 if we did, which means we will loop here in the dialog until all msmts are sent.
					//    0  - nothing more to send - server_lastdate same as my lastdate
					//    >0  - a valid batch of msmts sent and received
					//	<0 - some kind of error.  log it
				batch_cnt++ ; // we sent a batch
				if (rc < 0) { // return if send_status failed
					goto FAILED_TO_SEND ;
				}
				if (rc == 0) break ;  		// we can quit if server is up-to-date.  Otherwise, we will loop sending batches
				logAndPrint(F("Sent a batch..")) ;
				// or fall through
			}
			else {
				logAndPrint(F("Max number of batches sent")) ;
				break ;
			}
		}
		else if(0 == strncmp("setclock=",read_buffer, 9)) { // set the clock.  We can probably get rid of this code now that we have DT
			// This is the automatic set clock when the server notices a drift in the clock. The clock can also be set via the command table via "set_clock"
			// read_buffer will be "setclock=nnnnnnnnn" where nnn is epoch seconds
			char * ptr ;
			long time = strtoul(read_buffer+9, &ptr, 10); // convert large number to base 10 - couldn't get atoi to work
			logAndPrint(F("Setting clock")) ;
			time += 6 ; // it will take about 6 seconds elapsed time to get a response.
			rtc.setEpoch(uint32_t (time));			
			String timestamp = getDateTime();
			DST = get_dst() ; // set the DST to agree with what server sent us.
			rc = send_status() ;
			if (rc < 0) { // return if send_status failed
				goto FAILED_TO_SEND ;
			}
			// fall through
		}
		// else other automatic commands
		
		else { // we didn't receive a legal/valid command or communication failed somehow. Don't want to get in a loop so just quit until next hour
			logAndPrint(F("Invalid command received from server. Leaving dialog." )) ;
			break ; 
		}
	}
	
	set_clock_from_tower() ;  // Seems like a good place to check/set the clock.  We probably are connected to the tower.
	
	logAndPrint(F("leaving dialog")) ;
	// sleep the XBee in the code after returning from dialog
	return 0;
FAILED_TO_SEND:
	logAndPrint(F("Communication failed. Leaving dialog")) ;
	return rc ; //  if http_post failed, leave dialog and return negative number. We'll have to wait for next hour to connect again.
} // end dialog

#endif