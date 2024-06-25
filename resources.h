// resources.h

bool is_Pushed(long &previousTime, const long &timeInterval, volatile uint8_t &COLOUR_FLAG);
bool time_delay(long& previousTime, long timeInterval);
void IRAM_ATTR GREENLED_ISR();
void IRAM_ATTR REDLED_ISR();
void IRAM_ATTR BLUELED_ISR();
void IRAM_ATTR YELLOWLED_ISR();