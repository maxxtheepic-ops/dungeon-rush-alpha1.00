#include "combat_manager.h"
#include "damage_calculator.h"
#include "turn_queue.h"
#include "../utils/constants.h"
#include "../spells/spell.h"  // Include spell.h to get SpellLibrary definition

// Constructor
CombatManager::CombatManager() {
    player = nullptr;
    currentEnemy = nullptr;
    currentState = COMBAT_CHOOSE_ACTIONS;
    turnCounter = 0;
    actionsChosen = false;
    playerAction = ACTION_CAST_SPELL_1;  // CHANGED: Default to spell casting
    enemyAction = ENEMY_ATTACK;
}

// Start combat
void CombatManager::startCombat(Player* p, Enemy* e) {
    player = p;
    currentEnemy = e;
    turnCounter = 1;
    actionsChosen = false;
    currentState = COMBAT_CHOOSE_ACTIONS;
    
    // Clear any existing defense states
    player->resetDefense();
    currentEnemy->resetDefense();
    
    // Clear spell library recent casts for fresh combat
    player->getSpellLibrary()->clearRecentCasts();
    
    Serial.println("Combat begins! " + player->getName() + " vs " + currentEnemy->getName());
}

// End combat and cleanup
void CombatManager::endCombat() {
    if (player) {
        player->resetDefense();
        // Don't clear spell effects here - they should persist between combats
    }
    if (currentEnemy) {
        currentEnemy->resetDefense();
    }
    
    player = nullptr;
    currentEnemy = nullptr;
    currentState = COMBAT_CHOOSE_ACTIONS;
    turnCounter = 0;
    actionsChosen = false;
}

// Process complete turn: choose actions + execute them
CombatResult CombatManager::processTurn(PlayerAction action) {
    if (!player || !currentEnemy || currentState != COMBAT_CHOOSE_ACTIONS) {
        return RESULT_ONGOING;
    }
    
    // Store actions
    playerAction = action;
    enemyAction = currentEnemy->chooseAction();
    actionsChosen = true;
    currentState = COMBAT_EXECUTE_ACTIONS;
    
    // Show choices
    String playerActionName = getPlayerActionName(playerAction);
    String enemyActionName = (enemyAction == ENEMY_ATTACK) ? "ATTACK" : "DEFEND";
    
    Serial.println("CHOICES:");
    Serial.println("  " + player->getName() + " chooses: " + playerActionName);
    Serial.println("  " + currentEnemy->getName() + " chooses: " + enemyActionName);
    Serial.println();
    
    // Create turn queue to determine order (local variable)
    TurnQueue* turnQueue = new TurnQueue(player, currentEnemy, playerAction, enemyAction);
    
    // Show execution order
    Serial.println("EXECUTION ORDER: " + turnQueue->getTurnOrderReason());
    Serial.println();
    Serial.println("ACTIONS:");
    
    // Execute actions in order determined by TurnQueue
    bool playerGoesFirst = turnQueue->doesPlayerGoFirst();
    
    if (playerGoesFirst) {
        executePlayerAction();
        if (!isCombatOver()) {
            executeEnemyAction();
        }
    } else {
        executeEnemyAction();
        if (!isCombatOver()) {
            executePlayerAction();
        }
    }
    
    // Cleanup and prepare for next turn
    delete turnQueue;
    turnQueue = nullptr;
    actionsChosen = false;
    turnCounter++;
    
    if (!isCombatOver()) {
        currentState = COMBAT_CHOOSE_ACTIONS;
    }
    
    return getCombatResult();
}

// Execute player action with spell support
void CombatManager::executePlayerAction() {
    if (!player || !currentEnemy) return;
    
    // Start turn processing
    player->startTurn();
    
    switch(playerAction) {
        case ACTION_CAST_SPELL_1:
        case ACTION_CAST_SPELL_2:
        case ACTION_CAST_SPELL_3:
        case ACTION_CAST_SPELL_4:
            {
                int spellSlot = playerAction - ACTION_CAST_SPELL_1;
                Serial.print("  " + player->getName() + " casts spell from slot " + String(spellSlot + 1) + ": ");
                
                if (player->performCastSpell(spellSlot, currentEnemy)) {
                    // Spell casting is handled in the spell system with proper logging
                    if (!currentEnemy->isAlive()) {
                        currentState = COMBAT_PLAYER_WIN;
                        Serial.println("  " + currentEnemy->getName() + " is defeated by magic!");
                    }
                } else {
                    Serial.println("Spell failed to cast!");
                }
            }
            break;
            
        case ACTION_DEFEND:
            {
                int defenseBonus = player->performDefend();
                Serial.println("  " + player->getName() + " casts a protective ward (+" + String(defenseBonus) + " magical defense)");
            }
            break;
    }
    
    // End turn processing
    player->endTurn();
}

// Execute enemy action using DamageCalculator
void CombatManager::executeEnemyAction() {
    if (!player || !currentEnemy) return;
    
    if (enemyAction == ENEMY_ATTACK) {
        int baseDamage = DamageCalculator::calculateEnemyAttackDamage(currentEnemy);
        int playerDefense = player->getTotalDefense();
        int finalDamage = DamageCalculator::calculateFinalDamage(baseDamage, playerDefense);
        
        Serial.print("  " + currentEnemy->getName() + " attacks for " + String(baseDamage) + " damage");
        if (playerDefense > 0) {
            Serial.print(" (" + String(playerDefense) + " blocked)");
            Serial.print(" = " + String(finalDamage) + " final damage");
        }
        Serial.println();
        
        player->takeDamage(baseDamage);
        
        if (!player->isAlive()) {
            currentState = COMBAT_PLAYER_LOSE;
            Serial.println("  " + player->getName() + " is defeated!");
        }
    } else {
        int defenseBonus = DamageCalculator::calculateEnemyDefenseBonus(currentEnemy);
        currentEnemy->performDefend(); // This adds the defense bonus
        Serial.println("  " + currentEnemy->getName() + " defends for +" + String(defenseBonus) + " defense");
    }
}

// Helper method to get player action name for logging
String CombatManager::getPlayerActionName(PlayerAction action) {
    switch(action) {
        case ACTION_CAST_SPELL_1: return "CAST SPELL 1";
        case ACTION_CAST_SPELL_2: return "CAST SPELL 2";
        case ACTION_CAST_SPELL_3: return "CAST SPELL 3";
        case ACTION_CAST_SPELL_4: return "CAST SPELL 4";
        case ACTION_DEFEND: return "DEFEND";
        default: return "UNKNOWN";
    }
}

// Combat state checks
bool CombatManager::isCombatOver() const {
    return (currentState == COMBAT_PLAYER_WIN || currentState == COMBAT_PLAYER_LOSE);
}

CombatResult CombatManager::getCombatResult() const {
    switch (currentState) {
        case COMBAT_PLAYER_WIN:
            return RESULT_VICTORY;
        case COMBAT_PLAYER_LOSE:
            return RESULT_DEFEAT;
        default:
            return RESULT_ONGOING;
    }
}

// Information getters
CombatState CombatManager::getCurrentState() const {
    return currentState;
}

int CombatManager::getTurnCounter() const {
    return turnCounter;
}

Player* CombatManager::getPlayer() const {
    return player;
}

Enemy* CombatManager::getCurrentEnemy() const {
    return currentEnemy;
}

// Display helper
void CombatManager::printCombatStatus() const {
    if (!player || !currentEnemy) return;
    
    Serial.println("=== Combat Status ===");
    Serial.print(player->getName() + ": ");
    Serial.print(player->getCurrentHP());
    Serial.print("/");
    Serial.print(player->getMaxHP());
    Serial.print(" HP, ");
    Serial.print(player->getCurrentMana());
    Serial.print("/");
    Serial.print(player->getMaxMana());
    Serial.println(" Mana");
    
    Serial.print(currentEnemy->getName() + ": ");
    Serial.print(currentEnemy->getCurrentHP());
    Serial.print("/");
    Serial.print(currentEnemy->getMaxHP());
    Serial.println(" HP");
    
    Serial.print("Turn: ");
    Serial.println(turnCounter);
    Serial.print("Current Turn: ");
    Serial.println(currentState == COMBAT_CHOOSE_ACTIONS ? "Choose Actions" : "Execute Actions");
    
    // Show active spell effects
    auto activeEffects = player->getActiveEffects();
    if (!activeEffects.empty()) {
        Serial.println("Active spell effects: " + String(activeEffects.size()));
    }
    
    Serial.println();
}

// Destructor
CombatManager::~CombatManager() {
    endCombat();
}