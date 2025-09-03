#include "TreasureRoomState.h"
#include "../spells/spell.h"
#include "../game/GameStateManager.h"

TreasureRoomState::TreasureRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) 
    : GameState(disp, inp) {
    player = p;
    currentEnemy = e;
    dungeonManager = dm;
    gameStateManager = nullptr;  // Will be set by GameStateManager
    selectedOption = 0;
    maxOptions = 2; // Take treasure, Leave
    screenDrawn = false;
    lastSelectedOption = -1;
    treasureLooted = false;
}

void TreasureRoomState::enter() {
    Serial.println("You discover a treasure room!");
    selectedOption = 0;
    treasureLooted = false;
    screenDrawn = false;
    drawTreasureScreen();
}

void TreasureRoomState::update() {
    handleTreasureInput();
    
    // Redraw if selection changed
    if (lastSelectedOption != selectedOption || !screenDrawn) {
        drawTreasureScreen();
    }
}

void TreasureRoomState::exit() {
    Serial.println("You leave the treasure room behind...");
}

void TreasureRoomState::drawTreasureScreen() {
    display->clear();
    
    // Title and atmosphere
    display->drawText("TREASURE ROOM", 25, 15, TFT_YELLOW, 2);
    display->drawText("Ancient magic fills", 20, 40, TFT_CYAN);
    display->drawText("the air...", 45, 55, TFT_CYAN);
    
    // Player status
    display->drawText(("HP: " + String(player->getCurrentHP()) + "/" + String(player->getMaxHP())).c_str(), 
                     10, 80, TFT_WHITE);
    display->drawText(("Mana: " + String(player->getCurrentMana()) + "/" + String(player->getMaxMana())).c_str(), 
                     10, 95, TFT_BLUE);
    display->drawText(("Gold: " + String(player->getGold())).c_str(), 
                     10, 110, TFT_YELLOW);
    
    // Treasure description
    if (!treasureLooted) {
        display->drawText("You see:", 10, 140, TFT_WHITE);
        display->drawText("- Ancient scroll", 15, 155, TFT_PURPLE);
        display->drawText("- Magical reagents", 15, 170, TFT_GREEN);
        display->drawText("- Mystical gems", 15, 185, TFT_CYAN);
        
        // Menu options
        int yStart = 215;
        int ySpacing = 20;
        const char* options[2] = {"Take Treasure", "Leave Empty-Handed"};
        
        for (int i = 0; i < maxOptions; i++) {
            int yPos = yStart + (i * ySpacing);
            
            // Highlight selected option
            if (i == selectedOption) {
                display->fillRect(5, yPos - 3, 160, 18, TFT_BLUE);
                display->drawText(">", 10, yPos, TFT_YELLOW);
                display->drawText(options[i], 25, yPos, TFT_WHITE);
            } else {
                display->drawText(options[i], 25, yPos, TFT_WHITE);
            }
        }
    } else {
        display->drawText("The room is empty now.", 15, 150, TFT_GREEN);
        display->drawText("Only dust remains...", 20, 170, TFT_GREEN);
        
        display->drawText("Press any button", 25, 210, TFT_CYAN);
        display->drawText("to leave", 55, 225, TFT_CYAN);
    }
    
    // Controls
    if (!treasureLooted) {
        display->drawText("UP/DOWN: Navigate", 10, 270, TFT_CYAN, 1);
        display->drawText("A: Select, B: Leave", 10, 285, TFT_CYAN, 1);
    }
    
    screenDrawn = true;
    lastSelectedOption = selectedOption;
}

void TreasureRoomState::handleTreasureInput() {
    if (treasureLooted) {
        // Any button leaves after treasure is taken
        if (input->wasPressed(Button::UP) || 
            input->wasPressed(Button::DOWN) ||
            input->wasPressed(Button::A) || 
            input->wasPressed(Button::B)) {
            completeRoom();
        }
        return;
    }
    
    // Navigation
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) {
            selectedOption = maxOptions - 1;
        }
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption >= maxOptions) {
            selectedOption = 0;
        }
    }
    
    // Selection
    if (input->wasPressed(Button::A)) {
        TreasureAction action = static_cast<TreasureAction>(selectedOption);
        
        switch (action) {
            case TreasureAction::TAKE_TREASURE:
                takeTreasure();
                break;
            case TreasureAction::LEAVE:
                completeRoom();
                break;
        }
    }
    
    // Quick exit
    if (input->wasPressed(Button::B)) {
        completeRoom();
    }
}

void TreasureRoomState::takeTreasure() {
    // Generate random scroll reward
    Spell* foundScroll = generateRandomScroll();
    String scrollName = foundScroll ? foundScroll->getName() : "Unknown Scroll";
    String elementName = foundScroll ? foundScroll->getElementName() : "Arcane";
    uint16_t elementColor = foundScroll ? foundScroll->getElementColor() : TFT_PURPLE;
    
    // Give some gold and mana potions as bonus
    int goldFound = 15 + (rand() % 25); // 15-40 gold
    int manaPotionsFound = 1 + (rand() % 2); // 1-2 mana potions
    
    // Give rewards to player
    player->addGold(goldFound);
    player->addManaPotions(manaPotionsFound);
    
    // Show treasure result
    showTreasureResult(foundScroll, goldFound, manaPotionsFound);
    
    // Give scroll to library for later access
    giveScrollToLibrary(foundScroll);
    
    treasureLooted = true;
    screenDrawn = false; // Force redraw
    
    Serial.println("Player found scroll: " + scrollName + " (" + elementName + "), " + 
                  String(goldFound) + " gold, " + String(manaPotionsFound) + " mana potions");
}

void TreasureRoomState::showTreasureResult(Spell* foundScroll, int gold, int manaPotions) {
    display->clear();
    
    display->drawText("TREASURE FOUND!", 25, 60, TFT_YELLOW, 2);
    
    if (foundScroll) {
        display->drawText("Ancient Scroll:", 25, 90, TFT_WHITE);
        display->drawText(foundScroll->getName().c_str(), 15, 105, foundScroll->getElementColor());
        display->drawText(("(" + foundScroll->getElementName() + " Magic)").c_str(), 20, 120, TFT_CYAN);
    }
    
    display->drawText(("+" + String(gold) + " Gold").c_str(), 40, 140, TFT_YELLOW);
    display->drawText(("+" + String(manaPotions) + " Mana Potions").c_str(), 25, 155, TFT_BLUE);
    
    display->drawText("Visit the Library", 25, 180, TFT_PURPLE);
    display->drawText("to learn the spell!", 20, 195, TFT_PURPLE);
    
    display->drawText("Press any button", 25, 220, TFT_CYAN);
    display->drawText("to continue", 40, 235, TFT_CYAN);
    
    // Wait for input
    while (true) {
        input->update();
        if (input->wasPressed(Button::UP) || 
            input->wasPressed(Button::DOWN) ||
            input->wasPressed(Button::A) || 
            input->wasPressed(Button::B)) {
            break;
        }
        delay(10);
    }
}

Spell* TreasureRoomState::generateRandomScroll() {
    // Generate scroll based on current floor for progression
    int currentFloor = dungeonManager->getCurrentFloorNumber();
    
    // Floor-based spell tier selection
    int minTier = 1;
    int maxTier = 1;
    
    if (currentFloor >= 2) maxTier = 2;  // Floor 2+ can drop tier 2 spells
    if (currentFloor >= 4) maxTier = 3;  // Floor 4+ can drop tier 3 spells
    if (currentFloor >= 3) minTier = 2;  // Floor 3+ won't drop basic spells
    
    return SpellFactory::createRandomSpell(minTier, maxTier);
}

void TreasureRoomState::giveScrollToLibrary(Spell* scroll) {
    if (scroll && gameStateManager) {
        Serial.println("TreasureRoom: Adding scroll to global inventory: " + scroll->getName());
        gameStateManager->addScroll(scroll);
        Serial.println("Scroll will be available in the Library!");
    } else if (scroll) {
        Serial.println("WARNING: No GameStateManager reference, adding scroll directly to player");
        // Fallback: add directly to player's spell library
        if (player->learnSpell(scroll)) {
            Serial.println("Added scroll directly to player's grimoire");
        } else {
            Serial.println("Player already knows this spell, deleting scroll");
            delete scroll;
        }
    }
}

void TreasureRoomState::completeRoom() {
    Serial.println("Treasure room completed - returning to door choice");
    requestStateChange(StateTransition::DOOR_CHOICE);
}