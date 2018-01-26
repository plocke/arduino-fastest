#include <Arduino.h>

//Pin connected to latch pin (ST_CP) of 74HC595
const int PIN_LATCH_SHIFTREG = 13;
//Pin connected to clock pin (SH_CP) of 74HC595
const int PIN_CLOCK_SHIFTREG = 10;
////Pin connected to Data in (DS) of 74HC595
const int PIN_DATA_SHIFTREG = 8;



void writeTo74HC595(byte b) {
  digitalWrite(PIN_LATCH_SHIFTREG, LOW);
    // shift out the bits:
    shiftOut(PIN_DATA_SHIFTREG, PIN_CLOCK_SHIFTREG, MSBFIRST, b);
    //take the latch pin high so the LEDs will light up:
    digitalWrite(PIN_LATCH_SHIFTREG, HIGH);
}

void turnOffAllPlayerLights() {
  writeTo74HC595(B00000000);

}

void turnOnAllPlayerLights() {
  writeTo74HC595(B11110000);

}

void turnOnWinLightForPlayer(int playerNumber) {
  switch (playerNumber) {
    case 0:
      writeTo74HC595(B10000000);
    break;
    case 1:
      writeTo74HC595(B01000000);

    break;
    case 2:
      writeTo74HC595(B00100000);

    break;
    case 3:
      writeTo74HC595(B00010000);

    break;
  }
}
