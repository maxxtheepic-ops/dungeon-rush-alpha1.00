#include "ShopRoomState.h"

ShopRoomState::ShopRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) 
    : RoomState(disp, inp, p, e, dm) {
    selectedOption = 0;
    maxOptions = 2; // Buy something, Leave
    screenDrawn = false;
    lastSelectedOption = -1;
}

void ShopRoomState::enterRoom() {
    Serial.println("Welcome to the mysterious shop...");
    selectedOption = 0;
    screenDrawn = false;
    drawShopScreen();
}

void ShopRoomState::handleRoomInteraction() {
    handleShopInput();
    
    // Redraw if selection changed
    if (lastSelectedOption != selectedOption || !screenDrawn) {
        drawShopScreen();
    }
}

void ShopRoomState::exitRoom() {
    Serial.println("You leave the shop behind...");
}

void ShopRoomState::drawShopScreen() {
    display->clear();
    
    // Title and atmosphere
    display->drawText("SHOP", 65, 15, TFT_YELLOW, 2);
    display->drawText("\"Welcome, traveler!\"", 15, 40, TFT_CYAN);
    display->drawText("- Mysterious Merchant", 10, 55, TFT_WHITE, 1);
    
    // Player status
    display->drawText(("Gold: " + String(player->getGold())).c_str(), 
                     10, 80, TFT_YELLOW);
    display->drawText(("HP: " + String(player->getCurrentHP()) + "/" + String(player->getMaxHP())).c_str(), 
                     10, 95, TFT_WHITE);
    
    // Shop items (placeholder)
    display->drawText("Available Items:", 10, 120, TFT_WHITE);
    display->drawText("- Health Potion (25g)", 15, 135, TFT_GREEN);
    display->drawText("- Mystery Item (50g)", 15, 150, TFT_PURPLE);
    display->drawText("- Equipment Upgrade (100g)", 15, 165, TFT_BLUE);
    
    // Menu options
    int yStart = 200;
    int ySpacing = 20;
    const char* options[2] = {"Buy Health Potion", "Leave Shop"};
    
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
    
    // Show affordability
    if (selectedOption == 0 && player->getGold() < 25) {
        display->drawText("(Not enough gold!)", 10, 250, TFT_RED);
    }
    
    // Controls
    display->drawText("UP/DOWN: Navigate", 10, 270, TFT_CYAN, 1);
    display->drawText("A: Select, B: Leave", 10, 285, TFT_CYAN, 1);
    
    screenDrawn = true;
    lastSelectedOption = selectedOption;
}

void ShopRoomState::handleShopInput() {
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
        ShopAction action = static_cast<ShopAction>(selectedOption);
        
        switch (action) {
            case ShopAction::BUY_POTION:
                buyHealthPotion();
                break;
            case ShopAction::LEAVE:
                completeRoom();
                break;
        }
    }
    
    // Quick exit
    if (input->wasPressed(Button::B)) {
        completeRoom();
    }
}

void ShopRoomState::buyHealthPotion() {
    const int POTION_COST = 25;
    
    if (player->getGold() < POTION_COST) {
        showPurchaseResult(false, "Not enough gold!");
        return;
    }
    
    // Make the purchase
    player->spendGold(POTION_COST);
    player->addHealthPotions(1);
    
    showPurchaseResult(true, "Bought Health Potion!");
    Serial.println("Player bought health potion for " + String(POTION_COST) + " gold");
}

void ShopRoomState::showPurchaseResult(bool success, String message) {
    display->clear();
    
    if (success) {
        display->drawText("Purchase Success!", 20, 100, TFT_GREEN, 2);
        display->drawText(message.c_str(), 30, 130, TFT_WHITE);
        display->drawText(("Gold: " + String(player->getGold())).c_str(), 
                         40, 150, TFT_YELLOW);
        display->drawText(("Potions: " + String(player->getHealthPotions())).c_str(), 
                         40, 165, TFT_CYAN);
    } else {
        display->drawText("Cannot Buy!", 35, 100, TFT_RED, 2);
        display->drawText(message.c_str(), 30, 130, TFT_WHITE);
    }
    
    display->drawText("Press any button", 25, 190, TFT_CYAN);
    display->drawText("to continue", 40, 205, TFT_CYAN);
    
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
    
    // Return to shop screen
    screenDrawn = false;
}