#ifndef COMBAT_MANAGER_H
#define COMBAT_MANAGER_H

#include "../entities/player.h"
#include "../entities/enemy.h"
#include <Arduino.h>

// Forward declarations
class DamageCalculator;
class TurnQueue;
class CombatTextBox;  // NEW: Forward declaration for text box

enum CombatState {
    COMBAT_CHOOSE_ACTIONS,    // Both choose actions
    COMBAT_EXECUTE_ACTIONS,   // Execute actions in order
    COMBAT_PLAYER_WIN,
    COMBAT_PLAYER_LOSE
};

enum CombatResult {
    RESULT_ONGOING,
    RESULT_VICTORY,
    RESULT_DEFEAT
};

class CombatManager {
private:
    Player* player;
    Enemy* currentEnemy;
    CombatState currentState;
    int turnCounter;
    
    // Action storage
    PlayerAction playerAction;
    EnemyAction enemyAction;
    bool actionsChosen;
    
    // NEW: Text box reference for synergy display
    CombatTextBox* textBox;
    
    // Helper methods
    String getPlayerActionName(PlayerAction action);
    
public:
    // Constructor
    CombatManager();
    
    // Combat management
    void startCombat(Player* p, Enemy* e);
    void endCombat();
    
    // NEW: Set text box for synergy display
    void setTextBox(CombatTextBox* tb) { 
    Serial.println("DEBUG CombatManager::setTextBox() - tb: " + String(tb != nullptr ? "NOT NULL" : "NULL"));
    textBox = tb; 
    Serial.println("DEBUG CombatManager::setTextBox() - textBox member now: " + String(textBox != nullptr ? "NOT NULL" : "NULL"));
}
    
    // Turn processing
    CombatResult processTurn(PlayerAction action);
    
    // Action execution
    void executePlayerAction();
    void executeEnemyAction();
    
    // Combat state
    bool isCombatOver() const;
    CombatResult getCombatResult() const;
    CombatState getCurrentState() const;
    int getTurnCounter() const;
    
    // Entity access
    Player* getPlayer() const;
    Enemy* getCurrentEnemy() const;
    
    // Display helpers
    void printCombatStatus() const;
    
    // Cleanup
    virtual ~CombatManager();
};

#endif