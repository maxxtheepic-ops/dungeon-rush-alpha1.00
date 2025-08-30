#include "Display.h"

Display::Display() {
    // TFT_eSPI constructor handles initialization
}

void Display::init() {
    // Initialize backlight (INVERTED - LOW = ON!)
    pinMode(TFT_BL, OUTPUT);
    setBacklight(true);
    
    // Initialize display
    tft.init();
    tft.setRotation(2);
    clear();
}

void Display::clear() {
    tft.fillScreen(TFT_BLACK);
}

void Display::setBacklight(bool on) {
    // Backlight is inverted - LOW = ON
    digitalWrite(TFT_BL, on ? LOW : HIGH);
}

void Display::drawPixel(int x, int y, uint16_t color) {
    tft.drawPixel(x, y, color);
}

void Display::drawRect(int x, int y, int w, int h, uint16_t color) {
    tft.drawRect(x, y, w, h, color);
}

void Display::fillRect(int x, int y, int w, int h, uint16_t color) {
    tft.fillRect(x, y, w, h, color);
}

void Display::drawText(const char* text, int x, int y, uint16_t color) {
    drawText(text, x, y, color, 1);
}

void Display::drawText(const char* text, int x, int y, uint16_t color, uint8_t size) {
    tft.setTextColor(color, TFT_BLACK);
    tft.setTextSize(size);
    tft.setCursor(x, y);
    tft.print(text);
}

void Display::drawSprite(const uint8_t* spriteData, int x, int y, int w, int h) {
    // This is a placeholder for future sprite implementation
    // You'll implement this when you add sprite support
    fillRect(x, y, w, h, TFT_WHITE); // Temporary placeholder
}