//  TIA-Software_Mayfly_Card_Bee_DigiLTE.cpp - Copyright (c) 2020 TIA Software, LLC.  All rights reserved.

#include "TIA-Software_Mayfly_Card_Bee_DigiLTE.h"                     // include the header file


// CONSTRUCTOR
DigiLTE::DigiLTE() {}                                                 // constructor


TinyGsm DigiLTE::modem(SerialAT) {
}
    
    
TinyGsmClient DigiLTE::client(modem) {
  
}


// METHOD: setup module in bee socket
bool DigiLTE::setup()
{
  Serial.println("13: inside TIA-Software_Mayfly_Card_Bee_DigiLTE.cpp");
	analogWrite(A1, 255);  // to Bee pin 5 (reset_not) - High is NOT RESET
  pinMode(19, INPUT);   //to BEE pin 12 - BEE_CTS or ATTN in SPI mode. Don't mess with this
  pinMode(20, INPUT);   // to BEE pin 16 - RTS. Don't mess with this either
  pinMode(23, OUTPUT);    // to BEE pin 9 - Sleep control. This is the only pin that requires setting
  digitalWrite(23, LOW);  // LOW is not sleep, HIGH is sleep, make sure you set sleep mode SM:  0=never sleep, 1=sleep by pin 23

	
//	//  Missing SD card causes hang with solid red light - why is this here?
//	int first_time = 1 ;
//	while(1){
//		if (SD.begin(SD_CS_PIN)) break ;
//		if (first_time) {
//			logAndPrint(F("SD Card failure! Hang here until inserted."));
//			digitalWrite(9, HIGH);
//		}
//		first_time = 0 ;
//    }
//	digitalWrite(9, LOW) ;
	
  //// see if xbee is responsive now that pin 23 is low and the modem is not sleeping
	//enable_modem = detect_baud_rate() ;
	
  SerialAT.begin(9600) ;// try 9600 - preferred
	Serial.println(F("Trying 9600 Baud Rate")) ;
	int State8 = LOW;
	for (int i=0; i<2; i++) {
	  if (i==1)	{
		  SerialAT.begin(19200) ;// try 19200
		  Serial.println(F("Trying 19200")) ;
	  }
	  if ( modem.testAT(5000L)) {// testAT does a commandMode() and a loop for 5 seconds testing for any return from the XBee
		enable_modem = 1 ;  // in case it was disabled but allowed to try at noon
	  }
  }

//	// only do these things if modem is responsive
//	if (enable_modem) { 
//		// blink green 2 times and go on
//		logAndPrint(F("XBee is operational (blink green 2x).")) ;
//		pinMode(8, OUTPUT);
//		digitalWrite(8, HIGH);            delay(1000) ;
//		digitalWrite(8, LOW);             delay(1000) ;
//		digitalWrite(8, HIGH);            delay(1000) ;
//		digitalWrite(8, LOW);
//	
//	
//		set_xbee_modes() ; //initialize XBee's various settings: 
//		
//		// check all AT values against signature
//		check_signature() ;
//	
//		// Just display a bunch of things that are nice to know on bring-up
//		// run_at just executes the AT command and displays the text: then output of AT command
//		run_at("VR","Firmware version") ; // Firmware: I'm at 11413.   Use Remote Manager or XCTU from Digi to download and install updates
//		//run_at("IM","IMEI") ;// IMEI  - probably won't display until the modem connects. we just get "ok"
//		//run_at("S#","SIM") ;  // SIM card - probably won't display until the modem connects
//		// Show Remote Manager Status - it should be a "4" because we disconnected it above
//		//run_at("DI","Digi Remote Manager Status (1 = before internet, 4 = off)") ; // 0=connected 1=before internet 2=in progress 3=disconnected 4=not configured. Won't show right until modem connects.
//  return true;
//}
