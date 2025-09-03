#include "CombatRoomState.h"

CombatRoomState::CombatRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) 
    : RoomState(disp, inp, p, e, dm) {
    // Use spell combat menu instead of regular combat menu
    spellCombatMenu = new SpellCombatMenu(display, input, p);
    combatManager = new CombatManager();
    combatHUD = new CombatHUD(display);
    combatActive = false;
    showingResultScreen = false;
}

CombatRoomState::~CombatRoomState() {
    delete spellCombatMenu;
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
                // Check if this was a boss room - UPDATED: Auto go to library
                if (currentRoom && currentRoom->getType() == ROOM_BOSS) {
                    Serial.println("=== BOSS DEFEATED ===");
                    Serial.println("Boss defeated! Automatically going to library...");
                    
                    // Mark room as completed first
                    completeRoom();
                    
                    // Advance to next floor
                    dungeonManager->advanceToNextFloor();
                    Serial.println("Advanced to floor: " + String(dungeonManager->getCurrentFloorNumber()));
                    
                    // Go directly to library (automatic reward for defeating boss)
                    requestStateChange(StateTransition::LIBRARY);
                    Serial.println("=== GOING TO LIBRARY ===");
                    return;
                } else {
                    // Regular room - normal completion, return to door choice
                    Serial.println("Regular enemy defeated - completing room");
                    completeRoom(); // This will trigger return to door choice
                }
            } else {
                // Player was defeated - game over
                Serial.println("Player defeated - game over");
                requestStateChange(StateTransition::GAME_OVER);
            }
        }
    } else if (combatActive && spellCombatMenu->getIsActive()) {
        handleCombatInput();
    }
}

void CombatRoomState::exitRoom() {
    Serial.println("Combat completed, exiting room");
    combatActive = false;
    showingResultScreen = false;
    spellCombatMenu->deactivate();
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
    spellCombatMenu->activate();
    spellCombatMenu->render();
    combatActive = true;
    
    Serial.println("=== COMBAT STARTED ===");
    combatManager->printCombatStatus();
}

void CombatRoomState::handleCombatInput() {
    MenuResult result = spellCombatMenu->handleInput();
    spellCombatMenu->render();
    
    if (result == MenuResult::SELECTED) {
        // Convert spell menu selection to PlayerAction
        SpellCombatAction menuAction = spellCombatMenu->getSelectedAction();
        PlayerAction playerAction;
        
        switch (menuAction) {
            case SpellCombatAction::CAST_SPELL_1:
                playerAction = ACTION_CAST_SPELL_1;
                break;
            case SpellCombatAction::CAST_SPELL_2:
                playerAction = ACTION_CAST_SPELL_2;
                break;
            case SpellCombatAction::CAST_SPELL_3:
                playerAction = ACTION_CAST_SPELL_3;
                break;
            case SpellCombatAction::CAST_SPELL_4:
                playerAction = ACTION_CAST_SPELL_4;
                break;
            case SpellCombatAction::DEFEND:
                playerAction = ACTION_DEFEND;
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
            spellCombatMenu->deactivate();
            showingResultScreen = true;

            if (currentRoom && currentRoom->getType() == ROOM_BOSS) {
                Serial.println("Boss defeated! Will go to library after button press...");
            } else {
                Serial.println("Regular enemy defeated!");
            }

            // Wait for player input before processing victory
        } else if (combatResult == RESULT_DEFEAT) {
            combatHUD->drawDefeatScreen();
            combatActive = false;
            spellCombatMenu->deactivate();
            showingResultScreen = true;  // Wait for player input
            
            // Log what type of room we died in
            if (currentRoom) {
                Serial.println("Died in: " + currentRoom->getRoomName());
                if (currentRoom->getType() == ROOM_BOSS) {
                    Serial.println("Death was in boss room!");
                }
            }
        } else {
            // Combat continues
            spellCombatMenu->activate();
            spellCombatMenu->render();
        }
    }
}