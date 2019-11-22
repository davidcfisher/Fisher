#include <Mayfly_Card_TIA-Software.h>

// establish instance of Mayfly Card
Mayfly_card mayflyCard;
Red_LED redLED;
Green_LED greenLED;
  
void setup() {
  mayflyCard.setup();                             // setup the Mayfly card
  delay(1000);
  redLED.turnOff();
  delay(1000);
  greenLED.turnOff();
  redLED.turnOn();
  SerialMon.println("exiting setup()");
}

void loop() {
}
