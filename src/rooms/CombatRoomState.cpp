#include "CombatRoomState.h"

CombatRoomState::CombatRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) 
    : RoomState(disp, inp, p, e, dm) {
    combatMenu = new CombatMenu(display, input);
    combatManager = new CombatManager();
    combatHUD = new CombatHUD(display);
    combatActive = false;
    showingResultScreen = false;
}

CombatRoomState::~CombatRoomState() {
    delete combatMenu;
    delete combatManager;
    delete combatHUD;
}

void CombatRoomState::enterRoom() {
    Serial.println("Starting combat in " + currentRoom->getRoomName());
    startCombat();
}

void CombatRoomState::handleRoomInteraction() {
    if (showingResultScreen) {
        // Wait for player input to continue
        if (input->wasPressed(Button::UP) || 
            input->wasPressed(Button::DOWN) ||
            input->wasPressed(Button::A) || 
            input->wasPressed(Button::B)) {
            
            showingResultScreen = false;
            
            // Now process the result
            if (combatManager->getCombatResult() == RESULT_VICTORY) {
                // Check if this was a boss room
                if (currentRoom && currentRoom->getType() == ROOM_BOSS) {
                    Serial.println("=== BOSS DEFEATED ===");
                    Serial.println("Advancing to next floor!");
                    Serial.println("Current floor: " + String(dungeonManager->getCurrentFloorNumber()));
                    
                    // Advance to next floor instead of just completing room
                    dungeonManager->advanceToNextFloor();
                    
                    Serial.println("Advanced to floor: " + String(dungeonManager->getCurrentFloorNumber()));
                    Serial.println("===================");
                    
                    // Return to door choice with new floor
                    requestStateChange(StateTransition::DOOR_CHOICE);
                } else {
                    // Regular room - normal completion
                    Serial.println("Regular enemy defeated - completing room");
                    completeRoom(); // This will trigger return to door choice
                }
            } else {
                // Player was defeated - game over
                Serial.println("Player defeated - game over");
                requestStateChange(StateTransition::GAME_OVER);
            }
        }
    } else if (combatActive && combatMenu->getIsActive()) {
        handleCombatInput();
    }
}

void CombatRoomState::exitRoom() {
    Serial.println("Combat completed, exiting room");
    combatActive = false;
    showingResultScreen = false;
    combatMenu->deactivate();
    combatManager->endCombat();
}

void CombatRoomState::startCombat() {
    // Create enemy from room
    if (currentRoom) {
        *currentEnemy = currentRoom->createEnemy();
        Serial.println("Combat: Fighting " + currentEnemy->getName());
    } else {
        *currentEnemy = Enemy::createRandomEnemy();
        Serial.println("Combat: Fighting random " + currentEnemy->getName());
    }
    
    // Start combat systems
    combatManager->startCombat(player, currentEnemy);
    combatHUD->drawFullCombatScreen(player, currentEnemy, combatManager->getTurnCounter());
    combatMenu->activate();
    combatMenu->render();
    combatActive = true;
    
    Serial.println("=== COMBAT STARTED ===");
    combatManager->printCombatStatus();
}

void CombatRoomState::handleCombatInput() {
    MenuResult result = combatMenu->handleInput();
    combatMenu->render();
    
    if (result == MenuResult::SELECTED) {
        // Convert menu selection to PlayerAction
        CombatAction menuAction = combatMenu->getSelectedAction();
        PlayerAction playerAction;
        
        switch (menuAction) {
            case CombatAction::ATTACK:
                playerAction = ACTION_ATTACK;
                break;
            case CombatAction::DEFEND:
                playerAction = ACTION_DEFEND;
                break;
            case CombatAction::ITEM:
                playerAction = ACTION_USE_ITEM;
                break;
        }
        
        // Process combat turn
        CombatResult combatResult = combatManager->processTurn(playerAction);
        
        // Update display
        combatHUD->updateCombatStats(player, currentEnemy, combatManager->getTurnCounter());
        
        // Check if combat is over
        if (combatResult == RESULT_VICTORY) {
                combatHUD->drawVictoryScreen();
                combatActive = false;
             combatMenu->deactivate();
             showingResultScreen = true;
    
                // NEW: Check if this was a boss room - if so, advance floor immediately
            if (currentRoom && currentRoom->getType() == ROOM_BOSS) {
                Serial.println("Boss defeated! Advancing to next floor!");
                // Don't call completeRoom() for boss - handle specially
             } else {
            // Regular room completion
             Serial.println("Regular enemy defeated!");
                }

            
            // Don't call completeRoom() yet - wait for player input
        } else if (combatResult == RESULT_DEFEAT) {
            combatHUD->drawDefeatScreen();
            combatActive = false;
            combatMenu->deactivate();
            showingResultScreen = true;  // Wait for player input
            
            // Log what type of room we died in
            if (currentRoom) {
                Serial.println("Died in: " + currentRoom->getRoomName());
                if (currentRoom->getType() == ROOM_BOSS) {
                    Serial.println("Death was in boss room!");
                }
            }
            
            // Don't call requestStateChange() yet - wait for player input
        } else {
            // Combat continues
            combatMenu->activate();
            combatMenu->render();
        }
    }
}