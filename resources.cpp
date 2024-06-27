#include <Arduino.h>
#include "resources.h"

bool time_delay(long& previousTime, const long& timeInterval) {
  if (millis() - previousTime > timeInterval) {
    previousTime = millis();
    return true;
  }
  else {
    return false;
  }
}

void LEDOFF() {

  // Turn all lights OFF
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
}

// bin is FLAGS
void lights(const uint8_t& bin) {

  LEDOFF();
  if (bin & 0x01) { digitalWrite(D6, HIGH); } // Left LED
  if (bin & 0x02) { digitalWrite(D7, HIGH); } // Right LED
  if (bin & 0x04) { digitalWrite(D6, HIGH); digitalWrite(D7, HIGH); } // Both LEDs are ON
}

