#include <Arduino.h>
#include <global_constants.h>
#include <global_functions.h>
#include <lcdhelper.h>
#include <serialdebug.h>
#include <shiftregister.h>
#include <game_simon.h>
#include <eepromhelper.h>
#include <fade.h>

const int MAX_CHAINSIZE = 100;

const int FAIL_TONE_FREQUENCY_HZ = 50;
const int FAIL_TONE_LENGTH_MS = 1500;

const int STATE_WAITING_TO_START_SIMON = 100;
const int STATE_SHOW_CHAIN = 200;
const int STATE_WAIT_FOR_NEXT_BUTTON = 300;
const int STATE_PROCESSING_BUTTON_PUSH = 400;

int simonState = STATE_WAITING_TO_START_SIMON;
int simonChain[MAX_CHAINSIZE];
int chainSize = 0;
int checkSpotInChain=0;
long timeOfLastButtonStateChangeMillis = -1;
int lastButtonState[NUM_USER_BUTTONS] = {LOW,LOW,LOW,LOW};
int buttonState[NUM_USER_BUTTONS] = {LOW,LOW,LOW,LOW};




void loop_simon() {
  switch (simonState) {
    case STATE_WAITING_TO_START_SIMON:
      if (checkForStartButton()) {
        startNewGameSimon();
      }
    break;
    case STATE_SHOW_CHAIN:
      for (int i = 0; i < chainSize; i++) {
          showChainElement(i);
      }
      simonState = STATE_WAIT_FOR_NEXT_BUTTON;
    break;
    case STATE_WAIT_FOR_NEXT_BUTTON:
      readInputPins();
      for (int i = 0; i < NUM_USER_BUTTONS; i++) {

        if (userButtonReadPins[i] != lastButtonState[i]) { //press or hardware bounce
          timeOfLastButtonStateChangeMillis = millis();
        }

        if (millis()-timeOfLastButtonStateChangeMillis>100) { //we've been at the button state long enough

          if (userButtonReadPins[i] != buttonState[i]) { //state has changed
            buttonState[i] = userButtonReadPins[i];

            if (buttonState[i] > 0) {
                timeOfLastButtonStateChangeMillis = millis();
                int currentlyPressedButton = i;
                int correctButton = simonChain[checkSpotInChain];
                debugSerialPrintStringAndNumber("Got "+getPlayerColourNameFromPosition(currentlyPressedButton)+" button press ",currentlyPressedButton);
                debugSerialPrintStringAndNumber("Correct button is "+getPlayerColourNameFromPosition(correctButton)+" ",correctButton);


                debugChain();

                if (currentlyPressedButton == correctButton) //success press
                {
                  turnOnWinLightForPlayer(currentlyPressedButton);
                  playInputSoundForLight(currentlyPressedButton, SUCCESS_TONE_LENGTH_MS);
                  if (checkSpotInChain == (chainSize-1) ){ //got it all on to next round
                    debugSerialPrintStringAndNumber("Correct chain element chosen:", correctButton);
                    setBothLCDLines("You got it!", "Score="+String(chainSize),  lcd);
                    turnOffAllPlayerLights();
                    delay(100);
                    turnOnAllPlayerLights();
                    delay(100);
                    turnOffAllPlayerLights();
                    delay(100);
                    turnOnAllPlayerLights();
                    delay(100);
                    turnOffAllPlayerLights();
                    delay(1000);

                    addToChain();
                    simonState = STATE_SHOW_CHAIN;
                  } else { //got it right, keep checking
                    checkSpotInChain++;
                    simonState = STATE_WAIT_FOR_NEXT_BUTTON;
                    int numberLeftToGet = chainSize-checkSpotInChain;
                    setBothLCDLines(getPlayerColourNameFromPosition(currentlyPressedButton)+", Yes!!","Left to get: "+String(numberLeftToGet), lcd);
                  }
                } else { //fail press game over
                  setBothLCDLines("It was "+getPlayerColourNameFromPosition(correctButton),"Score:"+String(chainSize-1),  lcd);
                  tone(PIN_SPEAKER, FAIL_TONE_FREQUENCY_HZ, 1500);
                  simonState = STATE_WAITING_TO_START_SIMON;
                }
            } else {
              turnOffAllPlayerLights();
              debugSerialPrintStringAndNumber(getPlayerColourNameFromPosition(i)+" Button Release: ", i);
            }
          }


      }
      lastButtonState[i]=userButtonReadPins[i];
    }
    break;
  }
}

void debugChain() {
  debugSerialPrint("chain: ");
  for (int i = 0; i < chainSize; i++) {
      debugSerialPrint(getPlayerColourNameFromPosition(simonChain[i]));
      debugSerialPrint(":");
      debugSerialPrint(String(simonChain[i]));
      debugSerialPrint(",");
  }
 debugSerialPrintln("");
  debugSerialPrintStringAndNumber("chainSize: ",chainSize);
  debugSerialPrintStringAndNumber("checkSpotInChain: ",checkSpotInChain);
}

void addToChain() {
  int buttonToAdd = int(random(0, NUM_USER_BUTTONS));
  simonChain[chainSize] = buttonToAdd;
  checkSpotInChain = 0;
  chainSize++;
  debugSerialPrintStringAndNumber(getPlayerColourNameFromPosition(buttonToAdd)+" Button added to chain: ",buttonToAdd);
  debugChain();
}

void startNewGameSimon() {
  randomSeed(millis()+analogRead(A5));
  chainSize = 0;
  checkSpotInChain = 0;
  for (int i = 0; i < NUM_USER_BUTTONS; i++) {
    lastButtonState[i] = LOW;
    buttonState[i] = LOW;
  }
  setBothLCDLines("Get Ready!", "",  lcd);
  delay(1500);
  setBothLCDLines("Get Ready!", "Go!",  lcd);

  addToChain();
  simonState = STATE_SHOW_CHAIN;
}

void showChainElement(int chainPosition) {
  turnOnWinLightForPlayer(simonChain[chainPosition]);
  playInputSoundForLight(simonChain[chainPosition], SUCCESS_TONE_LENGTH_MS);
  delay(SUCCESS_TONE_LENGTH_MS+50);
  turnOffAllPlayerLights();
  delay(50);
}
