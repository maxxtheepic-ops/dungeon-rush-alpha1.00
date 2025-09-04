#include "GameStateManager.h"
#include "../spells/spell.h"

GameStateManager::GameStateManager(Display* disp, Input* inp) {
    display = disp;
    input = inp;
    
    // Initialize shared entities
    player = new Player("Hero");
    currentEnemy = new Enemy();
    dungeonManager = new DungeonManager(player);
    
    // Initialize scroll inventory with a starting scroll for testing
    availableScrolls.clear();
    
    // ADDED: Give player a starting scroll for testing the library feature
    Serial.println("DEBUG: Creating starting scroll...");
    Spell* startingScroll = SpellFactory::createSpell(1); // Fireball scroll
    if (startingScroll) {
        availableScrolls.push_back(startingScroll);
        Serial.println("DEBUG: Added starting scroll for testing: " + startingScroll->getName());
        Serial.println("DEBUG: Total scrolls in GameStateManager: " + String(availableScrolls.size()));
    } else {
        Serial.println("ERROR: Failed to create starting scroll!");
    }
    
    // Initialize states with GameStateManager reference
    mainMenuState = new MainMenuState(display, input);
    doorChoiceState = new DoorChoiceState(display, input, dungeonManager);
    combatRoomState = new CombatRoomState(display, input, player, currentEnemy, dungeonManager);
    libraryRoomState = new LibraryRoomState(display, input, player, currentEnemy, dungeonManager);
    shopRoomState = new ShopRoomState(display, input, player, currentEnemy, dungeonManager);
    treasureRoomState = new TreasureRoomState(display, input, player, currentEnemy, dungeonManager);
    
    // Set GameStateManager references for room states that need global access
    if (libraryRoomState) libraryRoomState->setGameStateManager(this);
    if (treasureRoomState) treasureRoomState->setGameStateManager(this);
    if (combatRoomState) combatRoomState->setGameStateManager(this);
    if (shopRoomState) shopRoomState->setGameStateManager(this);
    
    // Start with main menu
    currentState = mainMenuState;
    handlingGameOver = false;
}

GameStateManager::~GameStateManager() {
    // Clean up scrolls
    clearAllScrolls();
    
    delete player;
    delete currentEnemy;
    delete dungeonManager;
    delete mainMenuState;
    delete doorChoiceState;
    delete combatRoomState;
    delete libraryRoomState;  // CHANGED: Delete library instead of campfire
    delete shopRoomState;
    delete treasureRoomState;
}

void GameStateManager::initialize() {
    Serial.println("=== ESP32 Wizard Dungeon Crawler ===");  // CHANGED: Updated title
    Serial.println("Game State Manager Initialized");
    
    // Check if all states are properly initialized
    Serial.println("DEBUG: Checking state initialization:");
    Serial.println("  mainMenuState: " + String(mainMenuState ? "OK" : "NULL"));
    Serial.println("  doorChoiceState: " + String(doorChoiceState ? "OK" : "NULL"));
    Serial.println("  combatRoomState: " + String(combatRoomState ? "OK" : "NULL"));
    Serial.println("  libraryRoomState: " + String(libraryRoomState ? "OK" : "NULL"));  // CHANGED: Library instead of campfire
    Serial.println("  shopRoomState: " + String(shopRoomState ? "OK" : "NULL"));
    Serial.println("  treasureRoomState: " + String(treasureRoomState ? "OK" : "NULL"));
    
    // DEBUG: Check scroll inventory status
    Serial.println("DEBUG: Initial scroll inventory check:");
    Serial.println("DEBUG: Available scrolls: " + String(availableScrolls.size()));
    for (Spell* scroll : availableScrolls) {
        if (scroll) {
            Serial.println("DEBUG: - " + scroll->getName());
        }
    }
    
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
        else if (currentState == libraryRoomState) currentStateName = "Library";  // CHANGED: Library instead of campfire
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
            
        case StateTransition::LIBRARY:  // CHANGED: Library instead of campfire
            Serial.println("DEBUG: Switching to Library - START");
            if (!libraryRoomState) {
                Serial.println("ERROR: libraryRoomState is NULL!");
                currentState = mainMenuState;
                break;
            }
            // DEBUG: Show scroll count before transfer
            Serial.println("DEBUG: Available scrolls before transfer: " + String(availableScrolls.size()));
            for (Spell* scroll : availableScrolls) {
                if (scroll) Serial.println("DEBUG: - " + scroll->getName());
            }
            
            // Transfer scrolls to library before entering
            transferScrollsToLibrary();
            currentState = libraryRoomState;
            Serial.println("DEBUG: Switching to Library - ASSIGNED");
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
    
    // Explain what happens (wizard theme)
    display->drawText("Your magic failed", 20, 100, TFT_WHITE);
    display->drawText("in the dungeon...", 20, 115, TFT_WHITE);
    
    display->drawText("All progress lost!", 15, 140, TFT_WHITE);
    display->drawText("Spells forgotten!", 20, 155, TFT_WHITE);  // CHANGED: Spells instead of equipment
    
    // Instructions
    display->drawText("Press any button", 10, 180, TFT_WHITE);
    display->drawText("to return to town", 10, 195, TFT_WHITE);
    
    Serial.println("DEBUG: Game over screen drawn");
    Serial.println("=== GAME OVER ===");
    Serial.println("Wizard died - all progress will be reset");
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
        if (libraryRoomState) libraryRoomState->clearTransition();  // CHANGED: Library instead of campfire
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
            display->drawText("Coming Soon!", 30, 120, TFT_WHITE);
            display->drawText("Press A to return", 10, 160, TFT_WHITE);
            
            if (input->wasPressed(Button::A)) {
                currentState = mainMenuState;
                currentState->enter();
            }
            break;
            
        case StateTransition::CREDITS:
            display->drawText("Credits", 50, 80, TFT_WHITE, 2);
            display->drawText("Wizard Dungeon", 20, 120, TFT_WHITE);  // CHANGED: Updated credits
            display->drawText("Crawler v0.2", 30, 135, TFT_WHITE);
            display->drawText("Made with ESP32", 20, 150, TFT_GREEN);
            display->drawText("Press A to return", 10, 180, TFT_WHITE);
            
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
    // Only reset health and mana, keep spells/progress
    player->heal(player->getMaxHP());
    player->restoreAllMana();  // CHANGED: Also restore mana
    player->addHealthPotions(3);
    player->addManaPotions(2);  // CHANGED: Also give mana potions
    Serial.println("Player health and mana restored");
}

void GameStateManager::resetDungeonProgress() {
    // Use the proper reset method instead of recreating
    dungeonManager->resetToFirstFloor();
    Serial.println("Dungeon progress reset - starting from Floor 1");
}

void GameStateManager::fullGameReset() {
    Serial.println("DEBUG: Starting full game reset");
    
    // Clear all scrolls
    clearAllScrolls();
    
    // Reset player to base wizard stats (lose all equipment/progress)
    player->resetToBaseStats();
    player->heal(player->getMaxHP());
    player->restoreAllMana();  // CHANGED: Also restore mana
    player->addHealthPotions(3);
    player->addManaPotions(2);  // CHANGED: Also give mana potions
    
    // Reset dungeon progress
    resetDungeonProgress();
    
    Serial.println("DEBUG: Full game reset complete - fresh wizard start!");
}

// NEW: Global scroll inventory system implementation
void GameStateManager::addScroll(Spell* scroll) {
    if (scroll) {
        availableScrolls.push_back(scroll);
        Serial.println("GameStateManager: Added scroll to global inventory: " + scroll->getName());
        Serial.println("Total scrolls available: " + String(availableScrolls.size()));
    }
}

std::vector<Spell*> GameStateManager::getAvailableScrolls() {
    return availableScrolls;
}

void GameStateManager::removeScroll(int index) {
    if (index >= 0 && index < availableScrolls.size()) {
        Spell* scroll = availableScrolls[index];
        Serial.println("GameStateManager: Removing scroll: " + (scroll ? scroll->getName() : "null"));
        availableScrolls.erase(availableScrolls.begin() + index);
        // Don't delete the scroll here - it should be transferred to player's library
    }
}

void GameStateManager::transferScrollsToLibrary() {
    if (libraryRoomState) {
        Serial.println("GameStateManager: Transferring " + String(availableScrolls.size()) + " scrolls to library");
        
        // Transfer each scroll to the library
        for (Spell* scroll : availableScrolls) {
            if (scroll) {
                libraryRoomState->addAvailableScroll(scroll);
                Serial.println("Transferred scroll: " + scroll->getName());
            }
        }
        
        // Clear the global list (scrolls are now owned by library)
        availableScrolls.clear();
        Serial.println("Global scroll inventory cleared after transfer");
    } else {
        Serial.println("GameStateManager: No library state available for scroll transfer");
    }
}

void GameStateManager::clearAllScrolls() {
    // Clean up all scrolls in global inventory
    for (Spell* scroll : availableScrolls) {
        if (scroll) {
            delete scroll;
        }
    }
    availableScrolls.clear();
    Serial.println("GameStateManager: Cleared all scrolls from global inventory");
}