#include "CombatState.h"

CombatGameState::CombatGameState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) : GameState(disp, inp) {
    combatMenu = new CombatMenu(display, input);
    combatManager = new CombatManager();
    combatHUD = new CombatHUD(display);
    
    // Store references to shared entities
    player = p;
    currentEnemy = e;
    dungeonManager = dm;
}

CombatGameState::~CombatGameState() {
    delete combatMenu;
    delete combatManager;
    delete combatHUD;
}

void CombatGameState::enter() {
    Serial.println("Entering Combat State");
    startNewCombat();
}

void CombatGameState::update() {
    if (combatMenu->getIsActive()) {
        handleCombatInput();
    }
}

void CombatGameState::exit() {
    Serial.println("Exiting Combat State");
    combatMenu->deactivate();
    combatManager->endCombat();
}

void CombatGameState::startNewCombat() {
    // Get enemy from current room
    Room* currentRoom = dungeonManager->getCurrentFloor()->getCurrentRoom();
    if (currentRoom) {
        *currentEnemy = currentRoom->createEnemy();
        Serial.println("Combat: Fighting " + currentEnemy->getName() + " in " + currentRoom->getRoomName());
    } else {
        // Fallback to random enemy
        *currentEnemy = Enemy::createRandomEnemy();
        Serial.println("Combat: Fighting random " + currentEnemy->getName());
    }
    
    // Start combat
    combatManager->startCombat(player, currentEnemy);
    
    // Draw initial combat screen
    combatHUD->drawFullCombatScreen(player, currentEnemy, combatManager->getTurnCounter());
    
    // Activate combat menu
    combatMenu->activate();
    combatMenu->render();
    
    Serial.println("=== COMBAT STARTED ===");
    combatManager->printCombatStatus();
}

void CombatGameState::handleCombatInput() {
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
        
        // Process the combat turn
        CombatResult combatResult = combatManager->processTurn(playerAction);
        
        // Update display
        combatHUD->updateCombatStats(player, currentEnemy, combatManager->getTurnCounter());
        
        // Check if combat is over
        if (combatResult == RESULT_VICTORY || combatResult == RESULT_DEFEAT) {
            if (combatResult == RESULT_VICTORY) {
                combatHUD->drawVictoryScreen();
                
                // Mark room as completed and advance dungeon
                dungeonManager->markRoomCompleted();
                Serial.println("Room completed! Returning to door choice...");
                
                // Return to door choice instead of game over
                requestStateChange(StateTransition::DOOR_CHOICE);
            } else {
                combatHUD->drawDefeatScreen();
                requestStateChange(StateTransition::GAME_OVER);
            }
        } else {
            // Combat continues
            combatMenu->activate();
            combatMenu->render();
        }
    }
}