#include "TreasureRoomState.h"

TreasureRoomState::TreasureRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) 
    : RoomState(disp, inp, p, e, dm) {
    selectedOption = 0;
    maxOptions = 2; // Take treasure, Leave
    screenDrawn = false;
    lastSelectedOption = -1;
    treasureLooted = false;
}

void TreasureRoomState::enterRoom() {
    Serial.println("You discover a treasure room!");
    selectedOption = 0;
    treasureLooted = false;
    screenDrawn = false;
    drawTreasureScreen();
}

void TreasureRoomState::handleRoomInteraction() {
    handleTreasureInput();
    
    // Redraw if selection changed
    if (lastSelectedOption != selectedOption || !screenDrawn) {
        drawTreasureScreen();
    }
}

void TreasureRoomState::exitRoom() {
    Serial.println("You leave the treasure room behind...");
}

void TreasureRoomState::drawTreasureScreen() {
    display->clear();
    
    // Title and atmosphere
    display->drawText("TREASURE ROOM", 25, 15, TFT_YELLOW, 2);
    display->drawText("Something glints in", 20, 40, TFT_CYAN);
    display->drawText("the torchlight...", 25, 55, TFT_CYAN);
    
    // Player status
    display->drawText(("Gold: " + String(player->getGold())).c_str(), 
                     10, 80, TFT_YELLOW);
    display->drawText(("HP: " + String(player->getCurrentHP()) + "/" + String(player->getMaxHP())).c_str(), 
                     10, 95, TFT_WHITE);
    display->drawText(("Potions: " + String(player->getHealthPotions())).c_str(), 
                     10, 110, TFT_CYAN);
    
    // Treasure description
    if (!treasureLooted) {
        display->drawText("You see:", 10, 140, TFT_WHITE);
        display->drawText("- A chest of gold", 15, 155, TFT_YELLOW);
        display->drawText("- Health potions", 15, 170, TFT_GREEN);
        display->drawText("- Mysterious gear", 15, 185, TFT_PURPLE);
        
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
    // Generate random treasure
    int goldFound = 30 + (rand() % 40); // 30-70 gold
    int potionsFound = 1 + (rand() % 3); // 1-3 potions
    
    // Give treasure to player
    player->addGold(goldFound);
    player->addHealthPotions(potionsFound);
    
    // Small equipment bonus
    player->addEquipmentBonus(5, 2, 1, 0);
    
    showTreasureResult(goldFound, potionsFound);
    
    treasureLooted = true;
    screenDrawn = false; // Force redraw
    
    Serial.println("Player found treasure: " + String(goldFound) + " gold, " + String(potionsFound) + " potions, +5 HP, +2 ATK, +1 DEF");
}

void TreasureRoomState::showTreasureResult(int gold, int potions) {
    display->clear();
    
    display->drawText("TREASURE FOUND!", 25, 80, TFT_YELLOW, 2);
    display->drawText(("+" + String(gold) + " Gold!").c_str(), 40, 110, TFT_YELLOW);
    display->drawText(("+" + String(potions) + " Potions!").c_str(), 35, 125, TFT_GREEN);
    display->drawText("+5 HP, +2 ATK, +1 DEF", 15, 140, TFT_PURPLE);
    
    display->drawText("Press any button", 25, 170, TFT_CYAN);
    display->drawText("to continue", 40, 185, TFT_CYAN);
    
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