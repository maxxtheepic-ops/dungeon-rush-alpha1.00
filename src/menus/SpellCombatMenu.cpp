#include "SpellCombatMenu.h"
#include "../spells/spell.h"
#include "../entities/player.h"

SpellCombatMenu::SpellCombatMenu(Display* disp, Input* inp, Player* p) 
    : MenuBase(disp, inp, 5) {  // 4 spell slots + defend
    player = p;
    lastRenderedSelection = -1;
    needsRedraw = true;
    
    // Initialize spell info with proper struct construction
    for (int i = 0; i < 4; i++) {
        spellInfo[i].name = "Empty";
        spellInfo[i].shortName = "----";
        spellInfo[i].color = 0x8410;  // Gray color (TFT_GRAY equivalent)
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
    
    // Only redraw if selection changed or forced redraw
    if (selectedOption != lastRenderedSelection || needsRedraw) {
        drawMenuArea();
        lastRenderedSelection = selectedOption;
        needsRedraw = false;
    }
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
            // Set struct members individually
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

void SpellCombatMenu::drawMenuArea() {
    clearMenuArea();
    
    // Draw player status first (top of menu area)
    drawPlayerStatus();
    
    // Menu positioned in bottom area
    int menuStartY = 230;
    
    // Layout: 2x2 grid for spells + defend button below
    int slotWidth = 70;
    int slotHeight = 30;
    int spacing = 10;
    
    // Spell slots in 2x2 grid
    for (int i = 0; i < 4; i++) {
        int row = i / 2;
        int col = i % 2;
        int x = 10 + (col * (slotWidth + spacing));
        int y = menuStartY + (row * (slotHeight + spacing));
        
        drawSpellSlot(i, x, y, selectedOption == i);
    }
    
    // Defend button below spells
    int defendX = 35;
    int defendY = menuStartY + 2 * (slotHeight + spacing);
    drawDefendOption(defendX, defendY, selectedOption == 4);
}

void SpellCombatMenu::drawPlayerStatus() {
    int statusY = 205;
    
    // HP and Mana on same line
    display->drawText(("HP:" + String(player->getCurrentHP())).c_str(), 
                     10, statusY, TFT_RED, 1);
    display->drawText(("MP:" + String(player->getCurrentMana())).c_str(), 
                     70, statusY, TFT_BLUE, 1);
    
    // Show recent spell synergies if any
    auto recentCasts = player->getSpellLibrary()->getRecentCasts();
    if (!recentCasts.empty()) {
        display->drawText("Synergy Active", 110, statusY, TFT_PURPLE, 1);
    }
}

void SpellCombatMenu::drawSpellSlot(int slot, int x, int y, bool selected) {
    SpellDisplayInfo& info = spellInfo[slot];
    
    // Draw slot background
    uint16_t bgColor = TFT_BLACK;
    uint16_t borderColor = TFT_WHITE;
    
    if (selected) {
        bgColor = TFT_BLUE;
        borderColor = TFT_YELLOW;
    } else if (!info.available && info.name != "Empty") {
        borderColor = TFT_RED; // Not enough mana
    }
    
    // Draw border and background
    display->drawRect(x, y, 70, 30, borderColor);
    display->fillRect(x + 1, y + 1, 68, 28, bgColor);
    
    // Slot number
    display->drawText(String(slot + 1).c_str(), x + 3, y + 2, TFT_YELLOW, 1);
    
    if (info.name != "Empty") {
        // Spell name (shortened)
        uint16_t textColor = info.available ? info.color : 0x8410;  // Use gray instead of TFT_GRAY
        display->drawText(info.shortName.c_str(), x + 3, y + 12, textColor, 1);
        
        // Mana cost
        display->drawText(String(info.manaCost).c_str(), x + 50, y + 22, TFT_BLUE, 1);
        
        // Power indicator
        display->drawText(String(info.power).c_str(), x + 3, y + 22, TFT_YELLOW, 1);
    } else {
        display->drawText("Empty", x + 3, y + 15, 0x8410, 1);  // Use gray instead of TFT_GRAY
    }
    
    // Selection indicator
    if (selected) {
        display->drawText(">", x - 8, y + 12, TFT_YELLOW);
    }
}

void SpellCombatMenu::drawDefendOption(int x, int y, bool selected) {
    uint16_t bgColor = selected ? TFT_BLUE : TFT_BLACK;
    uint16_t borderColor = selected ? TFT_YELLOW : TFT_WHITE;
    
    // Draw defend option
    display->drawRect(x, y, 100, 20, borderColor);
    display->fillRect(x + 1, y + 1, 98, 18, bgColor);
    
    display->drawText("5: DEFEND", x + 5, y + 5, TFT_WHITE);
    
    if (selected) {
        display->drawText(">", x - 8, y + 5, TFT_YELLOW);
    }
}

MenuResult SpellCombatMenu::handleInput() {
    if (!isActive) return MenuResult::NONE;
    
    // Handle navigation - 2x2 grid for spells + defend
    if (input->wasPressed(Button::UP)) {
        if (selectedOption == 4) {
            // From defend to bottom row of spells
            selectedOption = 2;
        } else if (selectedOption >= 2) {
            // From bottom row to top row
            selectedOption -= 2;
        } else {
            // From top row, wrap to defend
            selectedOption = 4;
        }
        return MenuResult::NONE;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        if (selectedOption >= 2 && selectedOption < 4) {
            // From bottom row to defend
            selectedOption = 4;
        } else if (selectedOption < 2) {
            // From top row to bottom row
            selectedOption += 2;
        } else {
            // From defend to top row
            selectedOption = 0;
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
    if (selectedOption == 4) {
        return true; // Defend is always valid
    }
    
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