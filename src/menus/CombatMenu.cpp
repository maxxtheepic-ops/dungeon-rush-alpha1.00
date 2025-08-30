#include "CombatMenu.h"

CombatMenu::CombatMenu(Display* disp, Input* inp) : MenuBase(disp, inp, 3) {
    lastRenderedSelection = -1;
    needsRedraw = true;
}

void CombatMenu::activate() {
    MenuBase::activate();
    lastRenderedSelection = -1;
    needsRedraw = true;
}

void CombatMenu::render() {
    if (!isActive) return;
    
    // Only redraw if selection changed or forced redraw
    if (selectedOption != lastRenderedSelection || needsRedraw) {
        drawMenuArea();
        lastRenderedSelection = selectedOption;
        needsRedraw = false;
    }
}

void CombatMenu::clearMenuArea() {
    // Clear only the bottom menu area (reserve top for sprites)
    display->fillRect(0, 250, Display::WIDTH, 70, TFT_BLACK);
}

void CombatMenu::drawMenuArea() {
    clearMenuArea();
    
    // Menu positioned at bottom of screen
    int yStart = 260;
    int xSpacing = 50;
    
    for (int i = 0; i < maxOptions; i++) {
        int xPos = 10 + (i * xSpacing);
        
        // Highlight selected option
        if (i == selectedOption) {
            // Draw selection box
            display->fillRect(xPos - 2, yStart - 2, 44, 20, TFT_YELLOW);
            display->drawText(menuOptions[i], xPos, yStart, TFT_BLACK, 1);
        } else {
            // Draw unselected option
            display->drawText(menuOptions[i], xPos, yStart, TFT_WHITE, 1);
        }
    }
}

MenuResult CombatMenu::handleInput() {
    if (!isActive) return MenuResult::NONE;
    
    // Handle navigation
    if (input->wasPressed(Button::UP)) {
        moveSelectionUp();
        return MenuResult::NONE;  // Will trigger redraw on next render()
    }
    
    if (input->wasPressed(Button::DOWN)) {
        moveSelectionDown();
        return MenuResult::NONE;  // Will trigger redraw on next render()
    }
    
    // Handle selection
    if (input->wasPressed(Button::A)) {
        selectionMade = selectedOption;
        return MenuResult::SELECTED;
    }
    
    // Handle cancel
    if (input->wasPressed(Button::B)) {
        return MenuResult::CANCELLED;
    }
    
    return MenuResult::NONE;
}

CombatAction CombatMenu::getSelectedAction() const {
    return static_cast<CombatAction>(selectionMade);
}