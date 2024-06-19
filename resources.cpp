#include <Arduino.h>
#include "resources.h"

// tests to see if a pb has already been pushed (ISR dupe flag prevention)
bool is_Pushed(long &previousTime, const long &timeInterval, const uint8_t &COLOUR_FLAG) {
  if (millis() - previousTime > timeInterval)
  {
    // Update previousTime
    previousTime = millis();

    if (COLOUR_FLAG != 0x00)
    {
      Serial.println("A PB IS ALREADY PRESSED!");
      return true;
    }
    else
    {
      Serial.println("PB PRESSED!");
      return false;
    }
  }

  return true; // True as in, not ready to accept inputs.
}

