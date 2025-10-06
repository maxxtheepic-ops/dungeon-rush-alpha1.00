// src/combat/CombatHUD.h - Updated to support victory image
#ifndef COMBAT_HUD_H
#define COMBAT_HUD_H

#include "../graphics/Display.h"
#include "../entities/player.h"
#include "../entities/enemy.h"

class CombatHUD {
private:
    Display* display;
    
    // Layout constants
    static const int SPRITE_AREA_HEIGHT = 200;  // Top area for sprites and HUD
    static const int PLAYER_INFO_X = 10;
    static const int ENEMY_INFO_X = 100;
    static const int INFO_START_Y = 20;
    static const int LINE_HEIGHT = 15;
    
    // Drawing helper methods
    void drawPlayerInfo(Player* player);
    void drawEnemyInfo(Enemy* enemy);
    void drawTurnInfo(int turnCounter);
    
    // NEW: Victory image drawing methods
    void drawVictoryImage();         // Draw victory image pixel by pixel
    void drawVictoryImageFast();     // Alternative faster method using pushImage
    
    // Area-specific clearing methods
    void clearSpriteAndHUDArea();    // Clear y=0 to y=200 (preserve text box + spell menu)
    void clearHUDInfoArea();         // Clear only HUD info section for updates
    void clearEntireScreen();        // Clear the entire screen including text box and spell menu
    
public:
    CombatHUD(Display* disp);
    
    // Main drawing functions
    void drawFullCombatScreen(Player* player, Enemy* enemy, int turnCounter);
    void updateCombatStats(Player* player, Enemy* enemy, int turnCounter);
    
    // Result screens
    void drawVictoryScreen();        // UPDATED: Now displays image + minimal text
    void drawDefeatScreen();
    void drawNewCombatPrompt();
    
    // Legacy methods (for compatibility)
    void clearCombatArea();          // Clear everything except spell menu
};

#endif