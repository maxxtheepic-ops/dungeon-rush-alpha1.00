#ifndef TURN_QUEUE_H
#define TURN_QUEUE_H

#include "../entities/player.h"
#include "../entities/enemy.h"

enum ActionPriority {
    PRIORITY_DEFEND = 0,      // Defend always goes first
    PRIORITY_ITEM = 1,        // Items go second
    PRIORITY_ATTACK = 2       // Attacks go last (speed-based)
};

struct TurnAction {
    bool isPlayer;            // true = player action, false = enemy action
    PlayerAction playerAction;
    EnemyAction enemyAction;
    ActionPriority priority;
    int speed;
    
    TurnAction(bool player, PlayerAction pAction, int spd) 
        : isPlayer(player), playerAction(pAction), priority(getPlayerActionPriority(pAction)), speed(spd) {}
    
    TurnAction(bool player, EnemyAction eAction, int spd) 
        : isPlayer(player), enemyAction(eAction), priority(getEnemyActionPriority(eAction)), speed(spd) {}
        
private:
    static ActionPriority getPlayerActionPriority(PlayerAction action);
    static ActionPriority getEnemyActionPriority(EnemyAction action);
};

class TurnQueue {
private:
    TurnAction playerTurn;
    TurnAction enemyTurn;
    bool playerGoesFirst;
    
public:
    // Constructor
    TurnQueue(Player* player, Enemy* enemy, PlayerAction pAction, EnemyAction eAction);
    
    // Turn order calculation
    void calculateTurnOrder();
    bool doesPlayerGoFirst() const;
    
    // Priority helpers
    static ActionPriority getActionPriority(PlayerAction action);
    static ActionPriority getActionPriority(EnemyAction action);
    
    // Turn order explanation (for display)
    String getTurnOrderReason() const;
    
    // Getters
    TurnAction getFirstAction() const;
    TurnAction getSecondAction() const;
};

#endif