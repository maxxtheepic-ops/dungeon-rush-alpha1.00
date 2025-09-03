// src/rooms/LibraryRoomState.cpp
#include "LibraryRoomState.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../dungeon/DungeonManager.h"
#include "../dungeon/Room.h"  // ADDED: Need Room class for completion
#include "../spells/spell.h"

LibraryRoomState::LibraryRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) 
    : GameState(disp, inp) {
    player = p;
    currentEnemy = e;
    dungeonManager = dm;
    gameStateManager = nullptr;  // Will be set by GameStateManager
    
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
    // Clean up any unclaimed scrolls
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
    // ADDED: Force immediate draw when entering
    drawMainMenu();
}

void LibraryRoomState::update() {
    switch (currentScreen) {
        case SCREEN_MAIN_MENU:
            handleMainMenuInput();
            // Only redraw main menu if we're actually still on the main menu screen
            if (currentScreen == SCREEN_MAIN_MENU && (!screenDrawn || selectedOption != lastSelectedOption)) {
                drawMainMenu();
            }
            break;
            
        case SCREEN_SCROLL_SELECTION:
            handleScrollSelectionInput();
            // Only draw if we're still on this screen
            if (currentScreen == SCREEN_SCROLL_SELECTION && !screenDrawn) {
                drawScrollSelection();
            }
            break;
            
        case SCREEN_SPELL_MANAGEMENT:
            handleSpellManagementInput();
            // Only draw if we're still on this screen
            if (currentScreen == SCREEN_SPELL_MANAGEMENT && !screenDrawn) {
                drawSpellManagement();
            }
            break;
            
        case SCREEN_SPELL_REPLACEMENT:
            handleSpellReplacementInput();
            // Only draw if we're still on this screen
            if (currentScreen == SCREEN_SPELL_REPLACEMENT && !screenDrawn) {
                drawSpellReplacement();
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

void LibraryRoomState::handleMainMenuInput() {
    // Navigation
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) selectedOption = maxOptions - 1;
        // CHANGED: Force immediate redraw
        drawMainMenu();
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption >= maxOptions) selectedOption = 0;
        // CHANGED: Force immediate redraw
        drawMainMenu();
    }
    
    // Selection with A button only
    if (input->wasPressed(Button::A)) {
        Serial.println("DEBUG: A button pressed, selectedOption = " + String(selectedOption));
        switch (selectedOption) {
            case 0: // Rest
                performRest();
                break;
            case 1: // Read Scrolls
                if (hasScrolls()) {
                    openScrolls();
                } else {
                    Serial.println("No scrolls available - ignoring selection");
                }
                break;
            case 2: // Manage Spells
                openSpellManagement();
                break;
            case 3: // Leave
                Serial.println("DEBUG: Leave option selected, calling completeRoom()");
                completeRoom();
                return;
        }
    }
    
    // REMOVED: All B button handling from main menu
    // B button should NEVER exit the library from the main menu
    // Only the "Leave" option should exit the library
}

void LibraryRoomState::handleScrollSelectionInput() {
    if (availableScrolls.empty()) {
        if (input->wasPressed(Button::B)) {
            Serial.println("DEBUG: B pressed on empty scroll screen - returning to main menu");
            returnToMainMenu();
        }
        return;
    }
    
    // Navigation
    if (input->wasPressed(Button::UP)) {
        selectedScrollIndex--;
        if (selectedScrollIndex < 0) selectedScrollIndex = availableScrolls.size() - 1;
        // CHANGED: Force immediate redraw
        drawScrollSelection();
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedScrollIndex++;
        if (selectedScrollIndex >= availableScrolls.size()) selectedScrollIndex = 0;
        // CHANGED: Force immediate redraw
        drawScrollSelection();
    }
    
    // Selection - read the selected scroll
    if (input->wasPressed(Button::A)) {
        readSelectedScroll();
    }
    
    if (input->wasPressed(Button::B)) {
        Serial.println("DEBUG: B pressed in scroll selection - returning to main menu");
        returnToMainMenu();
    }
}

void LibraryRoomState::handleSpellManagementInput() {
    // Navigation
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) selectedOption = 3;
        // CHANGED: Force immediate redraw
        drawSpellManagement();
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption > 3) selectedOption = 0;
        // CHANGED: Force immediate redraw
        drawSpellManagement();
    }
    
    // Selection - manage the selected spell slot
    if (input->wasPressed(Button::A)) {
        selectedSpellSlot = selectedOption;
        if (player->getSpellLibrary()->getKnownSpellCount() > 0) {
            currentScreen = SCREEN_SPELL_REPLACEMENT;
            selectedOption = 0;
            // CHANGED: Force immediate draw of new screen
            drawSpellReplacement();
        }
    }
    
    if (input->wasPressed(Button::B)) {
        Serial.println("DEBUG: B pressed in spell management - returning to main menu");
        returnToMainMenu();
    }
}

void LibraryRoomState::handleSpellReplacementInput() {
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    
    // Navigation
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) selectedOption = knownSpells.size() - 1;
        // CHANGED: Force immediate redraw
        drawSpellReplacement();
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption >= knownSpells.size()) selectedOption = 0;
        // CHANGED: Force immediate redraw
        drawSpellReplacement();
    }
    
    // Selection
    if (input->wasPressed(Button::A)) {
        equipSpellToSlot();
    }
    
    if (input->wasPressed(Button::B)) {
        Serial.println("DEBUG: B pressed in spell replacement - returning to spell management");
        currentScreen = SCREEN_SPELL_MANAGEMENT;
        selectedOption = selectedSpellSlot;
        // CHANGED: Force immediate draw of spell management screen
        drawSpellManagement();
    }
}

void LibraryRoomState::handleRestResultInput() {
    if (input->wasPressed(Button::UP) || input->wasPressed(Button::DOWN) ||
        input->wasPressed(Button::A) || input->wasPressed(Button::B)) {
        returnToMainMenu();
    }
}

void LibraryRoomState::readSelectedScroll() {
    if (selectedScrollIndex >= availableScrolls.size()) return;
    
    Spell* scrollToRead = availableScrolls[selectedScrollIndex];
    
    // Check if player already knows this spell
    if (player->getSpellLibrary()->hasSpell(scrollToRead->getID())) {
        display->clear();
        display->drawText("Already Known!", 30, 100, TFT_RED, 2);
        display->drawText("You already know", 25, 130, TFT_WHITE);
        display->drawText("this spell!", 40, 145, TFT_WHITE);
        display->drawText("Press any button", 20, 170, TFT_CYAN);
        
        // Wait for input
        while (true) {
            input->update();
            if (input->wasPressed(Button::UP) || input->wasPressed(Button::DOWN) ||
                input->wasPressed(Button::A) || input->wasPressed(Button::B)) {
                break;
            }
            delay(10);
        }
        
        // Remove the scroll since it was "read"
        delete scrollToRead;
        availableScrolls.erase(availableScrolls.begin() + selectedScrollIndex);
        
        // Adjust selection if needed
        if (selectedScrollIndex >= availableScrolls.size() && selectedScrollIndex > 0) {
            selectedScrollIndex--;
        }
        
        // Always return to main menu after reading
        Serial.println("Scroll was already known - returning to main library menu");
        returnToMainMenu();
        return;
    }
    
    // Learn the spell
    if (player->learnSpell(scrollToRead)) {
        showSpellLearned(scrollToRead);
        
        // Remove from available scrolls (now owned by player)
        availableScrolls.erase(availableScrolls.begin() + selectedScrollIndex);
        
        // Adjust selection if needed
        if (selectedScrollIndex >= availableScrolls.size() && selectedScrollIndex > 0) {
            selectedScrollIndex--;
        }
        
        // Always return to main menu after learning a spell
        Serial.println("Spell learned - returning to main library menu");
        returnToMainMenu();
    }
}

void LibraryRoomState::returnToMainMenu() {
    currentScreen = SCREEN_MAIN_MENU;
    selectedOption = 0;
    screenDrawn = false;
    
    // Small delay to prevent input carry-over
    delay(100);
    
    // CHANGED: Force immediate redraw of main menu
    drawMainMenu();
    
    Serial.println("DEBUG: Returned to library main menu");
}

void LibraryRoomState::completeRoom() {
    Serial.println("=== DEBUG: LibraryRoomState::completeRoom() START ===");
    
    // Direct approach: Just increment the dungeon progress
    if (dungeonManager) {
        Serial.println("DEBUG: DungeonManager exists");
        Floor* currentFloor = dungeonManager->getCurrentFloor();
        if (currentFloor) {
            Serial.println("DEBUG: Current floor exists");
            Serial.println("DEBUG: Rooms completed BEFORE: " + String(currentFloor->getRoomsCompleted()));
            
            // Directly increment the room completion counter
            currentFloor->incrementRoomsCompleted();
            
            Serial.println("DEBUG: Rooms completed AFTER: " + String(currentFloor->getRoomsCompleted()));
            Serial.println("DEBUG: Library room completion - SUCCESS");
        } else {
            Serial.println("ERROR: No current floor found!");
        }
    } else {
        Serial.println("ERROR: No dungeon manager found!");
    }
    
    Serial.println("=== DEBUG: LibraryRoomState::completeRoom() END ===");
    Serial.println("Library visit complete - returning to door choice with progress");
    requestStateChange(StateTransition::DOOR_CHOICE);
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
    
    // Perform rest
    player->spendGold(REST_COST);
    player->heal(player->getMaxHP());
    player->restoreAllMana();
    player->clearSpellEffects();
    
    drawRestResult(true, "");
    currentScreen = SCREEN_REST_RESULT;
    
    Serial.println("Player rested - full health and mana restored");
}

void LibraryRoomState::openScrolls() {
    currentScreen = SCREEN_SCROLL_SELECTION;
    selectedScrollIndex = 0;
    screenDrawn = false;
    // CHANGED: Force immediate draw of scroll selection screen
    drawScrollSelection();
}

void LibraryRoomState::openSpellManagement() {
    currentScreen = SCREEN_SPELL_MANAGEMENT;
    selectedOption = 0;
    screenDrawn = false;
    // CHANGED: Force immediate draw of spell management screen
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
        display->drawText("Press any button", 20, 170, TFT_CYAN);
        
        // Wait for input
        while (true) {
            input->update();
            if (input->wasPressed(Button::UP) || input->wasPressed(Button::DOWN) ||
                input->wasPressed(Button::A) || input->wasPressed(Button::B)) {
                break;
            }
            delay(10);
        }
        
        Serial.println("Equipped " + spellToEquip->getName() + " to slot " + String(selectedSpellSlot + 1));
    }
    
    // Return to spell management
    currentScreen = SCREEN_SPELL_MANAGEMENT;
    selectedOption = selectedSpellSlot;
    screenDrawn = false;
    // CHANGED: Force immediate draw of spell management screen
    drawSpellManagement();
}

void LibraryRoomState::showSpellLearned(Spell* spell) {
    display->clear();
    
    display->drawText("SPELL LEARNED!", 25, 80, TFT_GREEN, 2);
    display->drawText(spell->getName().c_str(), 30, 110, spell->getElementColor());
    display->drawText(spell->getElementName().c_str(), 40, 125, TFT_WHITE);
    display->drawText(("Power: " + String(spell->getBasePower())).c_str(), 45, 140, TFT_YELLOW);
    
    display->drawText("Added to grimoire!", 20, 165, TFT_CYAN);
    display->drawText("Visit 'Manage Spells'", 15, 185, TFT_PURPLE);
    display->drawText("to equip it!", 40, 200, TFT_PURPLE);
    display->drawText("Press any button", 25, 225, TFT_CYAN);
    
    // Wait for input
    while (true) {
        input->update();
        if (input->wasPressed(Button::UP) || input->wasPressed(Button::DOWN) ||
            input->wasPressed(Button::A) || input->wasPressed(Button::B)) {
            break;
        }
        delay(10);
    }
    
    Serial.println("Player learned: " + spell->getName());
}

// Drawing methods remain the same...
void LibraryRoomState::drawMainMenu() {
    display->clear();
    
    // Title with mystical theme
    display->drawText("ARCANE LIBRARY", 25, 15, TFT_PURPLE, 2);
    display->drawText("\"Knowledge is power\"", 15, 35, TFT_CYAN);
    display->drawText("- Ancient Wizard", 25, 50, TFT_WHITE, 1);
    
    // Player status
    display->drawText(("HP: " + String(player->getCurrentHP()) + "/" + String(player->getMaxHP())).c_str(), 
                     10, 70, TFT_WHITE);
    display->drawText(("Mana: " + String(player->getCurrentMana()) + "/" + String(player->getMaxMana())).c_str(), 
                     10, 85, TFT_BLUE);
    display->drawText(("Gold: " + String(player->getGold())).c_str(), 
                     10, 100, TFT_YELLOW);
    
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
            display->drawText(("T2: " + String(tier2Count)).c_str(), 55, 130, TFT_YELLOW);
        }
        if (tier3Count > 0) {
            display->drawText(("T3: " + String(tier3Count)).c_str(), 95, 130, TFT_RED);
        }
    }
    
    // Menu options
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
                textColor = TFT_DARKGREY;
            } else {
                optionText = "Read Scrolls (" + String(availableScrolls.size()) + ")";
                textColor = TFT_GREEN;
            }
        }
        
        if (i == selectedOption) {
            display->fillRect(5, yPos - 3, 160, 20, TFT_BLUE);
            display->drawText(">", 10, yPos, TFT_YELLOW);
            display->drawText(optionText.c_str(), 25, yPos, TFT_WHITE);
        } else {
            display->drawText(optionText.c_str(), 25, yPos, textColor);
        }
    }
    
    // Show equipped spells at bottom
    display->drawText("Equipped:", 10, 250, TFT_PURPLE);
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
            display->drawText((String(i + 1) + ": ---").c_str(), x, y, TFT_DARKGREY, 1);
        }
    }
    
    // Controls - UPDATED: No mention of B button for main menu
    display->drawText("UP/DOWN: Navigate, A: Select", 5, 295, TFT_CYAN, 1);
    
    screenDrawn = true;
    lastSelectedOption = selectedOption;
}

void LibraryRoomState::drawScrollSelection() {
    display->clear();
    
    display->drawText("ANCIENT SCROLLS", 25, 15, TFT_GREEN, 2);
    
    if (availableScrolls.empty()) {
        display->drawText("No scrolls to read", 25, 100, TFT_RED);
        display->drawText("Find treasure chests", 20, 120, TFT_YELLOW);
        display->drawText("or defeat bosses!", 25, 135, TFT_YELLOW);
        display->drawText("B: Return", 50, 200, TFT_CYAN);
        screenDrawn = true;
        return;
    }
    
    // Show scrolls with mystery/tier information only
    display->drawText(("You have " + String(availableScrolls.size()) + " scrolls:").c_str(), 10, 40, TFT_WHITE);
    
    for (int i = 0; i < availableScrolls.size() && i < 6; i++) {
        int yPos = 60 + (i * 35);
        bool selected = (i == selectedScrollIndex);
        
        if (selected) {
            display->fillRect(5, yPos - 3, 160, 30, TFT_BLUE);
        }
        
        Spell* scroll = availableScrolls[i];
        
        // Show mysterious scroll description instead of actual spell name
        String scrollDesc = "Mysterious Scroll";
        String tierDesc = "Tier 1";
        uint16_t tierColor = TFT_WHITE;
        
        if (scroll->getBasePower() > 25) {
            tierDesc = "Tier 3 (Powerful)";
            tierColor = TFT_RED;
        } else if (scroll->getBasePower() > 20) {
            tierDesc = "Tier 2 (Advanced)";
            tierColor = TFT_YELLOW;
        } else {
            tierDesc = "Tier 1 (Basic)";
            tierColor = TFT_WHITE;
        }
        
        display->drawText(("> " + scrollDesc).c_str(), 10, yPos, TFT_WHITE);
        display->drawText(tierDesc.c_str(), 10, yPos + 12, tierColor);
        display->drawText(scroll->getElementName().c_str(), 10, yPos + 24, scroll->getElementColor());
    }
    
    // Instructions
    display->drawText("A: Read Scroll", 20, 280, TFT_CYAN);
    display->drawText("B: Back", 20, 295, TFT_CYAN);
    
    screenDrawn = true;
}

void LibraryRoomState::drawSpellManagement() {
    display->clear();
    
    display->drawText("SPELL GRIMOIRE", 25, 15, TFT_PURPLE, 2);
    
    // Show equipped spells
    display->drawText("Equipped:", 10, 40, TFT_WHITE);
    auto equippedSpells = player->getEquippedSpells();
    
    for (int i = 0; i < 4; i++) {
        int yPos = 55 + (i * 25);
        bool selected = (i == selectedOption);
        
        if (selected) {
            display->fillRect(5, yPos - 3, 160, 20, TFT_BLUE);
        }
        
        String slotText = "Slot " + String(i + 1) + ": ";
        if (i < equippedSpells.size() && equippedSpells[i]) {
            slotText += equippedSpells[i]->getName();
            display->drawText(slotText.c_str(), 10, yPos, TFT_WHITE);
            display->drawText(equippedSpells[i]->getElementName().c_str(), 
                            120, yPos, equippedSpells[i]->getElementColor());
        } else {
            slotText += "Empty";
            display->drawText(slotText.c_str(), 10, yPos, TFT_DARKGREY);
        }
    }
    
    // Show known spells
    display->drawText("Known:", 10, 170, TFT_WHITE);
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    int knownCount = min(3, (int)knownSpells.size());
    
    for (int i = 0; i < knownCount; i++) {
        int yPos = 185 + (i * 15);
        display->drawText(("- " + knownSpells[i]->getName()).c_str(), 15, yPos, TFT_CYAN, 1);
    }
    
    if (knownSpells.size() > 3) {
        display->drawText(("... and " + String(knownSpells.size() - 3) + " more").c_str(), 
                         15, 230, TFT_CYAN, 1);
    }
    
    display->drawText("A: Manage Slot, B: Back", 10, 280, TFT_CYAN);
    screenDrawn = true;
}

void LibraryRoomState::drawSpellReplacement() {
    display->clear();
    
    display->drawText("EQUIP SPELL", 30, 15, TFT_PURPLE, 2);
    display->drawText(("To Slot " + String(selectedSpellSlot + 1)).c_str(), 50, 35, TFT_WHITE);
    
    // Show current spell in slot (if any)
    auto equippedSpells = player->getEquippedSpells();
    if (selectedSpellSlot < equippedSpells.size() && equippedSpells[selectedSpellSlot]) {
        display->drawText("Current:", 10, 55, TFT_WHITE);
        display->drawText(equippedSpells[selectedSpellSlot]->getName().c_str(), 
                         15, 70, equippedSpells[selectedSpellSlot]->getElementColor());
    } else {
        display->drawText("Current: Empty", 10, 55, TFT_DARKGREY);
    }
    
    // Show available spells - UPDATED: Cursor only
    display->drawText("Available:", 10, 95, TFT_WHITE);
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    
    for (int i = 0; i < knownSpells.size() && i < 7; i++) {
        int yPos = 110 + (i * 20);
        bool selected = (i == selectedOption);
        
        Spell* spell = knownSpells[i];
        
        if (selected) {
            display->drawText(">", 10, yPos, TFT_YELLOW);
        }
        
        display->drawText(spell->getName().c_str(), 25, yPos, TFT_WHITE);
        display->drawText(spell->getElementName().c_str(), 100, yPos, spell->getElementColor());
    }
    
    display->drawText("A: Equip, B: Back", 20, 280, TFT_CYAN);
    screenDrawn = true;
}

void LibraryRoomState::drawRestResult(bool success, String message) {
    display->clear();
    
    if (success) {
        display->drawText("Rest Complete", 25, 80, TFT_GREEN, 2);
        display->drawText("Health & Mana", 25, 110, TFT_WHITE);
        display->drawText("Fully Restored!", 20, 125, TFT_WHITE);
        display->drawText(("Gold: " + String(player->getGold())).c_str(), 40, 145, TFT_YELLOW);
    } else {
        display->drawText("Cannot Rest", 30, 80, TFT_RED, 2);
        display->drawText(message.c_str(), 20, 110, TFT_WHITE);
    }
    
    display->drawText("Press any button", 20, 160, TFT_CYAN);
    display->drawText("to continue", 35, 175, TFT_CYAN);
}

// Scroll reward methods
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