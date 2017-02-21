#include <LiquidCrystal.h>

void setTopLine(String line, LiquidCrystal lcd){
  lcd.setCursor(0, 0);
  lcd.print(line);
}

void setBottomLine(String line, LiquidCrystal lcd) {
  lcd.setCursor(0, 1);
  lcd.print(line);
}
void setBothLCDLines(String topLine, String bottomLine, LiquidCrystal lcd) {
  lcd.clear();
  setTopLine(topLine, lcd);
  setBottomLine(bottomLine, lcd);

}
