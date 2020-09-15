#ifndef lcdhelper_h
#define lcdhelper_h
#include <Wire.h>
#include <LiquidCrystal.h>

void setTopLine(String line, LiquidCrystal lcd);
void setBottomLine(String line, LiquidCrystal lcd);
void setBothLCDLines(String line1, String line2, LiquidCrystal lcd);

#endif
