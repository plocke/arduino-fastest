#include <Arduino.h>


int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by
const int FADEDELAY = 10;
long lastFadeChangeMillis = 0;




void turnOffFadeLED(int ledPIN) {
  digitalWrite(ledPIN, LOW);
}

void fadeLED(int ledPIN) {

  if ((millis() - lastFadeChangeMillis) > FADEDELAY)
  {
    analogWrite(ledPIN, brightness);
    lastFadeChangeMillis = millis();
    brightness = brightness + fadeAmount;

    // reverse the direction of the fading at the ends of the fade:
    if (brightness <= 0 || brightness >= 255) {
      fadeAmount = -fadeAmount;

    }
  }
}
