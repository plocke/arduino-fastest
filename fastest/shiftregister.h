#ifndef shiftregister_h
#define shiftregister_h

#include <Arduino.h>

void writeTo74HC595(byte b);
void turnOnWinLightForPlayer(int playerNumber);
void turnOffAllPlayerLights();

#endif
