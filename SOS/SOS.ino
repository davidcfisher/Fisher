#include <Morse.h>

Morse morse(8);

void setup() {
}

void loop() {
  morse.character("O");
  morse.character("O");
  morse.character("S");
  delay(3000);
}
