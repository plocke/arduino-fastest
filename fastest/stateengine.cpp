// void doActionsForStateFromMainLoop() {
//   switch (state) {
//    case STATE_WAITING_TO_START:
//      //if start pressed transitionToState
//      // otherwise check for trigger toggle
//
//      checkAdvanceTriggerType();
//      if (checkForStartButton()) {
//        transitionToState(STATE_COUNTDOWN_TO_GO);
//      }
//      break;
//    case STATE_COUNTDOWN_TO_GO:
//        if (millis() > goTimeMillis) {
//          transitionToState(STATE_WAITING_FOR_WINNERS);
//        } else {
//          checkForEarlyPressersAndDQthem();
//          fadeLED(PIN_WAITFORIT_LED);
//        }
//      break;
//    case STATE_WAITING_FOR_WINNERS:
//      checkForWinnersUpdateStateIfAllUsersFinish();
//      if ((millis() - goTimeMillis) > MAX_WAIT_FOR_WINNERS_MILLIS) //we are done waiting
//      {
//        debugSerialPrintln("Done waiting for people to finish.  Game over.");
//        transitionToState(STATE_GAMEOVER);
//      }
//      break;
//    case STATE_GAMEOVER:
//      if (millis() > (gameOverTimeMillis + SHOW_SCORES_GAMEOVER_TIME)) {
//        transitionToState(STATE_WAITING_TO_START);
//      }
//      if (checkForStartButton()) {
//        transitionToState(STATE_COUNTDOWN_TO_GO);
//      }
//      break;
//   }
// }
