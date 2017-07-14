#include <Arduino.h>
#include <LiquidCrystal.h>
#include "fade.h"
#include "serialdebug.h"
#include "lcdhelper.h"
#include "shiftregister.h"

const String codeversion = "1.1";

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define NUM_USER_BUTTONS 4

int userButtonReadPins[NUM_USER_BUTTONS];

const int NOTE_FREQUENCY = 262;

//states
const int STATE_BOOTED_UP = 0;
const int STATE_WAITING_TO_START = 10;
const int STATE_COUNTDOWN_TO_GO = 20;
const int STATE_DISQUALIFIED_PLAYER = 30;
const int STATE_WAITING_FOR_WINNERS = 40;
const int STATE_GAMEOVER = 60;

//go trigger setting
enum GoTrigger {LIGHT,SOUND,BOTH};

//records
enum Records {NONE, SINCEBOOT};

//debug flag
const boolean DEBUG_SERIAL = false;


//analog user button inputs.  we are just using them for digital reads
const int PIN_USER1BUTTON = A0;
const int PIN_USER2BUTTON = A1;
const int PIN_USER3BUTTON = A2;
const int PIN_USER4BUTTON = A3;

//digital input buttons/switches
const int PIN_STARTBUTTON = A5;
const int PIN_GOTRIGGERCYCLE = A4;

//output pins
const int PIN_WAITFORIT_LED= 6;
const int PIN_GO_LED= 9;
const int PIN_SPEAKER = 7;


//variables that persist across games
GoTrigger goTriggerEnum = BOTH;
Records currentRecordToShow = NONE;
long lastTimeMillisForDisplayChangeRotation = 0;
long fastestTimeSinceBoot = -1;
int playerPositionHoldingFastestTime = -1;

//current state we are in
int state = STATE_BOOTED_UP;

//per game variables for timers and timing events
boolean disqualifiedUsers[NUM_USER_BUTTONS];
long winningUserTimes[NUM_USER_BUTTONS] = {0,0,0,0};
int cumulativeScores[NUM_USER_BUTTONS] = {0,0,0,0};
int orderedRunnerUpTimesBehindWinner[NUM_USER_BUTTONS-1] = {0,0,0};
String orderedRunnerUpNames[NUM_USER_BUTTONS-1] = {"","",""};
int runnerUpPosition = 0;
int randomizedStartDelayInMillis = 0;
int numberOfFinishers = 0;
long goTimeMillis = 0;
long winnerTimeMillis = 0;
long gameOverTimeMillis = 0;




//timing constants
const long MAX_WAIT_FOR_WINNERS_MILLIS = 3000;
const long SHOW_EACH_USER_SCORE_TIME_MILLIS = 2000;
const long SHOW_EACH_RECORD_TIME_MILLIS = 2000;

//const long SHOW_SCORES_GAMEOVER_TIME = 4000;


//

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

  doLightAndSoundCheckBootRoutine();
  pinMode (PIN_SPEAKER, INPUT); //weird workaround for buzzing issue, only set to output right before calling tone()
  transitionToState(STATE_WAITING_TO_START);
}

void doLightAndSoundCheckBootRoutine(){
  setBothLCDLines("Fastest v"+codeversion,"github / plocke",lcd );
  for (int i = 0; i < NUM_USER_BUTTONS; i++) {
     turnOnWinLightForPlayer(i);
     delay(250);
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


void setNewRandomStartDelay()
{
  randomSeed(millis()+analogRead(A5));
  randomizedStartDelayInMillis = random(3000, 10000);
  debugSerialPrintStringAndNumber("start delay: ", randomizedStartDelayInMillis);
}

boolean checkForStartButton()
{
  return digitalRead(PIN_STARTBUTTON) == HIGH;
}



void readInputPins() {
 userButtonReadPins[0] = digitalRead(PIN_USER1BUTTON);
 userButtonReadPins[1] = digitalRead(PIN_USER2BUTTON);
 userButtonReadPins[2] = digitalRead(PIN_USER3BUTTON);
 userButtonReadPins[3] = digitalRead(PIN_USER4BUTTON);
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

void loop() {
  //check for inputs that apply to any state

  //new game pressed


  //main logic.  change appropriate outputs depending on state, and/or transition to a new state if inputs result in it
   switch (state) {
    case STATE_WAITING_TO_START:
      //if start pressed transitionToState
      // otherwise check for trigger toggle

      checkAdvanceTriggerType();
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

void showDefaultStartInstructions() {
  setBothLCDLines("Press start --->", "<--- Signal type", lcd);
}

void showScoreRecords() {
  if ( (millis() - lastTimeMillisForDisplayChangeRotation) > SHOW_EACH_RECORD_TIME_MILLIS) {
    lastTimeMillisForDisplayChangeRotation = millis();
    switch (currentRecordToShow) {
      case NONE:
        showDefaultStartInstructions();
        currentRecordToShow = SINCEBOOT;
        break;
      case SINCEBOOT:
        if (fastestTimeSinceBoot < 0) {
          setBottomLine("No Record Yet   ", lcd);
        } else {
          setBothLCDLines("Best since boot:",getPlayerColourNameFromPosition(playerPositionHoldingFastestTime)+": "+fastestTimeSinceBoot+"ms" ,  lcd);
        }
        currentRecordToShow = NONE;
        break;
    }
  }
}


void checkAdvanceTriggerType()
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

String getPlayerColourNameFromPosition(int position) {
  switch (position) {
    case 0: return "Blue"; break;
    case 1: return "Green"; break;
    case 2: return "Red"; break;
    case 3: return "Yellow"; break;
  }
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
