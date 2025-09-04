// src/rooms/LibraryRoomState.cpp - Fixed version with better state management
#include "LibraryRoomState.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../dungeon/DungeonManager.h"
#include "../dungeon/Room.h"
#include "../spells/spell.h"

LibraryRoomState::LibraryRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) 
    : GameState(disp, inp) {
    player = p;
    currentEnemy = e;
    dungeonManager = dm;
    gameStateManager = nullptr;
    
    currentScreen = SCREEN_MAIN_MENU;
    selectedOption = 0;
    maxOptions = 4;
    screenDrawn = false;
    lastSelectedOption = -1;
    
    selectedSpellSlot = 0;
    selectedScrollIndex = 0;
    selectedKnownSpell = 0;
    
    availableScrolls.clear();
}

LibraryRoomState::~LibraryRoomState() {
    for (Spell* scroll : availableScrolls) {
        delete scroll;
    }
    availableScrolls.clear();
}

void LibraryRoomState::enter() {
    Serial.println("Entering the mystical library...");
    currentScreen = SCREEN_MAIN_MENU;
    selectedOption = 0;
    screenDrawn = false;
    lastSelectedOption = -1;
    
    // FIXED: Clear any pending state transitions immediately
    clearTransition();
    
    drawMainMenu();
}

void LibraryRoomState::update() {
    // FIXED: Don't process input if we have a pending state transition
    if (getNextState() != StateTransition::NONE) {
        return;
    }
    
    switch (currentScreen) {
        case SCREEN_MAIN_MENU:
            handleMainMenuInput();
            // Only update selection cursor if it changed
            if (currentScreen == SCREEN_MAIN_MENU && selectedOption != lastSelectedOption) {
                updateMainMenuSelection();
            }
            break;
            
        case SCREEN_SCROLL_SELECTION:
            handleScrollSelectionInput();
            if (currentScreen == SCREEN_SCROLL_SELECTION && selectedOption != lastSelectedOption) {
                updateScrollSelection();
            }
            break;
            
        case SCREEN_SPELL_MANAGEMENT:
            handleSpellManagementInput();
            if (currentScreen == SCREEN_SPELL_MANAGEMENT && selectedOption != lastSelectedOption) {
                updateSpellManagementSelection();
            }
            break;
            
        case SCREEN_SPELL_REPLACEMENT:
            handleSpellReplacementInput();
            if (currentScreen == SCREEN_SPELL_REPLACEMENT && selectedOption != lastSelectedOption) {
                updateSpellReplacementSelection();
            }
            break;
            
        case SCREEN_REST_RESULT:
            handleRestResultInput();
            break;
    }
}

void LibraryRoomState::exit() {
    Serial.println("Leaving the library behind...");
}

// ==============================================
// INPUT HANDLERS (unchanged)
// ==============================================

void LibraryRoomState::handleMainMenuInput() {
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) selectedOption = maxOptions - 1;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption >= maxOptions) selectedOption = 0;
    }
    
    if (input->wasPressed(Button::A)) {
        switch (selectedOption) {
            case 0: performRest(); break;
            case 1: if (hasScrolls()) openScrolls(); break;
            case 2: openSpellManagement(); break;
            case 3: completeRoom(); return;
        }
    }
}

void LibraryRoomState::handleScrollSelectionInput() {
    if (availableScrolls.empty()) {
        if (input->wasPressed(Button::B)) {
            returnToMainMenu();
        }
        return;
    }
    
    if (input->wasPressed(Button::UP)) {
        selectedScrollIndex--;
        if (selectedScrollIndex < 0) selectedScrollIndex = availableScrolls.size() - 1;
        selectedOption = selectedScrollIndex; // Keep in sync
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedScrollIndex++;
        if (selectedScrollIndex >= availableScrolls.size()) selectedScrollIndex = 0;
        selectedOption = selectedScrollIndex; // Keep in sync
    }
    
    if (input->wasPressed(Button::A)) {
        readSelectedScroll();
    }
    
    if (input->wasPressed(Button::B)) {
        returnToMainMenu();
    }
}

void LibraryRoomState::handleSpellManagementInput() {
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) selectedOption = 3;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption > 3) selectedOption = 0;
    }
    
    if (input->wasPressed(Button::A)) {
        selectedSpellSlot = selectedOption;
        if (player->getSpellLibrary()->getKnownSpellCount() > 0) {
            currentScreen = SCREEN_SPELL_REPLACEMENT;
            selectedOption = 0;
            lastSelectedOption = -1; // Force redraw
            drawSpellReplacement();
        }
    }
    
    if (input->wasPressed(Button::B)) {
        returnToMainMenu();
    }
}

void LibraryRoomState::handleSpellReplacementInput() {
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) selectedOption = knownSpells.size() - 1;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption >= knownSpells.size()) selectedOption = 0;
    }
    
    if (input->wasPressed(Button::A)) {
        equipSpellToSlot();
    }
    
    if (input->wasPressed(Button::B)) {
        currentScreen = SCREEN_SPELL_MANAGEMENT;
        selectedOption = selectedSpellSlot;
        lastSelectedOption = -1; // Force redraw
        drawSpellManagement();
    }
}

void LibraryRoomState::handleRestResultInput() {
    if (input->wasPressed(Button::UP) || input->wasPressed(Button::DOWN) ||
        input->wasPressed(Button::A) || input->wasPressed(Button::B)) {
        returnToMainMenu();
    }
}

// ==============================================
// OPTIMIZED DRAWING METHODS
// ==============================================

void LibraryRoomState::drawMainMenu() {
    display->clear();
    
    // Title with mystical theme
    display->drawText("ARCANE LIBRARY", 25, 15, TFT_WHITE, 2);
    display->drawText("\"Knowledge is power\"", 15, 35, TFT_WHITE);
    display->drawText("- Ancient Wizard", 25, 50, TFT_WHITE, 1);
    
    // Player status
    display->drawText(("HP: " + String(player->getCurrentHP()) + "/" + String(player->getMaxHP())).c_str(), 
                     10, 70, TFT_WHITE);
    display->drawText(("Mana: " + String(player->getCurrentMana()) + "/" + String(player->getMaxMana())).c_str(), 
                     10, 85, TFT_BLUE);
    display->drawText(("Gold: " + String(player->getGold())).c_str(), 
                     10, 100, TFT_WHITE);
    
    // Show scroll counts by tier
    if (hasScrolls()) {
        int tier1Count = 0, tier2Count = 0, tier3Count = 0;
        for (Spell* scroll : availableScrolls) {
            if (scroll->getBasePower() <= 20) tier1Count++;
            else if (scroll->getBasePower() <= 25) tier2Count++;
            else tier3Count++;
        }
        
        display->drawText("Scrolls Found:", 10, 115, TFT_GREEN);
        if (tier1Count > 0) {
            display->drawText(("T1: " + String(tier1Count)).c_str(), 15, 130, TFT_WHITE);
        }
        if (tier2Count > 0) {
            display->drawText(("T2: " + String(tier2Count)).c_str(), 55, 130, TFT_WHITE);
        }
        if (tier3Count > 0) {
            display->drawText(("T3: " + String(tier3Count)).c_str(), 95, 130, TFT_RED);
        }
    }
    
    // Draw menu options (static part)
    drawMainMenuOptions();
    
    // Draw equipped spells at bottom (static)
    drawEquippedSpellsFooter();
    
    // Draw controls
    display->drawText("UP/DOWN: Navigate, A: Select", 5, 295, TFT_WHITE, 1);
    
    // Draw initial cursor
    drawMainMenuCursor(selectedOption);
    
    screenDrawn = true;
    lastSelectedOption = selectedOption;
}

void LibraryRoomState::drawMainMenuOptions() {
    int yStart = 150;
    int ySpacing = 25;
    const char* options[4] = {"Rest (20g)", "Read Scrolls", "Manage Spells", "Leave"};
    
    for (int i = 0; i < maxOptions; i++) {
        int yPos = yStart + (i * ySpacing);
        String optionText = options[i];
        uint16_t textColor = TFT_WHITE;
        
        // Update option appearance based on availability
        if (i == 0 && player->getGold() < REST_COST) {
            textColor = TFT_RED;
        } else if (i == 1) {
            if (!hasScrolls()) {
                optionText = "Read Scrolls (0)";
                textColor = 0x8410; // Dark grey
            } else {
                optionText = "Read Scrolls (" + String(availableScrolls.size()) + ")";
                textColor = TFT_GREEN;
            }
        }
        
        // Draw option text with proper spacing for cursor
        display->drawText(optionText.c_str(), 30, yPos, textColor);
    }
}

void LibraryRoomState::drawMainMenuCursor(int option) {
    int yStart = 150;
    int ySpacing = 25;
    int yPos = yStart + (option * ySpacing);
    
    display->drawText(">", 15, yPos, TFT_WHITE);
}

void LibraryRoomState::clearMainMenuCursor(int option) {
    int yStart = 150;
    int ySpacing = 25;
    int yPos = yStart + (option * ySpacing);
    
    // Clear cursor area
    display->fillRect(15, yPos, 12, 12, TFT_BLACK);
}

void LibraryRoomState::updateMainMenuSelection() {
    // Clear old cursor
    if (lastSelectedOption >= 0 && lastSelectedOption < maxOptions) {
        clearMainMenuCursor(lastSelectedOption);
    }
    
    // Draw new cursor
    drawMainMenuCursor(selectedOption);
    
    lastSelectedOption = selectedOption;
}

void LibraryRoomState::drawEquippedSpellsFooter() {
    display->drawText("Equipped:", 10, 250, TFT_WHITE);
    auto equippedSpells = player->getEquippedSpells();
    
    for (int i = 0; i < 4; i++) {
        int x = 10 + (i * 35);
        int y = 265;
        
        if (i < equippedSpells.size() && equippedSpells[i]) {
            display->drawText((String(i + 1) + ":").c_str(), x, y, TFT_WHITE, 1);
            String shortName = equippedSpells[i]->getName();
            if (shortName.length() > 4) shortName = shortName.substring(0, 4);
            display->drawText(shortName.c_str(), x, y + 10, 
                            equippedSpells[i]->getElementColor(), 1);
        } else {
            display->drawText((String(i + 1) + ": ---").c_str(), x, y, 0x8410, 1);
        }
    }
}

void LibraryRoomState::drawScrollSelection() {
    display->clear();
    
    display->drawText("ANCIENT SCROLLS", 25, 15, TFT_GREEN, 2);
    
    if (availableScrolls.empty()) {
        display->drawText("No scrolls to read", 25, 100, TFT_RED);
        display->drawText("Find treasure chests", 20, 120, TFT_WHITE);
        display->drawText("or defeat bosses!", 25, 135, TFT_WHITE);
        display->drawText("B: Return", 50, 200, TFT_WHITE);
        screenDrawn = true;
        return;
    }
    
    display->drawText(("You have " + String(availableScrolls.size()) + " scrolls:").c_str(), 10, 40, TFT_WHITE);
    
    // Draw scroll options (static part)
    drawScrollOptions();
    
    // Draw controls
    display->drawText("A: Read Scroll, B: Back", 20, 280, TFT_WHITE);
    
    // Draw initial cursor
    drawScrollCursor(selectedScrollIndex);
    
    screenDrawn = true;
    lastSelectedOption = selectedScrollIndex;
}

void LibraryRoomState::drawScrollOptions() {
    for (int i = 0; i < availableScrolls.size() && i < 6; i++) {
        int yPos = 60 + (i * 35);
        Spell* scroll = availableScrolls[i];
        
        // Show mysterious scroll description
        String scrollDesc = "Mysterious Scroll";
        String tierDesc = "Tier 1";
        uint16_t tierColor = TFT_WHITE;
        
        if (scroll->getBasePower() > 25) {
            tierDesc = "Tier 3 (Powerful)";
            tierColor = TFT_RED;
        } else if (scroll->getBasePower() > 20) {
            tierDesc = "Tier 2 (Advanced)";
            tierColor = TFT_WHITE;
        }
        
        display->drawText(scrollDesc.c_str(), 30, yPos, TFT_WHITE);
        display->drawText(tierDesc.c_str(), 30, yPos + 12, tierColor);
        display->drawText(scroll->getElementName().c_str(), 30, yPos + 24, scroll->getElementColor());
    }
}

void LibraryRoomState::drawScrollCursor(int scrollIndex) {
    if (scrollIndex >= 0 && scrollIndex < availableScrolls.size()) {
        int yPos = 60 + (scrollIndex * 35) + 12; // Center vertically
        display->drawText(">", 15, yPos, TFT_WHITE);
    }
}

void LibraryRoomState::clearScrollCursor(int scrollIndex) {
    if (scrollIndex >= 0 && scrollIndex < availableScrolls.size()) {
        int yPos = 60 + (scrollIndex * 35) + 12;
        display->fillRect(15, yPos, 12, 12, TFT_BLACK);
    }
}

void LibraryRoomState::updateScrollSelection() {
    // Clear old cursor
    if (lastSelectedOption >= 0 && lastSelectedOption < availableScrolls.size()) {
        clearScrollCursor(lastSelectedOption);
    }
    
    // Draw new cursor
    drawScrollCursor(selectedScrollIndex);
    
    lastSelectedOption = selectedScrollIndex;
}

void LibraryRoomState::drawSpellManagement() {
    display->clear();
    
    display->drawText("SPELL GRIMOIRE", 25, 15, TFT_WHITE, 2);
    
    // Show equipped spells (static part)
    display->drawText("Equipped:", 10, 40, TFT_WHITE);
    drawSpellSlots();
    
    // Show known spells summary
    drawKnownSpellsSummary();
    
    // Draw controls
    display->drawText("A: Manage Slot, B: Back", 10, 280, TFT_WHITE);
    
    // Draw initial cursor
    drawSpellSlotCursor(selectedOption);
    
    screenDrawn = true;
    lastSelectedOption = selectedOption;
}

void LibraryRoomState::drawSpellSlots() {
    auto equippedSpells = player->getEquippedSpells();
    
    for (int i = 0; i < 4; i++) {
        int yPos = 55 + (i * 25);
        
        String slotText = "Slot " + String(i + 1) + ": ";
        if (i < equippedSpells.size() && equippedSpells[i]) {
            slotText += equippedSpells[i]->getName();
            display->drawText(slotText.c_str(), 30, yPos, TFT_WHITE);
            display->drawText(equippedSpells[i]->getElementName().c_str(), 
                            120, yPos, equippedSpells[i]->getElementColor());
        } else {
            slotText += "Empty";
            display->drawText(slotText.c_str(), 30, yPos, 0x8410);
        }
    }
}

void LibraryRoomState::drawSpellSlotCursor(int slot) {
    int yPos = 55 + (slot * 25);
    display->drawText(">", 15, yPos, TFT_WHITE);
}

void LibraryRoomState::clearSpellSlotCursor(int slot) {
    int yPos = 55 + (slot * 25);
    display->fillRect(15, yPos, 12, 12, TFT_BLACK);
}

void LibraryRoomState::updateSpellManagementSelection() {
    // Clear old cursor
    if (lastSelectedOption >= 0 && lastSelectedOption < 4) {
        clearSpellSlotCursor(lastSelectedOption);
    }
    
    // Draw new cursor
    drawSpellSlotCursor(selectedOption);
    
    lastSelectedOption = selectedOption;
}

void LibraryRoomState::drawKnownSpellsSummary() {
    display->drawText("Known:", 10, 170, TFT_WHITE);
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    int knownCount = min(3, (int)knownSpells.size());
    
    for (int i = 0; i < knownCount; i++) {
        int yPos = 185 + (i * 15);
        display->drawText(("- " + knownSpells[i]->getName()).c_str(), 15, yPos, TFT_WHITE, 1);
    }
    
    if (knownSpells.size() > 3) {
        display->drawText(("... and " + String(knownSpells.size() - 3) + " more").c_str(), 
                         15, 230, TFT_WHITE, 1);
    }
}

void LibraryRoomState::drawSpellReplacement() {
    display->clear();
    
    display->drawText("EQUIP SPELL", 30, 15, TFT_WHITE, 2);
    display->drawText(("To Slot " + String(selectedSpellSlot + 1)).c_str(), 50, 35, TFT_WHITE);
    
    // Show current spell in slot (static)
    drawCurrentSpellInSlot();
    
    // Show available spells (static)
    display->drawText("Available:", 10, 95, TFT_WHITE);
    drawAvailableSpells();
    
    // Draw controls
    display->drawText("A: Equip, B: Back", 20, 280, TFT_WHITE);
    
    // Draw initial cursor
    drawSpellReplacementCursor(selectedOption);
    
    screenDrawn = true;
    lastSelectedOption = selectedOption;
}

void LibraryRoomState::drawCurrentSpellInSlot() {
    auto equippedSpells = player->getEquippedSpells();
    if (selectedSpellSlot < equippedSpells.size() && equippedSpells[selectedSpellSlot]) {
        display->drawText("Current:", 10, 55, TFT_WHITE);
        display->drawText(equippedSpells[selectedSpellSlot]->getName().c_str(), 
                         15, 70, equippedSpells[selectedSpellSlot]->getElementColor());
    } else {
        display->drawText("Current: Empty", 10, 55, 0x8410);
    }
}

void LibraryRoomState::drawAvailableSpells() {
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    
    for (int i = 0; i < knownSpells.size() && i < 7; i++) {
        int yPos = 110 + (i * 20);
        Spell* spell = knownSpells[i];
        
        display->drawText(spell->getName().c_str(), 30, yPos, TFT_WHITE);
        display->drawText(spell->getElementName().c_str(), 100, yPos, spell->getElementColor());
    }
}

void LibraryRoomState::drawSpellReplacementCursor(int spellIndex) {
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    if (spellIndex >= 0 && spellIndex < knownSpells.size()) {
        int yPos = 110 + (spellIndex * 20);
        display->drawText(">", 15, yPos, TFT_WHITE);
    }
}

void LibraryRoomState::clearSpellReplacementCursor(int spellIndex) {
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    if (spellIndex >= 0 && spellIndex < knownSpells.size()) {
        int yPos = 110 + (spellIndex * 20);
        display->fillRect(15, yPos, 12, 12, TFT_BLACK);
    }
}

void LibraryRoomState::updateSpellReplacementSelection() {
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    
    // Clear old cursor
    if (lastSelectedOption >= 0 && lastSelectedOption < knownSpells.size()) {
        clearSpellReplacementCursor(lastSelectedOption);
    }
    
    // Draw new cursor
    drawSpellReplacementCursor(selectedOption);
    
    lastSelectedOption = selectedOption;
}

// ==============================================
// ACTION METHODS (mostly unchanged)
// ==============================================

void LibraryRoomState::readSelectedScroll() {
    if (selectedScrollIndex >= availableScrolls.size()) return;
    
    Spell* scrollToRead = availableScrolls[selectedScrollIndex];
    
    if (player->getSpellLibrary()->hasSpell(scrollToRead->getID())) {
        display->clear();
        display->drawText("Already Known!", 30, 100, TFT_RED, 2);
        display->drawText("You already know", 25, 130, TFT_WHITE);
        display->drawText("this spell!", 40, 145, TFT_WHITE);
        display->drawText("Press any button", 20, 170, TFT_WHITE);
        
        while (true) {
            input->update();
            if (input->wasPressed(Button::UP) || input->wasPressed(Button::DOWN) ||
                input->wasPressed(Button::A) || input->wasPressed(Button::B)) {
                break;
            }
            delay(10);
        }
        
        delete scrollToRead;
        availableScrolls.erase(availableScrolls.begin() + selectedScrollIndex);
        
        if (selectedScrollIndex >= availableScrolls.size() && selectedScrollIndex > 0) {
            selectedScrollIndex--;
        }
        
        returnToMainMenu();
        return;
    }
    
    if (player->learnSpell(scrollToRead)) {
        showSpellLearned(scrollToRead);
        availableScrolls.erase(availableScrolls.begin() + selectedScrollIndex);
        
        if (selectedScrollIndex >= availableScrolls.size() && selectedScrollIndex > 0) {
            selectedScrollIndex--;
        }
        
        returnToMainMenu();
    }
}

void LibraryRoomState::returnToMainMenu() {
    currentScreen = SCREEN_MAIN_MENU;
    selectedOption = 0;
    screenDrawn = false;
    lastSelectedOption = -1;
    
    delay(100);
    drawMainMenu();
}

void LibraryRoomState::completeRoom() {
    // FIXED: Only request state change if we don't already have one pending
    if (getNextState() == StateTransition::NONE) {
        if (dungeonManager) {
            Floor* currentFloor = dungeonManager->getCurrentFloor();
            if (currentFloor) {
                currentFloor->incrementRoomsCompleted();
            }
        }
        
        requestStateChange(StateTransition::DOOR_CHOICE);
    }
}

void LibraryRoomState::performRest() {
    if (player->getGold() < REST_COST) {
        drawRestResult(false, "Need 20 gold to rest");
        currentScreen = SCREEN_REST_RESULT;
        return;
    }
    
    if (player->getCurrentHP() >= player->getMaxHP() && 
        player->getCurrentMana() >= player->getMaxMana()) {
        drawRestResult(false, "Already at full health and mana");
        currentScreen = SCREEN_REST_RESULT;
        return;
    }
    
    player->spendGold(REST_COST);
    player->heal(player->getMaxHP());
    player->restoreAllMana();
    player->clearSpellEffects();
    
    drawRestResult(true, "");
    currentScreen = SCREEN_REST_RESULT;
}

void LibraryRoomState::openScrolls() {
    currentScreen = SCREEN_SCROLL_SELECTION;
    selectedScrollIndex = 0;
    selectedOption = 0; // Keep in sync
    screenDrawn = false;
    lastSelectedOption = -1;
    drawScrollSelection();
}

void LibraryRoomState::openSpellManagement() {
    currentScreen = SCREEN_SPELL_MANAGEMENT;
    selectedOption = 0;
    screenDrawn = false;
    lastSelectedOption = -1;
    drawSpellManagement();
}

void LibraryRoomState::equipSpellToSlot() {
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    if (selectedOption >= knownSpells.size()) return;
    
    Spell* spellToEquip = knownSpells[selectedOption];
    
    if (player->getSpellLibrary()->equipSpell(spellToEquip->getID(), selectedSpellSlot)) {
        display->clear();
        display->drawText("Spell Equipped!", 25, 100, TFT_GREEN, 2);
        display->drawText(spellToEquip->getName().c_str(), 20, 130, spellToEquip->getElementColor());
        display->drawText(("to Slot " + String(selectedSpellSlot + 1)).c_str(), 35, 145, TFT_WHITE);
        display->drawText("Press any button", 20, 170, TFT_WHITE);
        
        while (true) {
            input->update();
            if (input->wasPressed(Button::UP) || input->wasPressed(Button::DOWN) ||
                input->wasPressed(Button::A) || input->wasPressed(Button::B)) {
                break;
            }
            delay(10);
        }
    }
    
    currentScreen = SCREEN_SPELL_MANAGEMENT;
    selectedOption = selectedSpellSlot;
    screenDrawn = false;
    lastSelectedOption = -1;
    drawSpellManagement();
}

void LibraryRoomState::showSpellLearned(Spell* spell) {
    display->clear();
    
    display->drawText("SPELL LEARNED!", 25, 80, TFT_GREEN, 2);
    display->drawText(spell->getName().c_str(), 30, 110, spell->getElementColor());
    display->drawText(spell->getElementName().c_str(), 40, 125, TFT_WHITE);
    display->drawText(("Power: " + String(spell->getBasePower())).c_str(), 45, 140, TFT_WHITE);
    
    display->drawText("Added to grimoire!", 20, 165, TFT_WHITE);
    display->drawText("Visit 'Manage Spells'", 15, 185, TFT_WHITE);
    display->drawText("to equip it!", 40, 200, TFT_WHITE);
    display->drawText("Press any button", 25, 225, TFT_WHITE);
    
    while (true) {
        input->update();
        if (input->wasPressed(Button::UP) || input->wasPressed(Button::DOWN) ||
            input->wasPressed(Button::A) || input->wasPressed(Button::B)) {
            break;
        }
        delay(10);
    }
}

void LibraryRoomState::drawRestResult(bool success, String message) {
    display->clear();
    
    if (success) {
        display->drawText("Rest Complete", 25, 80, TFT_GREEN, 2);
        display->drawText("Health & Mana", 25, 110, TFT_WHITE);
        display->drawText("Fully Restored!", 20, 125, TFT_WHITE);
        display->drawText(("Gold: " + String(player->getGold())).c_str(), 40, 145, TFT_WHITE);
    } else {
        display->drawText("Cannot Rest", 30, 80, TFT_RED, 2);
        display->drawText(message.c_str(), 20, 110, TFT_WHITE);
    }
    
    display->drawText("Press any button", 20, 160, TFT_WHITE);
    display->drawText("to continue", 35, 175, TFT_WHITE);
}

// Scroll management methods (unchanged)
void LibraryRoomState::giveScrollReward(int spellID) {
    Spell* scroll = SpellFactory::createSpell(spellID);
    if (scroll) {
        addAvailableScroll(scroll);
        Serial.println("Received scroll: " + scroll->getName());
    }
}

void LibraryRoomState::giveBossScrollReward() {
    Spell* bossScroll = SpellFactory::createRandomSpell(2, 3);
    if (bossScroll) {
        addAvailableScroll(bossScroll);
        Serial.println("Boss dropped scroll: " + bossScroll->getName());
    }
}

void LibraryRoomState::giveRandomScroll() {
    Spell* randomScroll = SpellFactory::createRandomSpell(1, 2);
    if (randomScroll) {
        addAvailableScroll(randomScroll);
        Serial.println("Found random scroll: " + randomScroll->getName());
    }
}

void LibraryRoomState::addAvailableScroll(Spell* spell) {
    if (spell) {
        availableScrolls.push_back(spell);
        Serial.println("LibraryRoomState: Added scroll to available list: " + spell->getName());
    }
}

void LibraryRoomState::removeScroll(int index) {
    if (index >= 0 && index < availableScrolls.size()) {
        delete availableScrolls[index];
        availableScrolls.erase(availableScrolls.begin() + index);
    }
}

bool LibraryRoomState::hasScrolls() const {
    return !availableScrolls.empty();
}