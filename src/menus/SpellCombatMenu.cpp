// src/menus/SpellCombatMenu.cpp - Final clean version without drawPlayerStatus
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
    
    // Draw only spell slots (no player status)
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
    // Clear the bottom combat menu area
    display->fillRect(0, 200, Display::WIDTH, 120, TFT_BLACK);
}

void SpellCombatMenu::drawSpellSlots() {
    // Menu positioned higher up (no player status needed)
    int menuStartY = 210;
    
    // Layout: 2x2 grid for spells
    int slotWidth = 70;
    int slotHeight = 30;
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
    
    // Draw border and background
    display->drawRect(x, y, 70, 30, borderColor);
    display->fillRect(x + 1, y + 1, 68, 28, TFT_BLACK);
    
    // Slot number
    display->drawText(String(slot + 1).c_str(), x + 3, y + 2, TFT_YELLOW, 1);
    
    if (info.name != "Empty") {
        // Spell name (shortened)
        uint16_t textColor = info.available ? info.color : 0x8410;
        display->drawText(info.shortName.c_str(), x + 3, y + 12, textColor, 1);
        
        // Mana cost
        display->drawText(String(info.manaCost).c_str(), x + 50, y + 22, TFT_BLUE, 1);
        
        // Power indicator
        display->drawText(String(info.power).c_str(), x + 3, y + 22, TFT_YELLOW, 1);
    } else {
        display->drawText("Empty", x + 3, y + 15, 0x8410, 1);
    }
}

void SpellCombatMenu::drawMenuCursor(int option) {
    if (option >= 0 && option < 4) {
        // Spell slot cursor
        int row = option / 2;
        int col = option % 2;
        int x = 10 + (col * (70 + 10));
        int y = 210 + (row * (30 + 10));
        
        display->drawText(">", x - 8, y + 12, TFT_YELLOW);
    }
}

void SpellCombatMenu::clearMenuCursor(int option) {
    if (option >= 0 && option < 4) {
        // Clear spell slot cursor
        int row = option / 2;
        int col = option % 2;
        int x = 10 + (col * (70 + 10));
        int y = 210 + (row * (30 + 10));
        
        display->fillRect(x - 8, y + 12, 8, 8, TFT_BLACK);
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