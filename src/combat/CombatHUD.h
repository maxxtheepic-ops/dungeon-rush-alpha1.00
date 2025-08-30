#ifndef COMBAT_HUD_H
#define COMBAT_HUD_H

#include "../graphics/Display.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../combat/combat_manager.h"

class CombatHUD {
private:
    Display* display;
    
    // Layout constants
    static const int SPRITE_AREA_HEIGHT = 200;
    static const int PLAYER_INFO_X = 10;
    static const int ENEMY_INFO_X = 100;
    static const int INFO_START_Y = 20;
    static const int LINE_HEIGHT = 15;
    
    // Drawing helper methods
    void drawPlayerInfo(Player* player);
    void drawEnemyInfo(Enemy* enemy);
    void drawTurnInfo(int turnCounter);
    void drawInventoryInfo(Player* player);
    void clearSpriteArea();
    
public:
    CombatHUD(Display* disp);
    
    // Main drawing functions
    void drawFullCombatScreen(Player* player, Enemy* enemy, int turnCounter);
    void updateCombatStats(Player* player, Enemy* enemy, int turnCounter);
    
    // Result screens
    void drawVictoryScreen();
    void drawDefeatScreen();
    void drawNewCombatPrompt();
    
    // Utility
    void clearCombatArea();
};

#endif