#include "protothreads.h"

#include "game.h"
#include "resources.h" // Functions
#include "LCD.h" // LCD Screen

// For an easier way of changing button pins
#define PB1 D3 // Left
#define PB2 D5 // Right

// Blinking LED
int ledState = LOW;
long timeInterval = 1000; // in milliseconds
long previousTime = 0;

// Draw Time
long drawInterval = 200; // in milliseconds
long previousDraw = 0;

// Debouncing and PB Controls
long debounce_previousTime = 0;
volatile uint8_t FLAGS = 0x00; // 4-bit variable [0000_0000]. Used to keep track which button is pressed.
volatile long PB1PressTime = 0;
volatile long PB2PressTime = doublePress_Threshold + 1; // So it doesn't detect simutaneous button pressing at the begining of boot
bool PB1Pressed = false;
bool PB2Pressed = false;


// Game Variables
int confirmation = 0;
int gameState = State::MENU; // Statemachine
int difficulty = 5;
int lightCounter = 0; // Keeps track of which light to display in watch mode and the level that the player is on
int maxLightCounter = 1; // The level that the player or watch must reach to proceed onto the next stage

uint8_t* dynamicPatterns = nullptr; // declare a soon-to-be dynamic pointer


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
        digitalWrite(BUILTIN_LED, ledState);
    }

    PT_END(ptBlink);
}

// GPT-1 General Purpose Timer 1 (for draw)
long GPT1Interval = 1000;
long previousGPT1 = 0;

// GPT-2 General Purpose Timer 2 (for update)
long GPT2Interval = 1000;
long previousGPT2 = 0;

int GPC_1 = 0; // General Purpose Counter 1
int GPC_2 = 0; // General Purpose Counter 2

bool GPS_1 = false; // General Purpose Scene 1

int previousGameState = gameState; // Tracks if a state has been changed in the game

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
              previousGPT1 = millis();

              if (GPC_1 == 0) show_start(0);
              if (GPC_1 == 1) show_start(1);
              if (GPC_1 == 2) show_start(2);
              if (GPC_1 == 3) show_start(3);
              if (GPC_1 == 4) GPS_1 = true;

              GPC_1++;
            }
            break;

          case WATCHING:
            GPT1Interval = 500;
            if (millis() - previousGPT1 > GPT1Interval) {
              previousGPT1 = millis();

              if (GPC_1 == 0) {
                show_watch(0);
                GPC_1 = 1;
              }
              else if (GPC_1 == 1) {
                show_watch(1);
                GPC_1 = 0;
              }
            }
            break;

          case REPEAT:
            GPT1Interval = 500;
            if (millis() - previousGPT1 > GPT1Interval) {
              previousGPT1 = millis();

              if (GPC_1 == 0) {
                show_repeat(0);
                GPC_1 = 1;
              }
              else if (GPC_1 == 1) {
                show_repeat(1);
                GPC_1 = 0;
              }
            }
            break;

          case GAMEOVER:

            break;

          case VICTORY:

            break;
      
        }

        //Serial.println(previousGameState);
    }

    PT_END(ptDraw);
}

// LEFT
void IRAM_ATTR LEFT_ISR() {
  PB1Pressed = true;
  PB1PressTime = millis();
}

// RIGHT
void IRAM_ATTR RIGHT_ISR() {
  PB2Pressed = true;
  PB2PressTime = millis();
}


void setup() {

  // initialize inbuilt LED pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PB1, INPUT_PULLUP); // PB, Left
  pinMode(PB2, INPUT_PULLUP); // PB, Right
  pinMode(D6, OUTPUT); // LED, Left
  pinMode(D7, OUTPUT); // LED, Right

  Serial.begin(115200); // Set the baud rate to 9600 (you may have a different value)
  randomSeed(analogRead(0)); // Init random seed

  // Initialize protothread
  PT_INIT(&ptBlink);
  PT_INIT(&ptDraw);

  // Initialize LCD
  setup_LCD();
  show_splash();
  delay(3000);
  show_menu(0, difficulty);
  
  attachInterrupt(digitalPinToInterrupt(PB1), LEFT_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(PB2), RIGHT_ISR, FALLING);
}

void loop() {
  blinking_LED(&ptBlink);
  draw(&ptDraw);

  // Change gameState at the end of any scene (LCD animations) (theres a better way of doing this)
  if (GPS_1 == true) {
    GPS_1 = false;
    gameState = State::WATCHING;
  }

  // Execute when gameState changes
  if (previousGameState != gameState) {
    previousGameState = gameState;
    GPC_1 = 0;
    GPC_2 = 0;
    GPT1Interval = 1000;// defualt 1s

    //previousGPT1 = millis();
    previousGPT2 = millis();
    Serial.println("DIFF STATE!");
  }

  // Debounce
  if (time_delay(debounce_previousTime, debounce_Time)) {

    // Checks for button presses: Simultaneous or Single
    if(abs(PB1PressTime - PB2PressTime) < doublePress_Threshold) { // Check if the two buttons were pressed within a certain interval of each other
      FLAGS = 0x04;
      Serial.println("SIMU PB!");
    }
    else if (PB1Pressed == true) { // Check if PB1 is pressed, if not, check if PB2 is pressed (individual press)
      FLAGS = 0x01;
      Serial.println("PB1!");
    }
    else if (PB2Pressed == true) {
      FLAGS = 0x02;
      Serial.println("PB2!");
    }

    PB1Pressed = false;
    PB2Pressed = false;

    // Reset time to prevent continuous execution
    PB1PressTime = 0;
    PB2PressTime = doublePress_Threshold + 1;
  }

  switch(gameState) {

    case MENU:
      menu();
      break;

    case START:
      if (GPC_2 == 0) { start(); GPC_2 = 1; } // Execute once
      break;

    case WATCHING:
      if (GPC_2 == 0) { // Execute once and delay LED showcase by 2 second
        if (time_delay(previousGPT2, 2000)) {
          GPC_2 = 1; 
        }
        else { 
          break; 
        }
      }
      watch();
      break;

    case REPEAT:
      repeat();
      break;

  }

  resetPB();
}


void menu() {

  if (confirmation == 0) {
    // RIGHT
    if (FLAGS == 0x02) { difficulty++;}  
    // LEFT
    else if (FLAGS == 0x01) { difficulty--; }
    else if (FLAGS == 0x04) { confirmation = 1; Serial.println("confirmed = 1"); } // If player hit any other button

    // Min and Max Control
    if (difficulty <= minDifficulty) {difficulty = minDifficulty; }
    else if (difficulty >= maxDifficulty) {difficulty = maxDifficulty; }
  }
  else if (confirmation == 1) {
    if (FLAGS == 0x01 || FLAGS == 0x02) { confirmation = 0; Serial.println("confirmed = 0");}
    else if (FLAGS == 0x04) {gameState = State::START; } // If player hit any other button
    
  }
}

// Initializes and Resets all game elements
void start() {
  generatePattern();
}

void watch() {
  GPT2Interval = 1000; // change this to a function that speeds up per light later in dev
  long previousLightOff = previousGPT2 - 300;
  maxLightCounter = difficulty;

  if (time_delay(previousLightOff, GPT2Interval)) { LEDOFF(); } // 300ms before next lightCounter, turn off all lights

  if (time_delay(previousGPT2, GPT2Interval)) { // For every interval, begin a cycle of showing the LED order
    previousGPT2 = millis();

    if (lightCounter < maxLightCounter) { // If its still counting up execute this
      lights(dynamicPatterns[lightCounter]);
      lightCounter++;
      Serial.println("Showing Next Light");
    }

    if (lightCounter == maxLightCounter) { // Reached end of WATCHING
      gameState = State::REPEAT;

      // Reset Values
      lightCounter = 0;
      maxLightCounter = 1;
      Serial.println("End of Watch");
    }


  }
}

void repeat() {

}

void generatePattern()
{
  Serial.println("Generating pattern");
  free(dynamicPatterns);

  dynamicPatterns = (uint8_t*)malloc(difficulty * sizeof(uint8_t)); // Allocate memory

  for (int i = 0; i < difficulty - 1; i++)
  {
    switch (random(0, 3))
    {
      case 0: // LEFT
        dynamicPatterns[i] |= 0x1; Serial.print("L, "); break;
      case 1: // RIGHT
        dynamicPatterns[i] |= 0x2; Serial.print("R, "); break;
      case 2: // CENTER
        dynamicPatterns[i] |= 0x4; Serial.print("C, "); break;
    }

    Serial.println(dynamicPatterns[i]);
  }
}

void resetPB() { // Resets All PB Flags
  FLAGS = 0x00; // Reset all FLAGS
}