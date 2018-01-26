#ifndef global_constants_h
#define global_constants_h

#include <Arduino.h>
#include <LiquidCrystal.h>

const String codeversion = "2.0";

extern LiquidCrystal lcd;

const int NUM_USER_BUTTONS=4;

extern int userButtonReadPins[NUM_USER_BUTTONS];

const int NOTE_FREQUENCY = 262;

const int SUCCESS_TONE_LENGTH_MS = 350;

//states
const int STATE_BOOTED_UP = 0;
const int STATE_WAITING_TO_START = 10;
const int STATE_COUNTDOWN_TO_GO = 20;
const int STATE_DISQUALIFIED_PLAYER = 30;
const int STATE_WAITING_FOR_WINNERS = 40;
const int STATE_GAMEOVER = 60;

//game type
enum GameType {REACTION, SIMON};
extern GameType gameTypeEnum;

//go trigger setting
enum GoTrigger {LIGHT,SOUND,BOTH};

//records
enum Records {NONE, SINCEBOOT, EEPROMSAVE};

//debug flag
const boolean DEBUG_SERIAL = true;


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
extern GoTrigger goTriggerEnum;
extern Records currentRecordToShow;
extern long lastTimeMillisForDisplayChangeRotation;
extern long fastestTimeSinceBoot;
extern int playerPositionHoldingFastestTime;

//current state we are in
extern int state;

//per game variables for timers and timing events
extern boolean disqualifiedUsers[NUM_USER_BUTTONS];
extern long winningUserTimes[NUM_USER_BUTTONS];
extern int cumulativeScores[NUM_USER_BUTTONS];
extern int orderedRunnerUpTimesBehindWinner[NUM_USER_BUTTONS-1];
extern String orderedRunnerUpNames[NUM_USER_BUTTONS-1];
extern int runnerUpPosition;
extern int randomizedStartDelayInMillis;
extern int numberOfFinishers;
extern long goTimeMillis;
extern long winnerTimeMillis;
extern long gameOverTimeMillis;




//timing constants
const long MAX_WAIT_FOR_WINNERS_MILLIS = 3000;
const long SHOW_EACH_USER_SCORE_TIME_MILLIS = 2000;
const long SHOW_EACH_RECORD_TIME_MILLIS = 2000;

//const long SHOW_SCORES_GAMEOVER_TIME = 4000;


//eeprom saved values
struct eeprom_config_struct
{
    int fastestTimeInEeprom;
    int longestChainInEeprom;
};

extern eeprom_config_struct eepromsave;

const int EEPROM_SAVELOCATION = 0;
const boolean RESET_EEPROM = false; //set to true and next boot will clear eeprom

#endif
