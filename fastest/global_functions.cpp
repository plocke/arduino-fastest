#include <Arduino.h>
#include <shiftregister.h>
#include <lcdhelper.h>
#include <global_constants.h>
#include <global_functions.h>

boolean checkForStartButton()
{
  return digitalRead(PIN_STARTBUTTON) == HIGH;
}

void playInputSoundForLight(int lightNumber, int lengthOfTone) {
  tone(PIN_SPEAKER, getToneForLight(lightNumber), lengthOfTone);
}

void readInputPins() {
 userButtonReadPins[0] = digitalRead(PIN_USER1BUTTON);
 userButtonReadPins[1] = digitalRead(PIN_USER2BUTTON);
 userButtonReadPins[2] = digitalRead(PIN_USER3BUTTON);
 userButtonReadPins[3] = digitalRead(PIN_USER4BUTTON);
}

int getToneForLight(int lightNumber) {
    switch (lightNumber) {
      case 0:
        return 415;
      break;
      case 1:
        return 310;
      break;
      case 2:
        return 252;
      break;
      case 3:
        return 209;
      break;
  }
}


String getPlayerColourNameFromPosition(int position) {
  switch (position) {
    case 0: return "Blue"; break;
    case 1: return "Green"; break;
    case 2: return "Red"; break;
    case 3: return "Yellow"; break;
  }
}

void doLightAndSoundCheckBootRoutine(){
  setBothLCDLines("Fastest v"+codeversion,"github / plocke",lcd );
  for (int i = 0; i < NUM_USER_BUTTONS; i++) {
//todo move this into a shared function for simon
     turnOnWinLightForPlayer(i);
     playInputSoundForLight(i,SUCCESS_TONE_LENGTH_MS);
     delay(SUCCESS_TONE_LENGTH_MS+50);
  }
  turnOffAllPlayerLights();
  digitalWrite(PIN_WAITFORIT_LED, HIGH);
  delay(250);
  digitalWrite(PIN_WAITFORIT_LED, LOW);
  digitalWrite(PIN_GO_LED, HIGH);
  delay(250);
  digitalWrite(PIN_GO_LED, LOW);
  tone(PIN_SPEAKER, NOTE_FREQUENCY);
  delay(125);
  noTone(PIN_SPEAKER);


}

void showDefaultStartInstructions() {
  setBothLCDLines("Press start --->", "<--- Change Game Type", lcd);
}
