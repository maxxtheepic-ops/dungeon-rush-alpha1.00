// src/menus/MainMenu.cpp - Fixed version with better state management
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
        return;
    }
    
    // Full redraw only when needed
    if (needsRedraw) {
        drawFullMenu();
        needsRedraw = false;
        lastRenderedSelection = selectedOption; // FIXED: Update tracking immediately
    }
    // Partial update when only selection changes
    else if (selectedOption != lastRenderedSelection) {
        updateMenuSelection();
    }
}

void MainMenu::drawFullMenu() {
    display->clear();
    
    // Draw static elements
    drawTitle();
    drawMenuOptions();
    drawInstructions();
    
    // Draw initial cursor
    drawMenuCursor(selectedOption);
}

void MainMenu::drawTitle() {
    // Game title - centered and large
    display->drawText("ARCANE", 30, 65, TFT_SKYBLUE, 3);
    display->drawText("DUNGEON", 20, 90, TFT_LIGHTGREY, 3);
    
}

void MainMenu::drawMenuOptions() {
    // Menu options centered in the middle area
    int yStart = 160;
    int ySpacing = 30;
    
    for (int i = 0; i < maxOptions; i++) {
        int yPos = yStart + (i * ySpacing);
        int xPos = 0; // Leave space for cursor (was 35, now 50 for cursor space)
        
        // Draw menu option text (no highlighting here)
        display->drawText(menuOptions[i], xPos, yPos, TFT_WHITE, 2);
    }
}

void MainMenu::drawMenuCursor(int option) {
    int yStart = 160;
    int ySpacing = 30;
    int yPos = yStart + (option * ySpacing);
    // Draw yellow cursor arrow
    display->drawText(">", 130, yPos, TFT_WHITE, 2);
}

void MainMenu::clearMenuCursor(int option) {
    int yStart = 160;
    int ySpacing = 30;
    int yPos = yStart + (option * ySpacing);
    
    // Clear cursor area (slightly wider to ensure clean erase)
    display->fillRect(130, yPos, 18, 16, TFT_BLACK);
}

void MainMenu::updateMenuSelection() {
    // Clear old cursor
    if (lastRenderedSelection >= 0 && lastRenderedSelection < maxOptions) {
        clearMenuCursor(lastRenderedSelection);
    }
    
    // Draw new cursor
    drawMenuCursor(selectedOption);
    
    // Update tracking
    lastRenderedSelection = selectedOption;
}

void MainMenu::drawInstructions() {
    // Instructions at bottom
    int instructY = 280;
    display->drawText("UP/DOWN: Navigate", 0, instructY, TFT_WHITE, 1);
    instructY += 15;
    display->drawText("A: Select", 0, instructY, TFT_WHITE, 1);
}

MenuResult MainMenu::handleInput() {
    if (!isActive) {
        return MenuResult::NONE;
    }
    
    // FIXED: Clear any corrupted selection state at the start
    if (selectionMade != -1) {
        Serial.println("WARNING: MainMenu starting with unexpected selectionMade: " + String(selectionMade));
        selectionMade = -1; // Clear it
    }
    
    // Handle navigation
    if (input->wasPressed(Button::UP)) {
        Serial.println("DEBUG: MainMenu - UP button pressed");
        moveSelectionUp();
        // Note: render() will handle the cursor update automatically
        return MenuResult::NONE;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        Serial.println("DEBUG: MainMenu - DOWN button pressed");
        moveSelectionDown();
        // Note: render() will handle the cursor update automatically
        return MenuResult::NONE;
    }
    
    // Handle selection - FIXED: Add extra validation
    if (input->wasPressed(Button::A)) {
        // FIXED: Validate selection is in valid range
        if (selectedOption >= 0 && selectedOption < maxOptions) {
            Serial.println("DEBUG: MainMenu - A button pressed, making selection: " + String(selectedOption));
            selectionMade = selectedOption;
            return MenuResult::SELECTED;
        } else {
            Serial.println("ERROR: Invalid selection in MainMenu: " + String(selectedOption));
            selectedOption = 0; // Reset to safe value
            return MenuResult::NONE;
        }
    }
    
    // No input detected
    return MenuResult::NONE;
}

MainMenuOption MainMenu::getSelectedOption() const {
    // FIXED: Add validation
    if (selectionMade >= 0 && selectionMade < maxOptions) {
        Serial.println("DEBUG: MainMenu::getSelectedOption() returning: " + String(selectionMade));
        return static_cast<MainMenuOption>(selectionMade);
    } else {
        Serial.println("ERROR: Invalid selectionMade in MainMenu: " + String(selectionMade));
        return MainMenuOption::START_GAME; // Safe default
    }
}