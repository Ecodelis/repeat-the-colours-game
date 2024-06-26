// G = 0
// R = 1
// B = 2
// Y = 3

// ESP12E_DFINES.h
#ifndef game
#define game

// Time
#define watch_Time 100
#define debounce_Time 200
#define doublePress_Threshold 100 // should always be below debounce time

// Game Logic
#define num_Patterns 5
#define maxDifficulty 20
#define minDifficulty 1

// LED ARRAY
#define num_LEDS 4
#define GREEN_LED 0
#define RED_LED 1
#define BLUE_LED 2
#define YELLOW_LED 3

// DR ARRAY
#define DR0 0
#define DR1 1
#define DR2 2
#define DR3 3
#define DR4 4
#define DR5 5
#define DR6 6
#define DR7 7
#define DR8 8

enum State {
  MENU,
  START,
  WATCHING,
  REPEAT,
  GAMEOVER,
  VICTORY
};

#endif