#ifndef serialdebug_h
#define serialdebug_h

#include <Arduino.h>

void debugSerialPrintHelper(String stringToPrint, boolean newlineAfter);
void debugSerialPrintStringAndNumber(String stringToPrint, int numberToPrint);
void debugSerialPrintln(String stringToPrint);
void debugSerialPrint(String stringToPrint);
void debugStateTransition(int currentState, int newState);

#endif
