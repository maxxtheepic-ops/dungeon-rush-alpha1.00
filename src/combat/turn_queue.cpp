#include "turn_queue.h"

// Static helper functions for TurnAction
ActionPriority TurnAction::getPlayerActionPriority(PlayerAction action) {
    return TurnQueue::getActionPriority(action);
}

ActionPriority TurnAction::getEnemyActionPriority(EnemyAction action) {
    return TurnQueue::getActionPriority(action);
}

// Constructor
TurnQueue::TurnQueue(Player* player, Enemy* enemy, PlayerAction pAction, EnemyAction eAction)
    : playerTurn(true, pAction, player->getSpeed()),
      enemyTurn(false, eAction, enemy->getSpeed()) {
    calculateTurnOrder();
}

// Calculate who goes first based on priority and speed
void TurnQueue::calculateTurnOrder() {
    // Compare priorities first (lower number = higher priority)
    if (playerTurn.priority < enemyTurn.priority) {
        playerGoesFirst = true;
    } else if (enemyTurn.priority < playerTurn.priority) {
        playerGoesFirst = false;
    } else {
        // Same priority - use speed (higher speed wins, player wins ties)
        playerGoesFirst = (playerTurn.speed >= enemyTurn.speed);
    }
}

// Check if player goes first
bool TurnQueue::doesPlayerGoFirst() const {
    return playerGoesFirst;
}

// Get priority for player actions
ActionPriority TurnQueue::getActionPriority(PlayerAction action) {
    switch(action) {
        case ACTION_DEFEND:
            return PRIORITY_DEFEND;
        case ACTION_USE_ITEM:
            return PRIORITY_ITEM;
        case ACTION_ATTACK:
            return PRIORITY_ATTACK;
        default:
            return PRIORITY_ATTACK;
    }
}

// Get priority for enemy actions
ActionPriority TurnQueue::getActionPriority(EnemyAction action) {
    switch(action) {
        case ENEMY_DEFEND:
            return PRIORITY_DEFEND;
        case ENEMY_ATTACK:
            return PRIORITY_ATTACK;
        default:
            return PRIORITY_ATTACK;
    }
}

// Get explanation for turn order (for display)
String TurnQueue::getTurnOrderReason() const {
    if (playerTurn.priority < enemyTurn.priority) {
        return "Player goes first (priority)";
    } else if (enemyTurn.priority < playerTurn.priority) {
        return "Enemy goes first (priority)";
    } else {
        // Same priority
        if (playerTurn.speed > enemyTurn.speed) {
            return "Player goes first (speed: " + String(playerTurn.speed) + " vs " + String(enemyTurn.speed) + ")";
        } else if (enemyTurn.speed > playerTurn.speed) {
            return "Enemy goes first (speed: " + String(enemyTurn.speed) + " vs " + String(playerTurn.speed) + ")";
        } else {
            return "Player goes first (speed tie)";
        }
    }
}

// Get the first action to execute
TurnAction TurnQueue::getFirstAction() const {
    return playerGoesFirst ? playerTurn : enemyTurn;
}

// Get the second action to execute
TurnAction TurnQueue::getSecondAction() const {
    return playerGoesFirst ? enemyTurn : playerTurn;
}