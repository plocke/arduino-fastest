#ifndef game_reaction_h
#define game_reaction_h

void setNewRandomStartDelay();
void doLightAndSoundCheckBootRoutine();
boolean checkForStartButton();
void readInputPins();
void clearGameVars();
void loop_reaction();
void transitionToState(int newState);
void showDefaultStartInstructions();
void showScoreRecords();
void checkAdvanceTriggerType();
void showUsersTimes();
void checkForEarlyPressersAndDQthem();
void checkForWinnersUpdateStateIfAllUsersFinish();
void turnOffGoSignals();
String getPlayerColourNameFromPosition(int position);
boolean checkAndUpdateFastestTimesAcrossGames(int pressedButton, long winnerReactionTimeMillis);
boolean checkAndUpdateFastestTimesInEEPROM(int pressedButton, long winnerReactionTimeMillis);


#endif
