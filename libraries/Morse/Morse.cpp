/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Morse.h"

Morse::Morse(int pin) {
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void Morse::character(String morseCharacter) {
  
  if (morseCharacter == "O") {
    Morse::dash(); Morse::dash(); Morse::dash();
    delay(500);
  }
  
  if (morseCharacter == "S") {
    Morse::dot(); Morse::dot(); Morse::dot();
    delay(500);
  }
}

void Morse::dot() {
  digitalWrite(_pin, HIGH);
  delay(150);
  digitalWrite(_pin, LOW);
  delay(150);  
}

void Morse::dash() {
  digitalWrite(_pin, HIGH);
  delay(500);
  digitalWrite(_pin, LOW);
  delay(150);
}

