#include "CombatState.h"

CombatGameState::CombatGameState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) : GameState(disp, inp) {
    spellCombatMenu = new SpellCombatMenu(display, input, p);  // CHANGED: Use spell combat menu
    combatManager = new CombatManager();
    combatHUD = new CombatHUD(display);
    
    // Store references to shared entities
    player = p;
    currentEnemy = e;
    dungeonManager = dm;
}

CombatGameState::~CombatGameState() {
    delete spellCombatMenu;
    delete combatManager;
    delete combatHUD;
}

void CombatGameState::enter() {
    Serial.println("Entering Combat State");
    startNewCombat();
}

void CombatGameState::update() {
    if (spellCombatMenu->getIsActive()) {
        handleCombatInput();
    }
}

void CombatGameState::exit() {
    Serial.println("Exiting Combat State");
    spellCombatMenu->deactivate();
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
    
    // Activate spell combat menu
    spellCombatMenu->activate();
    spellCombatMenu->render();
    
    Serial.println("=== COMBAT STARTED ===");
    combatManager->printCombatStatus();
}

void CombatGameState::handleCombatInput() {
    MenuResult result = spellCombatMenu->handleInput();
    spellCombatMenu->render();
    
    if (result == MenuResult::SELECTED) {
        // Convert menu selection to PlayerAction
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
                Serial.println("Room completed!");
                
                // Check if this was a boss room - go to library after boss
                Room* currentRoom = dungeonManager->getCurrentFloor()->getCurrentRoom();
                if (currentRoom && currentRoom->getType() == ROOM_BOSS) {
                    Serial.println("Boss defeated! Going to library...");
                    requestStateChange(StateTransition::LIBRARY);
                } else {
                    Serial.println("Returning to door choice...");
                    requestStateChange(StateTransition::DOOR_CHOICE);
                }
            } else {
                combatHUD->drawDefeatScreen();
                requestStateChange(StateTransition::GAME_OVER);
            }
        } else {
            // Combat continues
            spellCombatMenu->activate();
            spellCombatMenu->render();
        }
    }
}