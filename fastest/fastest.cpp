#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include "fade.h"
#include "serialdebug.h"
#include "lcdhelper.h"
#include "shiftregister.h"
#include "eepromhelper.h"
#include "global_constants.h"
#include "global_functions.h"
#include "game_reaction.h"
#include "game_simon.h"

void checkAdvanceGameType();


void setup() {
  lcd.begin(16, 2);

  if (DEBUG_SERIAL) {
    Serial.begin(9600);
  }

  //output pins, all our digital pins are output other than 0 and 1
  for (int i = 2; i<= 13; i++) {
    pinMode(i, OUTPUT);
  }

//input pins
  for (int i = 0; i < NUM_USER_BUTTONS; i++) {
     pinMode(userButtonReadPins[i], INPUT);
  }

  turnOffAllPlayerLights();
  pinMode(PIN_STARTBUTTON, INPUT);
  pinMode(PIN_GOTRIGGERCYCLE, INPUT);

  if (RESET_EEPROM) {
    debugSerialPrintln("Resetting EEPROM");
    eepromsave.fastestTimeInEeprom = -1;
    eepromsave.longestChainInEeprom = -1;
    EEPROM_writeAnything(EEPROM_SAVELOCATION, eepromsave);
  }


  EEPROM_readAnything(EEPROM_SAVELOCATION, eepromsave);
  debugSerialPrintStringAndNumber("EEprom saved record fastest: ",eepromsave.fastestTimeInEeprom);
  debugSerialPrintStringAndNumber("EEprom saved record chain: ",eepromsave.longestChainInEeprom);


  doLightAndSoundCheckBootRoutine();
  pinMode (PIN_SPEAKER, INPUT); //weird workaround for buzzing issue, only set to output right before calling tone()
  transitionToState(STATE_WAITING_TO_START);
}

void loop() {
  checkAdvanceGameType();
  if (gameTypeEnum == SIMON) {
    loop_simon();
  } else if (gameTypeEnum == REACTION) {
    loop_reaction();
  }
}

void checkAdvanceGameType()
{
  if(digitalRead(PIN_GOTRIGGERCYCLE) == HIGH && millis() > (lastTimeMillisForDisplayChangeRotation + 500))
  {
    lastTimeMillisForDisplayChangeRotation = millis();
    debugSerialPrintStringAndNumber("Game type changing from ",gameTypeEnum);
    lcd.clear();
    setBottomLine("Press start --->", lcd);
    switch (gameTypeEnum) {
      case SIMON:
        gameTypeEnum = REACTION;
        setTopLine("Game:Reaction", lcd);
      break;
      case REACTION:
        gameTypeEnum = SIMON;
        setTopLine("Game:Simon", lcd);
      break;
    }
    debugSerialPrintStringAndNumber(" to ",gameTypeEnum);

  }
}
