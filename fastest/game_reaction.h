#ifndef game_reaction_h
#define game_reaction_h

void setNewRandomStartDelay();
void clearGameVars();
void loop_reaction();
void transitionToState(int newState);
void showScoreRecords();
void checkAdvanceTriggerType();
void showUsersTimes();
void checkForEarlyPressersAndDQthem();
void checkForWinnersUpdateStateIfAllUsersFinish();
void turnOffGoSignals();
boolean checkAndUpdateFastestTimesAcrossGames(int pressedButton, long winnerReactionTimeMillis);
boolean checkAndUpdateFastestTimesInEEPROM(int pressedButton, long winnerReactionTimeMillis);


#endif
