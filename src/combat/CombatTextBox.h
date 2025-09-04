#ifndef COMBAT_TEXTBOX_H
#define COMBAT_TEXTBOX_H

#include "../graphics/Display.h"
#include <vector>
#include <Arduino.h>

class CombatTextBox {
private:
    Display* display;
    
    // Text area bounds
    int textX, textY, textWidth, textHeight;
    
    // Text storage
    std::vector<String> textLines;
    static const int MAX_LINES = 3;     // 3 lines fit in the compact text area
    static const int MAX_CHARS_PER_LINE = 30;  // Wider text area can fit more characters
    
    // Display state
    bool needsRedraw;
    bool isVisible;  // NEW: Control visibility
    
    // Helper methods
    void wrapText(String text, std::vector<String>& wrappedLines);
    void scrollText();
    void drawTextArea();
    void clearTextArea();  // NEW: Method to clear the text area
    
public:
    CombatTextBox(Display* disp);
    
    // Setup
    void setTextArea(int x, int y, int width, int height);
    
    // Text management
    void addText(String text);
    void addText(String text, uint16_t color);
    void clearText();
    
    // Display control
    void render();
    void forceRedraw() { needsRedraw = true; }
    void hide();      // NEW: Hide the text box
    void show();      // NEW: Show the text box
    bool getIsVisible() const { return isVisible; }  // NEW: Check visibility
    
    // Combat-specific convenience methods
    void showPlayerAction(String playerName, String action);
    void showEnemyAction(String enemyName, String action);
    void showDamage(String target, int damage);
    void showHealing(String target, int healing);
    void showSpellCast(String caster, String spellName);
    void showTurnResult(String result);
};

#endif