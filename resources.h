// resources.h

bool time_delay(long& previousTime, const long& timeInterval);
void LEDOFF();
void lights(const uint8_t& bin);
void IRAM_ATTR GREENLED_ISR();
void IRAM_ATTR REDLED_ISR();
void IRAM_ATTR BLUELED_ISR();
void IRAM_ATTR YELLOWLED_ISR();