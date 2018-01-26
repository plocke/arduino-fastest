#include <Arduino.h>
#include <global_constants.h>
#include <global_functions.h>
#include <lcdhelper.h>
#include <serialdebug.h>
#include <shiftregister.h>
#include <game_simon.h>
#include <eepromhelper.h>
#include <fade.h>
void loop_simon() {
  setBothLCDLines("in simon yay!", "yup",  lcd);
}
