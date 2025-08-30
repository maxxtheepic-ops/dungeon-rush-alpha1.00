#include "GameStateManager.h"

GameStateManager::GameStateManager(Display* disp, Input* inp) {
    display = disp;
    input = inp;
    
    // Initialize shared entities
    player = new Player("Hero");
    currentEnemy = new Enemy();
    dungeonManager = new DungeonManager(player);
    
    // Initialize states
    mainMenuState = new MainMenuState(display, input);
    doorChoiceState = new DoorChoiceState(display, input, dungeonManager);
    combatRoomState = new CombatRoomState(display, input, player, currentEnemy, dungeonManager);
    campfireRoomState = new CampfireRoomState(display, input, player, currentEnemy, dungeonManager);
    shopRoomState = new ShopRoomState(display, input, player, currentEnemy, dungeonManager);
    treasureRoomState = new TreasureRoomState(display, input, player, currentEnemy, dungeonManager);
    
    // Start with main menu
    currentState = mainMenuState;
    handlingGameOver = false;
}

GameStateManager::~GameStateManager() {
    delete player;
    delete currentEnemy;
    delete dungeonManager;
    delete mainMenuState;
    delete doorChoiceState;
    delete combatRoomState;
    delete campfireRoomState;
    delete shopRoomState;
    delete treasureRoomState;
}

void GameStateManager::initialize() {
    Serial.println("=== ESP32 Dungeon Crawler ===");
    Serial.println("Game State Manager Initialized");
    
    // Check if all states are properly initialized
    Serial.println("DEBUG: Checking state initialization:");
    Serial.println("  mainMenuState: " + String(mainMenuState ? "OK" : "NULL"));
    Serial.println("  doorChoiceState: " + String(doorChoiceState ? "OK" : "NULL"));
    Serial.println("  combatRoomState: " + String(combatRoomState ? "OK" : "NULL"));
    Serial.println("  campfireRoomState: " + String(campfireRoomState ? "OK" : "NULL"));
    Serial.println("  shopRoomState: " + String(shopRoomState ? "OK" : "NULL"));
    Serial.println("  treasureRoomState: " + String(treasureRoomState ? "OK" : "NULL"));
    
    // Enter initial state
    if (currentState) {
        Serial.println("DEBUG: Entering initial state (Main Menu)");
        currentState->enter();
    } else {
        Serial.println("ERROR: currentState is NULL during initialization!");
    }
}

void GameStateManager::update() {
    // Handle game over screen specially
    if (handlingGameOver) {
        handleGameOverScreen();
        return;
    }
    
    // Update current state
    currentState->update();
    
    // Check for state transitions
    StateTransition nextState = currentState->getNextState();
    if (nextState != StateTransition::NONE) {
        // DEBUG: Log what type of state is requesting the transition
        String currentStateName = "Unknown";
        if (currentState == mainMenuState) currentStateName = "MainMenu";
        else if (currentState == doorChoiceState) currentStateName = "DoorChoice";
        else if (currentState == combatRoomState) currentStateName = "Combat";
        else if (currentState == campfireRoomState) currentStateName = "Campfire";
        else if (currentState == shopRoomState) currentStateName = "Shop";
        else if (currentState == treasureRoomState) currentStateName = "Treasure";
        
        Serial.println("DEBUG: State transition requested by: " + currentStateName + " -> " + String((int)nextState));
        
        changeState(nextState);
        currentState->clearTransition();
    }
}

void GameStateManager::changeState(StateTransition newState) {
    Serial.println("DEBUG: Changing to state: " + String((int)newState));
    
    // Handle game over specially - DON'T exit current state yet
    if (newState == StateTransition::GAME_OVER) {
        Serial.println("DEBUG: Game over triggered - showing game over screen");
        showGameOverScreen();
        handlingGameOver = true;
        return;
    }
    
    // Exit current state
    Serial.println("DEBUG: Exiting current state");
    currentState->exit();
    
    // Change to new state
    switch (newState) {
        case StateTransition::MAIN_MENU:
            Serial.println("DEBUG: Switching to Main Menu");
            currentState = mainMenuState;
            break;
            
        case StateTransition::DOOR_CHOICE:
            Serial.println("DEBUG: Switching to Door Choice");
            currentState = doorChoiceState;
            break;
            
        case StateTransition::COMBAT:
            Serial.println("DEBUG: Switching to Combat");
            currentState = combatRoomState;
            break;
            
        case StateTransition::CAMPFIRE:
            Serial.println("DEBUG: Switching to Campfire - START");
            if (!campfireRoomState) {
                Serial.println("ERROR: campfireRoomState is NULL!");
                currentState = mainMenuState;
                break;
            }
            currentState = campfireRoomState;
            Serial.println("DEBUG: Switching to Campfire - ASSIGNED");
            break;
            
        case StateTransition::SHOP:
            Serial.println("DEBUG: Switching to Shop");
            if (!shopRoomState) {
                Serial.println("ERROR: shopRoomState is NULL!");
                currentState = mainMenuState;
                break;
            }
            currentState = shopRoomState;
            break;
            
        case StateTransition::TREASURE:
            Serial.println("DEBUG: Switching to Treasure Room");
            if (!treasureRoomState) {
                Serial.println("ERROR: treasureRoomState is NULL!");
                currentState = mainMenuState;
                break;
            }
            currentState = treasureRoomState;
            break;
            
        case StateTransition::SETTINGS:
        case StateTransition::CREDITS:
            handlePlaceholderState(newState);
            return; // Don't change state, just show screen
            
        default:
            Serial.println("DEBUG: Unknown state transition, going to main menu");
            currentState = mainMenuState;
            break;
    }
    
    // Enter new state
    Serial.println("DEBUG: About to enter new state");
    if (currentState) {
        currentState->enter();
        Serial.println("DEBUG: State change complete");
    } else {
        Serial.println("ERROR: currentState is NULL after assignment!");
    }
}

void GameStateManager::showGameOverScreen() {
    Serial.println("DEBUG: Drawing game over screen");
    
    // Force clear screen and redraw
    display->clear();
    delay(50); // Small delay to ensure clear completes
    
    // Large game over text
    display->drawText("GAME OVER", 30, 60, TFT_RED, 2);
    
    // Explain what happens
    display->drawText("You have fallen in", 10, 100, TFT_WHITE);
    display->drawText("the dungeon...", 20, 115, TFT_WHITE);
    
    display->drawText("All progress lost!", 15, 140, TFT_YELLOW);
    display->drawText("Equipment lost!", 20, 155, TFT_YELLOW);
    
    // Instructions
    display->drawText("Press any button", 10, 180, TFT_CYAN);
    display->drawText("to return to town", 10, 195, TFT_CYAN);
    
    Serial.println("DEBUG: Game over screen drawn");
    Serial.println("=== GAME OVER ===");
    Serial.println("Player died - all progress will be reset");
}

void GameStateManager::handleGameOverScreen() {
    // Check for any button press
    bool anyButtonPressed = input->wasPressed(Button::UP) || 
                           input->wasPressed(Button::DOWN) ||
                           input->wasPressed(Button::A) || 
                           input->wasPressed(Button::B);
    
    if (anyButtonPressed) {
        Serial.println("DEBUG: Button pressed on game over screen - resetting game");
        handlingGameOver = false;
        
        // AGGRESSIVE CLEANUP: Clear all possible state transitions
        if (currentState) {
            currentState->exit();
            currentState->clearTransition();
        }
        if (mainMenuState) mainMenuState->clearTransition();
        if (doorChoiceState) doorChoiceState->clearTransition();
        if (combatRoomState) combatRoomState->clearTransition();
        if (campfireRoomState) campfireRoomState->clearTransition();
        if (shopRoomState) shopRoomState->clearTransition();
        if (treasureRoomState) treasureRoomState->clearTransition();
        
        Serial.println("DEBUG: All state transitions cleared");
        
        fullGameReset();  // Reset all progress on death
        currentState = mainMenuState;
        
        // Make sure main menu starts clean
        currentState->clearTransition();
        currentState->enter();
        
        Serial.println("DEBUG: Game reset complete, returned to main menu");
        return;
    }
    
    // Only show "waiting" message occasionally to reduce spam
    static unsigned long lastWaitingMessage = 0;
    if (millis() - lastWaitingMessage > 2000) { // Every 2 seconds instead of 1
        Serial.println("DEBUG: Waiting for button press on game over screen...");
        lastWaitingMessage = millis();
    }
}

void GameStateManager::handlePlaceholderState(StateTransition state) {
    // Handle simple placeholder screens that return to main menu
    display->clear();
    
    switch (state) {
        case StateTransition::SETTINGS:
            display->drawText("Settings", 50, 80, TFT_WHITE, 2);
            display->drawText("Coming Soon!", 30, 120, TFT_YELLOW);
            display->drawText("Press A to return", 10, 160, TFT_CYAN);
            
            if (input->wasPressed(Button::A)) {
                currentState = mainMenuState;
                currentState->enter();
            }
            break;
            
        case StateTransition::CREDITS:
            display->drawText("Credits", 50, 80, TFT_WHITE, 2);
            display->drawText("Made with ESP32", 20, 120, TFT_GREEN);
            display->drawText("& TFT_eSPI", 35, 140, TFT_GREEN);
            display->drawText("Press A to return", 10, 180, TFT_CYAN);
            
            if (input->wasPressed(Button::A)) {
                currentState = mainMenuState;
                currentState->enter();
            }
            break;
            
        default:
            break;
    }
}

void GameStateManager::resetPlayer() {
    // Only reset health and potions, keep equipment/progress
    player->heal(player->getMaxHP());
    player->addHealthPotions(3);
    Serial.println("Player health restored");
}

void GameStateManager::resetDungeonProgress() {
    // Use the proper reset method instead of recreating
    dungeonManager->resetToFirstFloor();
    Serial.println("Dungeon progress reset - starting from Floor 1");
}

void GameStateManager::fullGameReset() {
    Serial.println("DEBUG: Starting full game reset");
    
    // Reset player to base stats (lose all equipment/progress)
    player->resetToBaseStats();
    player->heal(player->getMaxHP());
    player->addHealthPotions(3);
    
    // Reset dungeon progress
    resetDungeonProgress();
    
    Serial.println("DEBUG: Full game reset complete - fresh start!");
}