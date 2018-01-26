
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <global_constants.h>

//global hardware
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int userButtonReadPins[NUM_USER_BUTTONS];
eeprom_config_struct eepromsave;

//global game choice
GameType gameTypeEnum = SIMON;


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
