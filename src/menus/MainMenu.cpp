#include "MainMenu.h"

MainMenu::MainMenu(Display* disp, Input* inp) : MenuBase(disp, inp, 3) {
    // 3 options: Start Game, Settings, Credits
    lastRenderedSelection = -1;
    needsRedraw = true;
}

void MainMenu::activate() {
    Serial.println("DEBUG: MainMenu::activate() called");
    MenuBase::activate();
    lastRenderedSelection = -1;
    needsRedraw = true;
}

void MainMenu::render() {
    if (!isActive) {
        Serial.println("DEBUG: MainMenu::render() called but menu is not active");
        return;
    }
    
    // Always redraw if the selection has changed OR if we need a redraw
    if (selectedOption != lastRenderedSelection || needsRedraw) {
        Serial.println("DEBUG: MainMenu::render() - redrawing (selection: " + String(selectedOption) + ", last: " + String(lastRenderedSelection) + ")");
        display->clear();
        drawTitle();
        drawMenuOptions();
        lastRenderedSelection = selectedOption;
        needsRedraw = false;
    } else {
        Serial.println("DEBUG: MainMenu::render() - no redraw needed (selection: " + String(selectedOption) + ")");
    }
}

void MainMenu::drawTitle() {
    // Game title - centered and large
    display->drawText("ESP32", 50, 40, TFT_CYAN, 2);
    display->drawText("DUNGEON", 30, 65, TFT_CYAN, 2);
    display->drawText("CRAWLER", 25, 90, TFT_CYAN, 2);
    
    // Version or subtitle
    display->drawText("v0.1.0", 60, 120, TFT_WHITE, 1);
}

void MainMenu::drawMenuOptions() {
    // Menu options centered in the middle area
    int yStart = 160;
    int ySpacing = 30;
    
    for (int i = 0; i < maxOptions; i++) {
        int yPos = yStart + (i * ySpacing);
        int xPos = 35; // Centered for longest option "Start Game"
        
        // Highlight selected option
        if (i == selectedOption) {
            // Draw selection background
            display->fillRect(xPos - 5, yPos - 3, 100, 20, TFT_BLUE);
            display->drawText(menuOptions[i], xPos, yPos, TFT_WHITE, 2);
        } else {
            // Draw unselected option
            display->drawText(menuOptions[i], xPos, yPos, TFT_WHITE, 2);
        }
    }
    
    // Instructions at bottom
    int instructY = 280;
    display->drawText("UP/DOWN: Navigate", 15, instructY, TFT_YELLOW, 1);
    instructY += 15;
    display->drawText("A: Select", 55, instructY, TFT_YELLOW, 1);
}

MenuResult MainMenu::handleInput() {
    if (!isActive) {
        Serial.println("DEBUG: MainMenu::handleInput() called but menu is not active");
        return MenuResult::NONE;
    }
    
    // Check for corrupted selection state
    if (selectionMade != -1) {
        Serial.println("WARNING: MainMenu starting with unexpected selectionMade: " + String(selectionMade));
        selectionMade = -1; // Clear it
    }
    
    // Handle navigation
    if (input->wasPressed(Button::UP)) {
        Serial.println("DEBUG: MainMenu - UP button pressed");
        moveSelectionUp();
        return MenuResult::NONE;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        Serial.println("DEBUG: MainMenu - DOWN button pressed");
        moveSelectionDown();
        return MenuResult::NONE;
    }
    
    // Handle selection
    if (input->wasPressed(Button::A)) {
        Serial.println("DEBUG: MainMenu - A button pressed, making selection: " + String(selectedOption));
        selectionMade = selectedOption;
        return MenuResult::SELECTED;
    }
    
    // No input detected
    return MenuResult::NONE;
}

MainMenuOption MainMenu::getSelectedOption() const {
    Serial.println("DEBUG: MainMenu::getSelectedOption() returning: " + String(selectionMade));
    return static_cast<MainMenuOption>(selectionMade);
}