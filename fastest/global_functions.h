#ifndef global_functions_h
#define global_functions_h
void doLightAndSoundCheckBootRoutine();
void playInputSoundForLight(int lightNumber, int lengthOfTone);

int getToneForLight(int lightNumber);

boolean checkForStartButton();
void readInputPins();
String getPlayerColourNameFromPosition(int position);
void showDefaultStartInstructions();


#endif
