#include "LCD.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup_LCD() {
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
}

void show_splash() {
  display.clearDisplay();
  display.setTextColor(WHITE);

  drawCentreString("SIMON", 64, 0, 2);

  drawCentreString("SAYS", 64, 20, 3);

  drawCentreString("Made By: Marcus Fu", 64, 55);
  
  display.display();
}

void show_menu(int screen, int difficulty) {
  display.clearDisplay();
  display.setTextColor(WHITE);

  drawCentreString("SIMON", 64, 8, 1);
  drawCentreString("SAYS", 64, 17, 1);

  // Line Code
  int width = 20;
  int height = 13;
  display.drawLine(64-width, 29, 64+width, 29, WHITE);
  display.drawLine(64-width, 15-height, 64-width, 15+height, WHITE);
  display.drawLine(64+width, 15-height, 64+width, 15+height, WHITE);
  display.drawLine(64-width, 2, 64+width, 2, WHITE);

  if (screen == 0) {
    drawCentreString("Difficulty", 64, 40, 1);
    drawEquilateralTriangle(48, 58, 5, 270, true);
    drawEquilateralTriangle(80, 58, 5, 90, true);
    drawCentreString(std::to_string(difficulty).c_str(), 64, 55, 1);
  } 
  else if (screen == 1) {
    drawCentreString("Are you ready?", 64, 40, 1);
  }


  display.display();
}

void show_start(int screen) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  int x = 64;
  int y = 20;
  int size = 3.5;
  if (screen == 0) drawCentreString("3", x, y, size);
  else if (screen == 1) drawCentreString("2", x, y, size);
  else if (screen == 2) drawCentreString("1", x, y, size);
  else if (screen == 3) drawCentreString("BEGIN", x, y, size);

  display.display();
}

void show_watch(int screen) {
  display.clearDisplay();
  display.setTextColor(WHITE);

  int y = 32;
  int Ytriangle = y - 7;
  int Ytriangle_up = y - 12;

  drawCentreString("WATCH", 64, y, 2);

  if (screen == 0) {
    display.drawTriangle(50, Ytriangle, 64, Ytriangle-8, 78, Ytriangle, WHITE);
  }
  else if (screen == 1) {
    display.fillTriangle(50, Ytriangle, 64, Ytriangle-8, 78, Ytriangle, WHITE);
  }
  
  display.display();
}

void drawCentreString(const char *buf, int x, int y, float size) {
    int16_t x1, y1;
    uint16_t w, h;

    display.setTextSize(size); // Needs to go first
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h); // Calculate width of new string
    display.setCursor(x - w / 2, y); // Set cursor position
    display.println(buf); // Print the string
}

void drawText(const char *buf, int x, int y, float size) {
  display.setTextSize(size);
  display.setCursor(x, y);
  display.println(buf);
}

void drawEquilateralTriangle(int x, int y, int length, float angle, bool filled) {

  angle *= PI/180;

  // Calculate the coordinates of the vertices
  int x1 = x - length / 2;
  int y1 = y - sqrt(3) / 6 * length;
  int x2 = x + length / 2;
  int y2 = y1;
  int x3 = x;
  int y3 = y + sqrt(3) / 3 * length;

  // Apply the rotation matrix to each vertex
  int x1r = x + (x1 - x) * cos(angle) - (y1 - y) * sin(angle);
  int y1r = y + (x1 - x) * sin(angle) + (y1 - y) * cos(angle);
  int x2r = x + (x2 - x) * cos(angle) - (y2 - y) * sin(angle);
  int y2r = y + (x2 - x) * sin(angle) + (y2 - y) * cos(angle);
  int x3r = x + (x3 - x) * cos(angle) - (y3 - y) * sin(angle);
  int y3r = y + (x3 - x) * sin(angle) + (y3 - y) * cos(angle);

  // Draw the triangle
  if (filled)
    display.fillTriangle(x1r, y1r, x2r, y2r, x3r, y3r, WHITE);
  else
    display.drawTriangle(x1r, y1r, x2r, y2r, x3r, y3r, WHITE);
}

void animateText(const char *buf, int startX, int startY, int endX, int endY, float startSize, float endSize, int steps) {
    for (int i = 0; i <= steps; i++) {
        float progress = (float)i / steps; // Calculate the progress of the animation
        float size = startSize + progress * (endSize - startSize); // Calculate the current size
        int x = startX + progress * (endX - startX); // Calculate the current x-coordinate
        int y = startY + progress * (endY - startY); // Calculate the current y-coordinate

        display.clearDisplay(); // Clear the display
        drawCentreString(buf, x, y, size); // Draw the text at the current position and size
        display.display(); // Update the display

        delay(100); // Wait for a short time before the next frame
    }
}
