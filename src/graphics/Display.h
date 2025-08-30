#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>

// Display configuration
#define SCREEN_WIDTH 170
#define SCREEN_HEIGHT 320
#define TFT_BL 14

class Display {
private:
    TFT_eSPI tft;
    
public:
    Display();
    void init();
    void clear();
    void setBacklight(bool on);
    
    // Basic drawing functions
    void drawPixel(int x, int y, uint16_t color);
    void drawRect(int x, int y, int w, int h, uint16_t color);
    void fillRect(int x, int y, int w, int h, uint16_t color);
    
    // Text functions
    void drawText(const char* text, int x, int y, uint16_t color);
    void drawText(const char* text, int x, int y, uint16_t color, uint8_t size);
    
    // Sprite functions (for future use)
    void drawSprite(const uint8_t* spriteData, int x, int y, int w, int h);
    
    // Get TFT instance for advanced operations
    TFT_eSPI& getTFT() { return tft; }
    
    // Screen dimensions
    static const int WIDTH = SCREEN_WIDTH;
    static const int HEIGHT = SCREEN_HEIGHT;
};

#endif