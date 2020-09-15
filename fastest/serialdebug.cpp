#include <Arduino.h>
#include <global_constants.h>

//const boolean DEBUG_SERIAL = true;

void debugSerialPrintHelper(String stringToPrint, boolean newlineAfter) {
  if (DEBUG_SERIAL)
 {
   Serial.print(stringToPrint);
   if (newlineAfter) {
     Serial.println();
   }
 }
}

void debugSerialPrintStringAndNumber(String stringToPrint, int numberToPrint) {
  debugSerialPrintHelper(stringToPrint, false);
  debugSerialPrintHelper(String(numberToPrint), true);

}

void debugSerialPrintln(String stringToPrint) {
  debugSerialPrintHelper(stringToPrint, true);
}

void debugSerialPrint(String stringToPrint) {
  debugSerialPrintHelper(stringToPrint, false);
}

void debugStateTransition(int currentState, int newState) {
      debugSerialPrint("Entering state ");
      debugSerialPrint(String(newState));
      debugSerialPrint(" From ");
      debugSerialPrintln(String(currentState));
}
