#include "protothreads.h"

#include "ESP12E_DEFINES.h"
#include "game.h"
#include "resources.h" // Functions
#include "LCD.h" // LCD Screen

// LEFT is D4, RIGHT IS D3
// Buttons for G R B Y -> RX, D8, D7, D6

// Blinking LED
int ledState = LOW;
long timeInterval = 1000; // in milliseconds
long previousTime = 0;

// Debouncing
long debounce_timeInterval = debounce_Time; //200
long debounce_previousTime = 0;

// Game Variables
int gameState = State::START; // Statemachine
int difficulty = 5;
uint8_t FLAGS = 0x00; // 4-bit variable [0000_0000]. Used to keep track which button is pressed. 0000=Option_0000=Colours


// Define protothread
static struct pt ptBlink;

int blinking_LED(struct pt* pt) {
    PT_BEGIN(pt);

    for (;;) {
        // Wait for a specific time (e.g., 1000 ms)
        PT_WAIT_UNTIL(pt, millis() - previousTime > timeInterval);
        previousTime = millis();

        // Toggle LED state
        ledState = (ledState == LOW) ? HIGH : LOW;
        digitalWrite(LED_BUILTIN, ledState);
    }

    PT_END(pt);
}

void IRAM_ATTR LEFTCLICK_ISR() {
    if (is_Pushed(debounce_previousTime, debounce_timeInterval, FLAGS) == false) {
    FLAGS |= 0x10; // Set Left Flag
    Serial.println("Left ISR");
  }
}

void IRAM_ATTR RIGHTCLICK_ISR() {
    if (is_Pushed(debounce_previousTime, debounce_timeInterval, FLAGS) == false) {
    FLAGS |= 0x20; // Set Right Flag
    Serial.println("Right ISR");
  }
}

void IRAM_ATTR GREENLED_ISR() {
  if (is_Pushed(debounce_previousTime, debounce_timeInterval, FLAGS) == false) {
    FLAGS |= 0x01; // Set G flag B0
    Serial.println("GREEN ISR");
  }
}

void IRAM_ATTR REDLED_ISR() {
  if (is_Pushed(debounce_previousTime, debounce_timeInterval, FLAGS) == false) {
    FLAGS |= 0x02; // Set R flag B1
    Serial.println("RED ISR");
  }
}

void IRAM_ATTR BLUELED_ISR() {
  if (is_Pushed(debounce_previousTime, debounce_timeInterval, FLAGS) == false) {
    FLAGS |= 0x04; // Set B flag B2
    Serial.println("BLUE ISR");
  }
}

void IRAM_ATTR YELLOWLED_ISR() {
  if (is_Pushed(debounce_previousTime, debounce_timeInterval, FLAGS) == false) {
    FLAGS |= 0x08; // Set Y flag B3
    Serial.println("YELLOW ISR");
  }
}


void setup() {

  // initialize inbuilt LED pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D3, INPUT_PULLUP); // RIGHT
  pinMode(D5, INPUT_PULLUP); // LEFT
  pinMode(RX, INPUT_PULLUP); // GREEN
  pinMode(D8, INPUT_PULLUP); // RED
  pinMode(D7, INPUT_PULLUP); // BLUE
  pinMode(D6, INPUT_PULLUP); // YELLOW



  Serial.begin(115200); // Set the baud rate to 9600 (you may have a different value)
  randomSeed(analogRead(0)); // Init random seed

  // Initialize protothread
  PT_INIT(&ptBlink);

  // Initialize LCD
  setup_LCD();
  show_splash();
  delay(3000);
  show_menu(difficulty);
  
  attachInterrupt(digitalPinToInterrupt(D4), LEFTCLICK_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(D3), RIGHTCLICK_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(RX), GREENLED_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(D8), REDLED_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(D7), BLUELED_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(D6), YELLOWLED_ISR, FALLING);
}

void loop() {
  blinking_LED(&ptBlink);

  if (gameState == )


}
