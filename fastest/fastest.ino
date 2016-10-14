#include <Arduino.h>


#define NUM_USER_BUTTONS 4

int userButtonReadPins[NUM_USER_BUTTONS];
int disqualifiedUsers[NUM_USER_BUTTONS];
long winningUserTimes[NUM_USER_BUTTONS] = {0,0,0,0};
int cumulativeScores[NUM_USER_BUTTONS] = {0,0,0,0};

//states
const int STATE_BOOTED_UP = 0;
const int STATE_WAITING_TO_START = 10;
const int STATE_COUNTDOWN_TO_GO = 20;
const int STATE_DISQUALIFIED_PLAYER = 30;
const int STATE_WAITING_FOR_WINNERS = 40;
const int STATE_GAMEOVER = 60;

//debug flag
const boolean DEBUG_SERIAL = true;

//analog user button inputs.  we are just using them for digital reads
const int PIN_USER1BUTTON = A0;
const int PIN_USER2BUTTON = A1;
const int PIN_USER3BUTTON = A2;
const int PIN_USER4BUTTON = A3;

//digital input buttons/switches
const int PIN_STARTBUTTON = 4;

//current state we are in
int state = STATE_BOOTED_UP;

//per game variables for timers and timing events

int randomizedStartDelayInMillis;
int numberOfFinishers = 0;
long goTimeMillis = 0;
long winnerTimeMillis = 0;
long runnerUpTimesMillis[NUM_USER_BUTTONS-1];
boolean gameInProgess = false;

//timing constants
const long MAX_WAIT_FOR_WINNERS_MILLIS = 5000;


//
int TONEPIN = 2;


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

void debugSerialPrintHelper(String stringToPrint, boolean newlineAfter) {
  if (DEBUG_SERIAL)
 {
   Serial.print(stringToPrint);
   if (newlineAfter) {
     Serial.println();
   }
 }
}

void setup() {
  //start serial communication if debug flag is set
  if (DEBUG_SERIAL) {
    Serial.begin(9600);
  }

  for (int i = 0; i < NUM_USER_BUTTONS; i++) {
     pinMode(userButtonReadPins[i], INPUT);
  }



  transitionToState(STATE_COUNTDOWN_TO_GO);
}

void setNewRandomStartDelay()
{
  randomSeed(millis()+analogRead(A5));
  randomizedStartDelayInMillis = random(3000, 10000);
  debugSerialPrintStringAndNumber("start delay: ", randomizedStartDelayInMillis);
}

boolean checkForStartButton()
{

}

void debugStateTransition(int currentState, int newState) {
      debugSerialPrint("Entering state ");
      debugSerialPrint(String(newState));
      debugSerialPrint(" From ");
      debugSerialPrintln(String(currentState));
}


void readInputPins() {
 userButtonReadPins[0] = digitalRead(PIN_USER1BUTTON);
 userButtonReadPins[1] = digitalRead(PIN_USER2BUTTON);
 userButtonReadPins[2] = digitalRead(PIN_USER3BUTTON);
 userButtonReadPins[3] = digitalRead(PIN_USER4BUTTON);
}



void loop() {
  //check for inputs that apply to any state

  //new game pressed


  //main logic.  change appropriate outputs depending on state, and/or transition to a new state if inputs result in it
   switch (state) {
    case STATE_WAITING_TO_START:
      //reset all per game arrays and timers
      //update lcd
      //
      //updateLCD2("Press start button for new game")
      //if start pressed
      break;
    case STATE_COUNTDOWN_TO_GO:
        if (millis() > goTimeMillis) {
          transitionToState(STATE_WAITING_FOR_WINNERS);
        } else {
          //set disqualified user if someone presses a button
          //pulse the 'wait for it LED'
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
      break;
   }

}

void transitionToState(int newState) {
  debugSerialPrintStringAndNumber("Transitioning to state ",newState);
  debugSerialPrintStringAndNumber("From state: ",state);

  switch (newState) {
    case STATE_GAMEOVER:
      showUsersTimes();
      break;
    case STATE_COUNTDOWN_TO_GO:
      setNewRandomStartDelay();
      goTimeMillis = millis()+randomizedStartDelayInMillis;
        //updateLCD1("Wait for it . . .");
      break;
    case STATE_WAITING_FOR_WINNERS:
        //set go LED and LCD
      break;

  }
  state = newState;


}

void showUsersTimes() {
      for (int i = 0; i < NUM_USER_BUTTONS; i++) {
                   long userTime = winningUserTimes[i];
                   if (userTime > 0)
                   {
                     long playerReactionTime = userTime - goTimeMillis;
                     debugSerialPrintStringAndNumber("Player ",i);
                     debugSerialPrintStringAndNumber("React time: ",playerReactionTime);
                   }


      }
}

void checkForWinnersUpdateStateIfAllUsersFinish() {
  if (numberOfFinishers < NUM_USER_BUTTONS) {
    readInputPins();
    for (int i = 0; i < NUM_USER_BUTTONS; i++) {
      if (userButtonReadPins[i] > 0 && winningUserTimes[i] == 0)
      {
        int pressedButton = i;
        winningUserTimes[pressedButton] = millis();
        if (numberOfFinishers == 0)
        {
          winnerTimeMillis = millis();
          long winnerReactionTime = winnerTimeMillis - goTimeMillis;
          debugSerialPrintStringAndNumber("Winner!  Button ",pressedButton);
          debugSerialPrintStringAndNumber("Time after signal: ",winnerReactionTime);
          cumulativeScores[i]++;

        } else {
          debugSerialPrintStringAndNumber("Runner up, button ",pressedButton);
          debugSerialPrintStringAndNumber("Position: ",numberOfFinishers+1);
          long runnerUpTimeBehindWinner = millis() - winnerTimeMillis;
          debugSerialPrintStringAndNumber("ms behind winner: ",runnerUpTimeBehindWinner);
        }
        numberOfFinishers++;
      }
    }
  } else {
    debugSerialPrintln("Everyone done.  Game over.");
    transitionToState(STATE_GAMEOVER);
  }
}
