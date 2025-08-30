#include "CampfireRoomState.h"

CampfireRoomState::CampfireRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) 
    : GameState(disp, inp) {
    player = p;
    currentEnemy = e;
    dungeonManager = dm;
    selectedOption = 0;
    maxOptions = 3;
    screenDrawn = false;
    lastSelectedOption = -1;
    showingInventory = false;
    inventorySelectedItem = 0;
    inventoryMaxItems = 0;
    showingRestResult = false;
}

void CampfireRoomState::enter() {
    Serial.println("DEBUG: Entering Campfire Room - START");
    selectedOption = 0;
    showingInventory = false;
    showingRestResult = false;
    screenDrawn = false;
    lastSelectedOption = -1;
    Serial.println("DEBUG: About to draw campfire menu");
    drawCampfireMenu();
    Serial.println("DEBUG: Campfire room entered successfully - END");
}

void CampfireRoomState::update() {
    // Simple state machine - no complex logic
    if (showingRestResult) {
        // Any button press returns to menu
        if (input->wasPressed(Button::UP) || 
            input->wasPressed(Button::DOWN) ||
            input->wasPressed(Button::A) || 
            input->wasPressed(Button::B)) {
            
            Serial.println("DEBUG: Button pressed on rest result - returning to menu");
            showingRestResult = false;
            screenDrawn = false;
            return;
        }
    } else if (showingInventory) {
        // B button returns to menu
        if (input->wasPressed(Button::B)) {
            Serial.println("DEBUG: B pressed in inventory - returning to menu");
            showingInventory = false;
            screenDrawn = false;
            return;
        }
    } else {
        // Main menu handling
        handleMenuInput();
    }
    
    // Redraw if needed
    if (!showingInventory && !showingRestResult && !screenDrawn) {
        Serial.println("DEBUG: Redrawing campfire menu");
        drawCampfireMenu();
    }
}

void CampfireRoomState::exit() {
    Serial.println("DEBUG: Exiting campfire room - cleaning up state");
    showingInventory = false;
    showingRestResult = false;
    screenDrawn = false;
}

void CampfireRoomState::drawCampfireMenu() {
    Serial.println("DEBUG: Drawing campfire menu - START");
    
    display->clear();
    
    // Title
    display->drawText("Campfire", 50, 15, TFT_YELLOW, 2);
    display->drawText("A warm, safe place", 20, 35, TFT_ORANGE);
    
    // Player status
    display->drawText(("HP: " + String(player->getCurrentHP()) + "/" + String(player->getMaxHP())).c_str(), 
                     10, 55, TFT_WHITE);
    display->drawText(("Gold: " + String(player->getGold())).c_str(), 
                     10, 70, TFT_YELLOW);
    
    // Menu options
    int yStart = 100;
    int ySpacing = 25;
    const char* options[3] = {"Rest (20g)", "Inventory", "Leave"};
    
    for (int i = 0; i < maxOptions; i++) {
        int yPos = yStart + (i * ySpacing);
        
        if (i == selectedOption) {
            display->fillRect(5, yPos - 3, 160, 20, TFT_BLUE);
            display->drawText(">", 10, yPos, TFT_YELLOW);
            display->drawText(options[i], 25, yPos, TFT_WHITE);
        } else {
            display->drawText(options[i], 25, yPos, TFT_WHITE);
        }
    }
    
    // Special case: show if can't afford rest
    if (selectedOption == 0 && player->getGold() < REST_COST) {
        display->drawText("(Not enough gold!)", 10, 200, TFT_RED);
    }
    
    // Controls
    display->drawText("UP/DOWN: Navigate", 10, 270, TFT_CYAN, 1);
    display->drawText("A: Select, B: Leave", 10, 285, TFT_CYAN, 1);
    
    screenDrawn = true;
    lastSelectedOption = selectedOption;
    
    Serial.println("DEBUG: Drawing campfire menu - END");
}

void CampfireRoomState::drawInventoryScreen() {
    display->clear();
    display->drawText("Inventory System", 20, 60, TFT_CYAN, 2);
    display->drawText("Coming Soon!", 35, 100, TFT_YELLOW);
    display->drawText("B: Return", 50, 200, TFT_CYAN);
}

void CampfireRoomState::drawRestResultScreen(bool success, String message) {
    display->clear();
    
    if (success) {
        display->drawText("Rest Complete", 25, 80, TFT_GREEN, 2);
        display->drawText("HP Restored!", 30, 110, TFT_WHITE);
        display->drawText(("Gold: " + String(player->getGold())).c_str(), 40, 130, TFT_YELLOW);
    } else {
        display->drawText("Cannot Rest", 30, 80, TFT_RED, 2);
        display->drawText(message.c_str(), 20, 110, TFT_WHITE);
    }
    
    display->drawText("Press any button", 20, 160, TFT_CYAN);
    display->drawText("to continue", 35, 175, TFT_CYAN);
}

void CampfireRoomState::handleMenuInput() {
    // Navigation
    if (input->wasPressed(Button::UP)) {
        Serial.println("DEBUG: UP pressed");
        selectedOption--;
        if (selectedOption < 0) {
            selectedOption = maxOptions - 1;
        }
        screenDrawn = false; // Force redraw
        return;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        Serial.println("DEBUG: DOWN pressed");
        selectedOption++;
        if (selectedOption >= maxOptions) {
            selectedOption = 0;
        }
        screenDrawn = false; // Force redraw
        return;
    }
    
    // Selection
    if (input->wasPressed(Button::A)) {
        Serial.println("DEBUG: A pressed, option: " + String(selectedOption));
        
        switch (selectedOption) {
            case 0: // Rest
                Serial.println("DEBUG: REST selected");
                performRest();
                break;
            case 1: // Inventory
                Serial.println("DEBUG: INVENTORY selected");
                showingInventory = true;
                drawInventoryScreen();
                break;
            case 2: // Leave
                Serial.println("DEBUG: LEAVE selected - completing room");
                completeRoom();
                return;
        }
    }
    
    // Quick exit
    if (input->wasPressed(Button::B)) {
        Serial.println("DEBUG: B pressed - leaving campfire");
        completeRoom();
        return;
    }
}

void CampfireRoomState::performRest() {
    Serial.println("DEBUG: performRest called");
    
    if (player->getGold() < REST_COST) {
        Serial.println("DEBUG: Not enough gold for rest");
        drawRestResultScreen(false, "Need 20 gold to rest");
        showingRestResult = true;
        return;
    }
    
    if (player->getCurrentHP() >= player->getMaxHP()) {
        Serial.println("DEBUG: Already at full health");
        drawRestResultScreen(false, "Already at full health");
        showingRestResult = true;
        return;
    }
    
    // Perform rest
    Serial.println("DEBUG: Performing rest - spending gold and healing");
    player->spendGold(REST_COST);
    player->heal(player->getMaxHP());
    
    drawRestResultScreen(true, "");
    showingRestResult = true;
    
    Serial.println("DEBUG: Rest complete - player healed");
}

void CampfireRoomState::openInventory() {
    // Simple placeholder
    showingInventory = true;
    drawInventoryScreen();
}

void CampfireRoomState::useSelectedItem() {
    // Placeholder
}

void CampfireRoomState::handleRestResultInput() {
    // This method is no longer used - handled in main update loop
}

void CampfireRoomState::returnToMenu() {
    showingInventory = false;
    showingRestResult = false;
    screenDrawn = false;
}

void CampfireRoomState::completeRoom() {
    // Mark room as complete and return to door choice
    Serial.println("DEBUG: Campfire complete - returning to door choice");
    requestStateChange(StateTransition::DOOR_CHOICE);
}