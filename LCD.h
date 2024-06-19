// LCD.h
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string>

#ifndef LCD
#define LCD

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#endif

void setup_LCD();

void show_splash();

void show_menu(int screen, int difficulty);

void show_start(int screen);

void drawCentreString(const char *buf, int x, int y, float size=1.0);

void drawText(const char *buf, int x, int y, float size=1.0);

void drawEquilateralTriangle(int x, int y, int length, float angle, bool filled=false);

void animateText(const char *buf, int startX, int startY, int endX, int endY, float startSize=1, float endSize=1, int steps=1);
