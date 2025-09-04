// src/menus/SpellCombatMenu.cpp - Moved spells to bottom for text box space
#include "SpellCombatMenu.h"
#include "../spells/spell.h"
#include "../entities/player.h"

SpellCombatMenu::SpellCombatMenu(Display* disp, Input* inp, Player* p) 
    : MenuBase(disp, inp, 4) {  // 4 spell slots only
    player = p;
    lastRenderedSelection = -1;
    needsRedraw = true;
    
    // Initialize spell info with proper struct construction
    for (int i = 0; i < 4; i++) {
        spellInfo[i].name = "Empty";
        spellInfo[i].shortName = "----";
        spellInfo[i].color = 0x8410;  // Gray color
        spellInfo[i].available = false;
        spellInfo[i].manaCost = 0;
        spellInfo[i].power = 0;
    }
}

void SpellCombatMenu::activate() {
    MenuBase::activate();
    updateSpellInfo();
    lastRenderedSelection = -1;
    needsRedraw = true;
}

void SpellCombatMenu::render() {
    if (!isActive) return;
    
    // Update spell info in case spells changed
    updateSpellInfo();
    
    // Full redraw when needed
    if (needsRedraw) {
        drawFullMenu();
        needsRedraw = false;
    }
    // Partial update when only selection changes
    else if (selectedOption != lastRenderedSelection) {
        updateMenuSelection();
    }
}

void SpellCombatMenu::drawFullMenu() {
    clearMenuArea();
    
    // Draw only spell slots at the bottom
    drawSpellSlots();
    
    // Draw initial cursor
    drawMenuCursor(selectedOption);
    
    lastRenderedSelection = selectedOption;
}

void SpellCombatMenu::updateSpellInfo() {
    auto equippedSpells = player->getEquippedSpells();
    
    for (int i = 0; i < 4; i++) {
        if (i < equippedSpells.size() && equippedSpells[i]) {
            Spell* spell = equippedSpells[i];
            spellInfo[i].name = spell->getName();
            spellInfo[i].shortName = spell->getName().length() > 8 ? 
                                   spell->getName().substring(0, 8) : spell->getName();
            spellInfo[i].color = spell->getElementColor();
            spellInfo[i].available = (player->getCurrentMana() >= spell->getManaCost());
            spellInfo[i].manaCost = spell->getManaCost();
            spellInfo[i].power = spell->getBasePower();
        } else {
            spellInfo[i].name = "Empty";
            spellInfo[i].shortName = "----";
            spellInfo[i].color = 0x8410;  // Gray color
            spellInfo[i].available = false;
            spellInfo[i].manaCost = 0;
            spellInfo[i].power = 0;
        }
    }
}

void SpellCombatMenu::clearMenuArea() {
    // Clear only the bottom spell menu area (reserve space above for text box)
    display->fillRect(0, 260, Display::WIDTH, 60, TFT_BLACK);
}

void SpellCombatMenu::drawSpellSlots() {
    // Menu positioned at the very bottom (moved down from 210)
    int menuStartY = 260;  // CHANGED: Moved down 50 pixels to make room for text
    
    // Layout: 2x2 grid for spells (slightly smaller to fit)
    int slotWidth = 70;
    int slotHeight = 25;    // CHANGED: Reduced height from 30 to 25
    int spacing = 10;
    
    // Draw spell slots (static part)
    for (int i = 0; i < 4; i++) {
        int row = i / 2;
        int col = i % 2;
        int x = 10 + (col * (slotWidth + spacing));
        int y = menuStartY + (row * (slotHeight + spacing));
        
        drawSpellSlotContent(i, x, y);
    }
}

void SpellCombatMenu::drawSpellSlotContent(int slot, int x, int y) {
    SpellDisplayInfo& info = spellInfo[slot];
    
    // Draw slot background and border (no selection highlighting)
    uint16_t borderColor = TFT_WHITE;
    if (!info.available && info.name != "Empty") {
        borderColor = TFT_RED; // Not enough mana
    }
    
    // Draw border and background (smaller height)
    display->drawRect(x, y, 70, 25, borderColor);  // CHANGED: Height from 30 to 25
    display->fillRect(x + 1, y + 1, 68, 23, TFT_BLACK);  // CHANGED: Height from 28 to 23
    
    // Slot number
    display->drawText(String(slot + 1).c_str(), x + 3, y + 1, TFT_WHITE, 1);  // CHANGED: Moved up 1 pixel
    
    if (info.name != "Empty") {
        // Spell name (shortened) - adjusted position
        uint16_t textColor = info.available ? info.color : 0x8410;
        display->drawText(info.shortName.c_str(), x + 3, y + 10, textColor, 1);  // CHANGED: Moved up 2 pixels
        
        // Mana cost and power on same line to save space
        display->drawText(String(info.manaCost).c_str(), x + 50, y + 18, TFT_BLUE, 1);  // CHANGED: Moved up 4 pixels
        display->drawText(String(info.power).c_str(), x + 3, y + 18, TFT_WHITE, 1);   // CHANGED: Moved up 4 pixels
    } else {
        display->drawText("Empty", x + 3, y + 12, 0x8410, 1);  // CHANGED: Moved up 3 pixels
    }
}

void SpellCombatMenu::drawMenuCursor(int option) {
    if (option >= 0 && option < 4) {
        // Spell slot cursor (adjusted for new positions)
        int row = option / 2;
        int col = option % 2;
        int x = 10 + (col * (70 + 10));
        int y = 260 + (row * (25 + 10));  // CHANGED: Updated y position
        
        display->drawText(">", x - 8, y + 10, TFT_WHITE);  // CHANGED: Adjusted y offset
    }
}

void SpellCombatMenu::clearMenuCursor(int option) {
    if (option >= 0 && option < 4) {
        // Clear spell slot cursor (adjusted for new positions)
        int row = option / 2;
        int col = option % 2;
        int x = 10 + (col * (70 + 10));
        int y = 260 + (row * (25 + 10));  // CHANGED: Updated y position
        
        display->fillRect(x - 8, y + 10, 8, 8, TFT_BLACK);  // CHANGED: Adjusted y offset
    }
}

void SpellCombatMenu::updateMenuSelection() {
    // Clear old cursor
    if (lastRenderedSelection >= 0 && lastRenderedSelection < 4) {
        clearMenuCursor(lastRenderedSelection);
    }
    
    // Draw new cursor
    drawMenuCursor(selectedOption);
    
    lastRenderedSelection = selectedOption;
}

MenuResult SpellCombatMenu::handleInput() {
    if (!isActive) return MenuResult::NONE;
    
    // Handle navigation - Linear navigation through all 4 slots
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) {
            selectedOption = 3; // Wrap to last slot
        }
        return MenuResult::NONE;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption >= 4) {
            selectedOption = 0; // Wrap to first slot
        }
        return MenuResult::NONE;
    }
    
    // Handle selection
    if (input->wasPressed(Button::A)) {
        // Check if selection is valid
        if (!isSelectedActionValid()) {
            // Invalid selection (not enough mana, empty slot, etc.)
            Serial.println("Invalid spell selection!");
            return MenuResult::NONE;
        }
        
        selectionMade = selectedOption;
        return MenuResult::SELECTED;
    }
    
    // Handle cancel
    if (input->wasPressed(Button::B)) {
        return MenuResult::CANCELLED;
    }
    
    return MenuResult::NONE;
}

SpellCombatAction SpellCombatMenu::getSelectedAction() const {
    return static_cast<SpellCombatAction>(selectionMade);
}

bool SpellCombatMenu::isSelectedActionValid() const {
    // Check if spell slot is valid
    if (selectedOption < 0 || selectedOption >= 4) {
        return false;
    }
    
    // Check if spell exists and player has enough mana
    return spellInfo[selectedOption].name != "Empty" && 
           spellInfo[selectedOption].available;
}

int SpellCombatMenu::getSelectedSpellSlot() const {
    if (selectionMade >= 0 && selectionMade < 4) {
        return selectionMade;
    }
    return -1; // Not a spell slot
}

void SpellCombatMenu::refreshSpellData() {
    updateSpellInfo();
    needsRedraw = true;
}

void SpellCombatMenu::getTextAreaBounds(int& x, int& y, int& width, int& height) {
    x = 10;                     // Small margin from edge
    y = 210;                    // Positioned between HUD and spells
    width = Display::WIDTH - 20; // Almost full width with small margins (150 pixels)
    height = 40;                // Compact height for 3 lines
}