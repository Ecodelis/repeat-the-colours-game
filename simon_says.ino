#include "protothreads.h"

#include "ESP12E_DEFINES.h"
#include "game.h"
#include "resources.h" // Functions
#include "LCD.h" // LCD Screen

// LEFT is D5, RIGHT IS D3
// Buttons for G R B Y -> RX, D8, D7, D6

// Blinking LED
int ledState = LOW;
long timeInterval = 1000; // in milliseconds
long previousTime = 0;

// Draw Time
long drawInterval = 200; // in milliseconds
long previousDraw = 0;



// Debouncing
long debounce_timeInterval = debounce_Time; //200
long debounce_previousTime = 0;

// Game Variables
int confirmation = 0;
int gameState = State::MENU; // Statemachine
int difficulty = 5;
uint8_t FLAGS = 0x00; // 4-bit variable [0000_0000]. Used to keep track which button is pressed. 0000=Option_0000=Colours


// Define protothread
static struct pt ptBlink;
int blinking_LED(struct pt* ptBlink) {
    PT_BEGIN(ptBlink);

    for (;;) {
        // Wait for a specific time (e.g., 1000 ms)
        PT_WAIT_UNTIL(ptBlink, millis() - previousTime > timeInterval);
        previousTime = millis();

        // Toggle LED state
        ledState = (ledState == LOW) ? HIGH : LOW;
        digitalWrite(LED_BUILTIN, ledState);
    }

    PT_END(ptBlink);
}


// GPT-1 General Purpose Timer 1
long GPT1Interval = 1000;
long previousGPT1 = 0;

int GPC_1 = -1; // General Purpose Counter 1
int previousGameState = State::MENU;

static struct pt ptDraw;
int draw(struct pt* ptDraw) {
    PT_BEGIN(ptDraw);

    for (;;) {
        // Wait for a specific time (e.g., 1000 ms)
        PT_WAIT_UNTIL(ptDraw, millis() - previousDraw > drawInterval);
        previousDraw = millis();



        switch (gameState) {
          case MENU:
            if (confirmation == 0)
              show_menu(0, difficulty);
            
            if (confirmation == 1)
              show_menu(1, difficulty);
            break;

          case START:
            GPT1Interval = 1000;
            if (millis() - previousGPT1 > GPT1Interval) {
              GPC_1++;
              previousGPT1 = millis();

              if (GPC_1 == 0) show_start(0);
              if (GPC_1 == 1) show_start(1);
              if (GPC_1 == 2) show_start(2);
              if (GPC_1 == 3) show_start(3);
              if (GPC_1 == 4) gameState = State::WATCHING;
            }
            break;

          case PLAYING:

            break;

          case WATCHING:

            break;

          case GAMEOVER:

            break;

          case VICTORY:

            break;
      
        }

        // Execute when gameState changes
        if (previousGameState != gameState) {
          previousGameState = gameState;
          GPC1 = -1;
          GPT1Interval = 1000;
        }
    }

    PT_END(ptDraw);
}

void IRAM_ATTR LEFTCLICK_ISR() {
    if (is_Pushed(debounce_previousTime, debounce_timeInterval, FLAGS) == false) {
    FLAGS |= 0x20; // Set Left Flag 0010
    Serial.println("Left ISR");
  }
}

void IRAM_ATTR RIGHTCLICK_ISR() {
    if (is_Pushed(debounce_previousTime, debounce_timeInterval, FLAGS) == false) {
    FLAGS |= 0x10; // Set Right Flag 0001
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
  show_menu(0, difficulty);
  
  attachInterrupt(digitalPinToInterrupt(D5), LEFTCLICK_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(D3), RIGHTCLICK_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(RX), GREENLED_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(D8), REDLED_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(D7), BLUELED_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(D6), YELLOWLED_ISR, FALLING);
}

void loop() {
  blinking_LED(&ptBlink);
  draw(&ptDraw);

  if (gameState == MENU) {
    menu();
  }

  if (gameState == START) {
    start();
  }


}


void menu() {

  if (confirmation == 0) {
    // RIGHT
    if (FLAGS == 0x10) { difficulty++;}  
    // LEFT
    else if (FLAGS == 0x20) { difficulty--; }
    else if (FLAGS != 0x00) { confirmation = 1; } // If player hit any other button

    // Min and Max Control
    if (difficulty <= minDifficulty) {difficulty = minDifficulty; }
    else if (difficulty >= maxDifficulty) {difficulty = maxDifficulty; }
  }
  else if (confirmation == 1) {
    if (FLAGS == 0x20 | FLAGS == 0x10) { confirmation = 0; }
    else if (FLAGS != 0x00) {gameState = State::START; } // If player hit any other button
    
  }

  FLAGS = 0x00; // Reset Pressed Buttons
}

// Initializes and Resets all game elements
void start() {
  FLAGS = 0x00; // Reset Pressed Buttons
}
