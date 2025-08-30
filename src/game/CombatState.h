#ifndef COMBAT_GAME_STATE_H
#define COMBAT_GAME_STATE_H

#include "GameState.h"
#include "../menus/CombatMenu.h"
#include "../combat/combat_manager.h"
#include "../combat/CombatHUD.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../dungeon/DungeonManager.h"

class CombatGameState : public GameState {
private:
    CombatMenu* combatMenu;
    CombatManager* combatManager;
    CombatHUD* combatHUD;
    
    // References to shared game entities
    Player* player;
    Enemy* currentEnemy;
    DungeonManager* dungeonManager;
    
public:
    CombatGameState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm);
    ~CombatGameState();
    
    void enter() override;
    void update() override;
    void exit() override;
    
private:
    void startNewCombat();
    void handleCombatInput();
};

#endif