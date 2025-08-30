#include "combat_manager.h"
#include "damage_calculator.h"
#include "turn_queue.h"
#include "../utils/constants.h"

// Constructor
CombatManager::CombatManager() {
    player = nullptr;
    currentEnemy = nullptr;
    currentState = COMBAT_CHOOSE_ACTIONS;
    turnCounter = 0;
    actionsChosen = false;
    playerAction = ACTION_ATTACK;
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
    
    Serial.println("Combat begins! " + player->getName() + " vs " + currentEnemy->getName());
}

// End combat and cleanup
void CombatManager::endCombat() {
    if (player) {
        player->resetDefense();
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
    String playerActionName = "";
    switch(playerAction) {
        case ACTION_ATTACK: playerActionName = "ATTACK"; break;
        case ACTION_DEFEND: playerActionName = "DEFEND"; break;
        case ACTION_USE_ITEM: playerActionName = "USE ITEM"; break;
    }
    
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

// Execute player action using DamageCalculator
void CombatManager::executePlayerAction() {
    if (!player || !currentEnemy) return;
    
    switch(playerAction) {
        case ACTION_ATTACK:
            {
                int baseDamage = DamageCalculator::calculatePlayerAttackDamage(player);
                int enemyDefense = currentEnemy->getTotalDefense();
                int finalDamage = DamageCalculator::calculateFinalDamage(baseDamage, enemyDefense);
                
                Serial.print("  " + player->getName() + " attacks for " + String(baseDamage) + " damage");
                if (enemyDefense > 0) {
                    Serial.print(" (" + String(enemyDefense) + " blocked)");
                    Serial.print(" = " + String(finalDamage) + " final damage");
                }
                Serial.println();
                
                currentEnemy->takeDamage(baseDamage);
                
                if (!currentEnemy->isAlive()) {
                    currentState = COMBAT_PLAYER_WIN;
                    Serial.println("  " + currentEnemy->getName() + " is defeated!");
                }
            }
            break;
            
        case ACTION_DEFEND:
            {
                int defenseBonus = DamageCalculator::calculatePlayerDefenseBonus(player);
                player->performDefend(); // This adds the defense bonus
                Serial.println("  " + player->getName() + " defends for +" + String(defenseBonus) + " defense");
            }
            break;
            
        case ACTION_USE_ITEM:
            {
                int oldHP = player->getCurrentHP();
                if (player->performUseItem()) {
                    int healed = player->getCurrentHP() - oldHP;
                    Serial.println("  " + player->getName() + " uses health potion! (+" + String(healed) + " HP)");
                } else {
                    Serial.println("  " + player->getName() + " has no items to use!");
                }
            }
            break;
    }
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
    Serial.println(" HP");
    
    Serial.print(currentEnemy->getName() + ": ");
    Serial.print(currentEnemy->getCurrentHP());
    Serial.print("/");
    Serial.print(currentEnemy->getMaxHP());
    Serial.println(" HP");
    
    Serial.print("Turn: ");
    Serial.println(turnCounter);
    Serial.print("Current Turn: ");
    Serial.println(currentState == COMBAT_CHOOSE_ACTIONS ? "Choose Actions" : "Execute Actions");
    Serial.println();
}

// Destructor
CombatManager::~CombatManager() {
    endCombat();
}