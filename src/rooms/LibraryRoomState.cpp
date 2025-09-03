#include "LibraryRoomState.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../dungeon/DungeonManager.h"
#include "../spells/spell.h"

LibraryRoomState::LibraryRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) 
    : GameState(disp, inp) {
    player = p;
    currentEnemy = e;
    dungeonManager = dm;
    
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
}

void LibraryRoomState::update() {
    switch (currentScreen) {
        case SCREEN_MAIN_MENU:
            handleMainMenuInput();
            if (!screenDrawn) drawMainMenu();
            break;
            
        case SCREEN_SCROLL_SELECTION:
            handleScrollSelectionInput();
            if (!screenDrawn) drawScrollSelection();
            break;
            
        case SCREEN_SPELL_MANAGEMENT:
            handleSpellManagementInput();
            if (!screenDrawn) drawSpellManagement();
            break;
            
        case SCREEN_SPELL_REPLACEMENT:
            handleSpellReplacementInput();
            if (!screenDrawn) drawSpellReplacement();
            break;
            
        case SCREEN_REST_RESULT:
            handleRestResultInput();
            break;
    }
}

void LibraryRoomState::exit() {
    Serial.println("Leaving the library behind...");
}

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
    
    // Available scrolls indicator
    if (hasScrolls()) {
        display->drawText(("Scrolls: " + String(availableScrolls.size())).c_str(), 
                         10, 115, TFT_GREEN);
    }
    
    // Menu options
    int yStart = 140;
    int ySpacing = 25;
    const char* options[4] = {"Rest (20g)", "Read Scrolls", "Manage Spells", "Leave"};
    
    // Update option text based on availability
    String scrollOption = hasScrolls() ? "Read Scrolls" : "Read Scrolls (0)";
    
    for (int i = 0; i < maxOptions; i++) {
        int yPos = yStart + (i * ySpacing);
        String optionText = options[i];
        
        if (i == 1) optionText = scrollOption; // Update scroll option
        
        if (i == selectedOption) {
            display->fillRect(5, yPos - 3, 160, 20, TFT_BLUE);
            display->drawText(">", 10, yPos, TFT_YELLOW);
            display->drawText(optionText.c_str(), 25, yPos, TFT_WHITE);
        } else {
            uint16_t color = TFT_WHITE;
            if (i == 0 && player->getGold() < REST_COST) color = TFT_RED;
            if (i == 1 && !hasScrolls()) color = TFT_GRAY;
            
            display->drawText(optionText.c_str(), 25, yPos, color);
        }
    }
    
    // Show equipped spells at bottom
    display->drawText("Equipped Spells:", 10, 250, TFT_PURPLE);
    auto equippedSpells = player->getEquippedSpells();
    for (int i = 0; i < 4; i++) {
        int x = 10 + (i * 35);
        int y = 265;
        
        if (i < equippedSpells.size() && equippedSpells[i]) {
            display->drawText((String(i + 1) + ":").c_str(), x, y, TFT_WHITE, 1);
            display->drawText(equippedSpells[i]->getName().substring(0, 4).c_str(), x, y + 10, 
                            equippedSpells[i]->getElementColor(), 1);
        } else {
            display->drawText((String(i + 1) + ": Empty").c_str(), x, y, TFT_GRAY, 1);
        }
    }
    
    // Controls
    display->drawText("UP/DOWN: Navigate, A: Select", 5, 295, TFT_CYAN, 1);
    
    screenDrawn = true;
    lastSelectedOption = selectedOption;
}

void LibraryRoomState::drawScrollSelection() {
    display->clear();
    
    display->drawText("ANCIENT SCROLLS", 25, 15, TFT_GREEN, 2);
    
    if (availableScrolls.empty()) {
        display->drawText("No scrolls available", 25, 100, TFT_RED);
        display->drawText("B: Return", 50, 200, TFT_CYAN);
        screenDrawn = true;
        return;
    }
    
    // Show scrolls
    for (int i = 0; i < availableScrolls.size() && i < 6; i++) {
        int yPos = 50 + (i * 35);
        bool selected = (i == selectedScrollIndex);
        
        if (selected) {
            display->fillRect(5, yPos - 3, 160, 30, TFT_BLUE);
        }
        
        Spell* spell = availableScrolls[i];
        display->drawText(("> " + spell->getName()).c_str(), 10, yPos, TFT_WHITE);
        display->drawText(spell->getElementName().c_str(), 10, yPos + 12, spell->getElementColor());
        display->drawText(("Power: " + String(spell->getBasePower())).c_str(), 10, yPos + 24, TFT_YELLOW);
    }
    
    // Instructions
    display->drawText("A: Learn, B: Back", 20, 280, TFT_CYAN);
    
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
            display->drawText(slotText.c_str(), 10, yPos, TFT_GRAY);
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
        display->drawText("Current: Empty", 10, 55, TFT_GRAY);
    }
    
    // Show available spells
    display->drawText("Available:", 10, 95, TFT_WHITE);
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    
    for (int i = 0; i < knownSpells.size() && i < 7; i++) {
        int yPos = 110 + (i * 20);
        bool selected = (i == selectedOption);
        
        if (selected) {
            display->fillRect(5, yPos - 3, 160, 18, TFT_BLUE);
        }
        
        Spell* spell = knownSpells[i];
        display->drawText((">" + spell->getName()).c_str(), 10, yPos, TFT_WHITE);
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

void LibraryRoomState::handleMainMenuInput() {
    // Navigation
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) selectedOption = maxOptions - 1;
        screenDrawn = false;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption >= maxOptions) selectedOption = 0;
        screenDrawn = false;
    }
    
    // Selection
    if (input->wasPressed(Button::A)) {
        switch (selectedOption) {
            case 0: // Rest
                performRest();
                break;
            case 1: // Read Scrolls
                if (hasScrolls()) {
                    openScrolls();
                } else {
                    // Show "no scrolls" message briefly
                    display->clear();
                    display->drawText("No scrolls to read!", 25, 120, TFT_RED, 2);
                    delay(1000);
                    screenDrawn = false;
                }
                break;
            case 2: // Manage Spells
                openSpellManagement();
                break;
            case 3: // Leave
                completeRoom();
                return;
        }
    }
    
    // Quick exit
    if (input->wasPressed(Button::B)) {
        completeRoom();
    }
}

void LibraryRoomState::handleScrollSelectionInput() {
    if (availableScrolls.empty()) {
        if (input->wasPressed(Button::B)) {
            returnToMainMenu();
        }
        return;
    }
    
    // Navigation
    if (input->wasPressed(Button::UP)) {
        selectedScrollIndex--;
        if (selectedScrollIndex < 0) selectedScrollIndex = availableScrolls.size() - 1;
        screenDrawn = false;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedScrollIndex++;
        if (selectedScrollIndex >= availableScrolls.size()) selectedScrollIndex = 0;
        screenDrawn = false;
    }
    
    // Selection
    if (input->wasPressed(Button::A)) {
        learnSelectedSpell();
    }
    
    if (input->wasPressed(Button::B)) {
        returnToMainMenu();
    }
}

void LibraryRoomState::handleSpellManagementInput() {
    // Navigation
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) selectedOption = 3;
        screenDrawn = false;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption > 3) selectedOption = 0;
        screenDrawn = false;
    }
    
    // Selection - manage the selected spell slot
    if (input->wasPressed(Button::A)) {
        selectedSpellSlot = selectedOption;
        // Show spell replacement screen if player has known spells
        if (player->getSpellLibrary()->getKnownSpellCount() > 0) {
            currentScreen = SCREEN_SPELL_REPLACEMENT;
            selectedOption = 0;
            screenDrawn = false;
        }
    }
    
    if (input->wasPressed(Button::B)) {
        returnToMainMenu();
    }
}

void LibraryRoomState::handleSpellReplacementInput() {
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    
    // Navigation
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) selectedOption = knownSpells.size() - 1;
        screenDrawn = false;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption >= knownSpells.size()) selectedOption = 0;
        screenDrawn = false;
    }
    
    // Selection
    if (input->wasPressed(Button::A)) {
        equipSpellToSlot();
    }
    
    if (input->wasPressed(Button::B)) {
        currentScreen = SCREEN_SPELL_MANAGEMENT;
        selectedOption = selectedSpellSlot;
        screenDrawn = false;
    }
}

void LibraryRoomState::handleRestResultInput() {
    if (input->wasPressed(Button::UP) || input->wasPressed(Button::DOWN) ||
        input->wasPressed(Button::A) || input->wasPressed(Button::B)) {
        returnToMainMenu();
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
    
    // Perform rest
    player->spendGold(REST_COST);
    player->heal(player->getMaxHP());
    player->restoreAllMana();
    player->clearSpellEffects(); // Clear any lingering spell effects
    
    drawRestResult(true, "");
    currentScreen = SCREEN_REST_RESULT;
    
    Serial.println("Player rested - full health and mana restored");
}

void LibraryRoomState::openScrolls() {
    currentScreen = SCREEN_SCROLL_SELECTION;
    selectedScrollIndex = 0;
    screenDrawn = false;
}

void LibraryRoomState::openSpellManagement() {
    currentScreen = SCREEN_SPELL_MANAGEMENT;
    selectedOption = 0;
    screenDrawn = false;
}

void LibraryRoomState::learnSelectedSpell() {
    if (selectedScrollIndex >= availableScrolls.size()) return;
    
    Spell* spellToLearn = availableScrolls[selectedScrollIndex];
    
    // Check if player already knows this spell
    if (player->getSpellLibrary()->hasSpell(spellToLearn->getID())) {
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
        
        screenDrawn = false;
        return;
    }
    
    // Learn the spell
    if (player->learnSpell(spellToLearn)) {
        showSpellLearned(spellToLearn);
        
        // Remove from available scrolls (don't delete, it's now owned by player)
        availableScrolls.erase(availableScrolls.begin() + selectedScrollIndex);
        
        // Adjust selection if needed
        if (selectedScrollIndex >= availableScrolls.size() && selectedScrollIndex > 0) {
            selectedScrollIndex--;
        }
        
        // If no more scrolls, return to main menu
        if (availableScrolls.empty()) {
            returnToMainMenu();
        } else {
            screenDrawn = false;
        }
    }
}

void LibraryRoomState::equipSpellToSlot() {
    auto knownSpells = player->getSpellLibrary()->getKnownSpells();
    if (selectedOption >= knownSpells.size()) return;
    
    Spell* spellToEquip = knownSpells[selectedOption];
    
    // Equip the spell to the selected slot
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
}

void LibraryRoomState::showSpellLearned(Spell* spell) {
    display->clear();
    
    display->drawText("SPELL LEARNED!", 25, 80, TFT_GREEN, 2);
    display->drawText(spell->getName().c_str(), 30, 110, spell->getElementColor());
    display->drawText(spell->getElementName().c_str(), 40, 125, TFT_WHITE);
    display->drawText(("Power: " + String(spell->getBasePower())).c_str(), 45, 140, TFT_YELLOW);
    
    display->drawText("Added to grimoire!", 20, 165, TFT_CYAN);
    display->drawText("Press any button", 25, 190, TFT_CYAN);
    
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

void LibraryRoomState::returnToMainMenu() {
    currentScreen = SCREEN_MAIN_MENU;
    selectedOption = 0;
    screenDrawn = false;
}

void LibraryRoomState::completeRoom() {
    Serial.println("Library visit complete - returning to door choice");
    requestStateChange(StateTransition::DOOR_CHOICE);
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
    // Boss drops higher tier spells
    Spell* bossScroll = SpellFactory::createRandomSpell(2, 3); // Tier 2-3 spells
    if (bossScroll) {
        addAvailableScroll(bossScroll);
        Serial.println("Boss dropped scroll: " + bossScroll->getName());
    }
}

void LibraryRoomState::giveRandomScroll() {
    Spell* randomScroll = SpellFactory::createRandomSpell(1, 2); // Tier 1-2 spells
    if (randomScroll) {
        addAvailableScroll(randomScroll);
        Serial.println("Found random scroll: " + randomScroll->getName());
    }
}

void LibraryRoomState::addAvailableScroll(Spell* spell) {
    if (spell) {
        availableScrolls.push_back(spell);
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