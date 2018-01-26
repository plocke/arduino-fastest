#include <Arduino.h>
#include <global_constants.h>
#include <global_functions.h>
#include <lcdhelper.h>
#include <serialdebug.h>
#include <shiftregister.h>
#include <game_reaction.h>
#include <eepromhelper.h>
#include <fade.h>

void setNewRandomStartDelay()
{
  randomSeed(millis()+analogRead(A5));
  randomizedStartDelayInMillis = random(3000, 10000);
  debugSerialPrintStringAndNumber("start delay: ", randomizedStartDelayInMillis);
}



void clearGameVars() {

  for (int i = 0; i < NUM_USER_BUTTONS; i++) {
    disqualifiedUsers[i] = 0;
    winningUserTimes[i] = 0;
    cumulativeScores[i] = 0;
    if (i < (NUM_USER_BUTTONS-1)) {
      orderedRunnerUpTimesBehindWinner[i] = -1;
      orderedRunnerUpNames[i] = "";

    }
  }

   runnerUpPosition = 0;
   randomizedStartDelayInMillis = 0;
   numberOfFinishers = 0;
   goTimeMillis = 0;
   winnerTimeMillis = 0;
   gameOverTimeMillis = 0;
   debugSerialPrintln("Game vars cleared");
   turnOffAllPlayerLights();
}

void loop_reaction() {
  //check for inputs that apply to any state

  //new game pressed


  //main logic.  change appropriate outputs depending on state, and/or transition to a new state if inputs result in it
   switch (state) {
    case STATE_WAITING_TO_START:
      //if start pressed transitionToState
      // otherwise check for trigger toggle

      //checkAdvanceTriggerType();
      showScoreRecords();
      if (checkForStartButton()) {
        transitionToState(STATE_COUNTDOWN_TO_GO);
      }
      break;
    case STATE_COUNTDOWN_TO_GO:
        if (millis() > goTimeMillis) {
          transitionToState(STATE_WAITING_FOR_WINNERS);
        } else {
          checkForEarlyPressersAndDQthem();
          fadeLED(PIN_WAITFORIT_LED);
        }
      break;
    case STATE_WAITING_FOR_WINNERS:
      checkForWinnersUpdateStateIfAllUsersFinish();
      if ((millis() - goTimeMillis) > MAX_WAIT_FOR_WINNERS_MILLIS) //we are done waiting
      {
        debugSerialPrintln("Done waiting for people to finish.  Game over.");
        debugSerialPrintStringAndNumber("Record since boot: ",fastestTimeSinceBoot);
        debugSerialPrintStringAndNumber("Fastest in eeprom: ",eepromsave.fastestTimeInEeprom);
        transitionToState(STATE_GAMEOVER);
      }
      break;
    case STATE_GAMEOVER:
      if (millis() > (gameOverTimeMillis + SHOW_EACH_USER_SCORE_TIME_MILLIS*numberOfFinishers)) {
        transitionToState(STATE_WAITING_TO_START);
      }
      if (checkForStartButton()) {
        transitionToState(STATE_COUNTDOWN_TO_GO);
      }
      if (millis()%SHOW_EACH_USER_SCORE_TIME_MILLIS == 0) {
        showUsersTimes();
      }
      break;
   }

}

void transitionToState(int newState) {
  debugSerialPrintStringAndNumber("Transitioning to state ",newState);
  debugSerialPrintStringAndNumber("From state: ",state);

  switch (newState) {
    case STATE_WAITING_TO_START:
      turnOffAllPlayerLights();
      debugSerialPrintln("Press start button or go trigger toggle");
      showDefaultStartInstructions();
      break;
    case STATE_COUNTDOWN_TO_GO:
      clearGameVars();
      setNewRandomStartDelay();
      goTimeMillis = millis()+randomizedStartDelayInMillis;
      setBothLCDLines("Wait for it . . .", " ", lcd);
      break;
    case STATE_WAITING_FOR_WINNERS:
      setBothLCDLines("Go ! ! !", " ", lcd);
      //write to LCD
      turnOffFadeLED(PIN_WAITFORIT_LED);
      if (goTriggerEnum == BOTH || goTriggerEnum == LIGHT) {
        digitalWrite(PIN_GO_LED, HIGH);
      }
      if (goTriggerEnum == BOTH || goTriggerEnum == SOUND) {
        pinMode(PIN_SPEAKER, OUTPUT); //weird workaround to buzzing during PWM
        tone(PIN_SPEAKER,NOTE_FREQUENCY);
      }
      break;
    case STATE_GAMEOVER:
      turnOffGoSignals(); //in case we had no winner
      gameOverTimeMillis = millis();
      break;

  }
  state = newState;


}



void showScoreRecords() {
  if ( (millis() - lastTimeMillisForDisplayChangeRotation) > SHOW_EACH_RECORD_TIME_MILLIS) {
    lastTimeMillisForDisplayChangeRotation = millis();
    switch (currentRecordToShow) {
      case NONE:
        showDefaultStartInstructions();
        if (fastestTimeSinceBoot < 0) {
          currentRecordToShow = EEPROMSAVE;
        } else {
          currentRecordToShow = SINCEBOOT;
        }
        break;
      case SINCEBOOT:
        if (fastestTimeSinceBoot < 0) {
          setBottomLine("No Record Yet   ", lcd);
        } else {
          setBothLCDLines("Best since boot:",getPlayerColourNameFromPosition(playerPositionHoldingFastestTime)+": "+fastestTimeSinceBoot+"ms" ,  lcd);
        }
        currentRecordToShow = EEPROMSAVE;
        break;
      case EEPROMSAVE:
        String bestEver = String(eepromsave.fastestTimeInEeprom);
        setBothLCDLines("Best time ever:",bestEver+"ms", lcd);
        currentRecordToShow = NONE;
      break;
    }
  }
}


void checkAdvanceTriggerType() //todo: make this work on not the game cycle button
{
  if(digitalRead(PIN_GOTRIGGERCYCLE) == HIGH && millis() > (lastTimeMillisForDisplayChangeRotation + 500))
  {
    lastTimeMillisForDisplayChangeRotation = millis();
    debugSerialPrintStringAndNumber("Trigger type changing from ",goTriggerEnum);
    lcd.clear();
    setBottomLine("Press start --->", lcd);
    switch (goTriggerEnum) {
      case LIGHT:
        goTriggerEnum = SOUND;
        setTopLine("Sound Only", lcd);
      break;
      case SOUND:
        goTriggerEnum = BOTH;
        setTopLine("Light and Sound", lcd);

      break;
      case BOTH:
        goTriggerEnum = LIGHT;
        setTopLine("Light Only", lcd);

      break;
    }
    debugSerialPrintStringAndNumber(" to ",goTriggerEnum);

  }
}


void showUsersTimes() {
  debugSerialPrintln("Showing runner up times");
  int positionToShow = (millis()-gameOverTimeMillis)/SHOW_EACH_USER_SCORE_TIME_MILLIS;
  if (positionToShow < runnerUpPosition)
  {
    setBothLCDLines("Finisher "+(String)(positionToShow+2), orderedRunnerUpNames[positionToShow]+": -"+orderedRunnerUpTimesBehindWinner[positionToShow]+"ms", lcd);
  }
}



void checkForEarlyPressersAndDQthem() {
    readInputPins();
    for (int i = 0; i < NUM_USER_BUTTONS; i++) {
      if (userButtonReadPins[i] > 0 && !disqualifiedUsers[i])
      {
        int pressedButton = i;
        disqualifiedUsers[pressedButton] = true;
        debugSerialPrintStringAndNumber("Disqualified User ",pressedButton);
        setBothLCDLines(getPlayerColourNameFromPosition(pressedButton),"Disqualified", lcd);
      }
    }
  }

void checkForWinnersUpdateStateIfAllUsersFinish() {
  if (numberOfFinishers < NUM_USER_BUTTONS) {
    readInputPins();
    for (int i = 0; i < NUM_USER_BUTTONS; i++) {
      if (userButtonReadPins[i] > 0 && winningUserTimes[i] == 0 && !disqualifiedUsers[i])
      {
        int pressedButton = i;
        winningUserTimes[pressedButton] = millis();
        if (numberOfFinishers == 0)
        {
          winnerTimeMillis = millis();
          long winnerReactionTime = winnerTimeMillis - goTimeMillis;
          boolean newRecordSinceBoot = checkAndUpdateFastestTimesAcrossGames(pressedButton, winnerReactionTime);
          String newRecord = "";
          if (newRecordSinceBoot) {
            newRecord = "New Best!";
            checkAndUpdateFastestTimesInEEPROM(pressedButton, winnerReactionTime);
          }
          setBothLCDLines(getPlayerColourNameFromPosition(pressedButton)+" Wins!", (String)winnerReactionTime+"ms "+newRecord,  lcd);
          debugSerialPrintStringAndNumber("Winner!  Button ",pressedButton);
          debugSerialPrintStringAndNumber("Time after signal: ",winnerReactionTime);
          cumulativeScores[i]++;

          //turn off go signals
          turnOffGoSignals();
          turnOnWinLightForPlayer(pressedButton);


        } else {
          debugSerialPrintStringAndNumber("Runner up, button ",pressedButton);
          debugSerialPrintStringAndNumber("Position: ",numberOfFinishers+1);

          long runnerUpTimeBehindWinner = millis() - winnerTimeMillis;
          orderedRunnerUpTimesBehindWinner[runnerUpPosition] = runnerUpTimeBehindWinner;
          orderedRunnerUpNames[runnerUpPosition] = getPlayerColourNameFromPosition(pressedButton);
          runnerUpPosition++;

          debugSerialPrintStringAndNumber("ms behind winner: ",runnerUpTimeBehindWinner);
        }
        numberOfFinishers++;
      }
    }
  } else {
    debugSerialPrintln("Everyone done.  Game over.");
  //  transitionToState(STATE_GAMEOVER);
  }
}

void turnOffGoSignals(){
  pinMode(PIN_SPEAKER, OUTPUT); //weird workaround to buzzing during PWM
  noTone(PIN_SPEAKER);
  pinMode(PIN_SPEAKER, INPUT); //weird workaround to buzzing during PWM
  digitalWrite(PIN_GO_LED, LOW);
}


boolean checkAndUpdateFastestTimesAcrossGames(int pressedButton, long winnerReactionTimeMillis) {
  if ( (fastestTimeSinceBoot < 0) || (winnerReactionTimeMillis < fastestTimeSinceBoot) ) {
    fastestTimeSinceBoot = winnerReactionTimeMillis;
    playerPositionHoldingFastestTime = pressedButton;
    return true;
  } else {
    return false;
  }
}

boolean checkAndUpdateFastestTimesInEEPROM(int pressedButton, long winnerReactionTimeMillis) {
  if ( (eepromsave.fastestTimeInEeprom <= 0) || (winnerReactionTimeMillis < eepromsave.fastestTimeInEeprom) ) {
    eepromsave.fastestTimeInEeprom = int(winnerReactionTimeMillis);
    EEPROM_writeAnything(EEPROM_SAVELOCATION, eepromsave);
    debugSerialPrintStringAndNumber("New fastest time saved to eeprom: ",eepromsave.fastestTimeInEeprom);
    return true;
  } else {
    return false;
  }
}
