#include "CombatTextBox.h"

CombatTextBox::CombatTextBox(Display* disp) {
    display = disp;
    textX = 10;
    textY = 210;
    textWidth = 150;
    textHeight = 40;
    needsRedraw = true;
    textLines.clear();
}

void CombatTextBox::setTextArea(int x, int y, int width, int height) {
    textX = x;
    textY = y;
    textWidth = width;
    textHeight = height;
    needsRedraw = true;
}

void CombatTextBox::addText(String text) {
    addText(text, TFT_WHITE);
}

void CombatTextBox::addText(String text, uint16_t color) {
    // For now, ignore color (we can enhance this later)
    // Wrap text to fit the display width
    std::vector<String> wrappedLines;
    wrapText(text, wrappedLines);
    
    // Add wrapped lines to our text buffer
    for (String line : wrappedLines) {
        textLines.push_back(line);
        
        // If we have too many lines, remove the oldest
        if (textLines.size() > MAX_LINES) {
            textLines.erase(textLines.begin());
        }
    }
    
    needsRedraw = true;
}

void CombatTextBox::wrapText(String text, std::vector<String>& wrappedLines) {
    if (text.length() <= MAX_CHARS_PER_LINE) {
        // Text fits on one line
        wrappedLines.push_back(text);
        return;
    }
    
    // Text needs to be wrapped
    int start = 0;
    while (start < text.length()) {
        int end = start + MAX_CHARS_PER_LINE;
        
        if (end >= text.length()) {
            // Last piece fits
            wrappedLines.push_back(text.substring(start));
            break;
        }
        
        // Try to find a good break point (space)
        int breakPoint = end;
        for (int i = end; i > start; i--) {
            if (text.charAt(i) == ' ') {
                breakPoint = i;
                break;
            }
        }
        
        // Add the line and continue
        wrappedLines.push_back(text.substring(start, breakPoint));
        start = (breakPoint == end) ? end : breakPoint + 1; // Skip the space
    }
}

void CombatTextBox::clearText() {
    textLines.clear();
    needsRedraw = true;
}

void CombatTextBox::render() {
    if (!needsRedraw) return;
    
    drawTextArea();
    needsRedraw = false;
}

void CombatTextBox::drawTextArea() {
    // AGGRESSIVE CLEAR: Clear a larger area to eliminate any persistent text
    display->fillRect(0, 200, Display::WIDTH, 70, TFT_BLACK);  // Clear from y=200 to y=270
    
    // Draw border
    display->drawRect(textX, textY, textWidth, textHeight, TFT_WHITE);
    
    // Draw text lines
    int lineHeight = 12;  // Height per line
    int padding = 2;      // Padding from border
    
    for (int i = 0; i < textLines.size(); i++) {
        int yPos = textY + padding + (i * lineHeight);
        
        // Make sure we don't draw outside the text area
        if (yPos + lineHeight > textY + textHeight) break;
        
        display->drawText(textLines[i].c_str(), textX + padding, yPos, TFT_WHITE, 1);
    }
}

// Combat-specific convenience methods
void CombatTextBox::showPlayerAction(String playerName, String action) {
    addText(playerName + " " + action);
}

void CombatTextBox::showEnemyAction(String enemyName, String action) {
    addText(enemyName + " " + action);
}

void CombatTextBox::showDamage(String target, int damage) {
    addText(target + " takes " + String(damage) + " damage!");
}

void CombatTextBox::showHealing(String target, int healing) {
    addText(target + " heals " + String(healing) + " HP!");
}

void CombatTextBox::showSpellCast(String caster, String spellName) {
    addText(caster + " casts " + spellName + "!");
}

void CombatTextBox::showTurnResult(String result) {
    addText("= " + result + " =");
}