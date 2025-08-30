#include "CombatHUD.h"
#include "../utils/constants.h"

CombatHUD::CombatHUD(Display* disp) {
    display = disp;
}

void CombatHUD::drawFullCombatScreen(Player* player, Enemy* enemy, int turnCounter) {
    clearSpriteArea();
    drawPlayerInfo(player);
    drawEnemyInfo(enemy);
    drawTurnInfo(turnCounter);
    drawInventoryInfo(player);
}

void CombatHUD::updateCombatStats(Player* player, Enemy* enemy, int turnCounter) {
    // Only update the stats area, not the whole screen
    display->fillRect(0, INFO_START_Y, Display::WIDTH, 120, TFT_BLACK);
    drawPlayerInfo(player);
    drawEnemyInfo(enemy);
    drawTurnInfo(turnCounter);
    drawInventoryInfo(player);
}

void CombatHUD::clearSpriteArea() {
    display->fillRect(0, 0, Display::WIDTH, SPRITE_AREA_HEIGHT, TFT_BLACK);
}

void CombatHUD::clearCombatArea() {
    display->fillRect(0, 0, Display::WIDTH, 240, TFT_BLACK);
}

void CombatHUD::drawPlayerInfo(Player* player) {
    int y = INFO_START_Y;
    
    // Player name in green
    display->drawText("HERO", PLAYER_INFO_X, y, TFT_GREEN);
    y += LINE_HEIGHT;
    
    // Health with color coding
    String hpText = "HP: " + String(player->getCurrentHP()) + "/" + String(player->getMaxHP());
    uint16_t hpColor = (player->getCurrentHP() < player->getMaxHP() / 3) ? TFT_RED : TFT_WHITE;
    display->drawText(hpText.c_str(), PLAYER_INFO_X, y, hpColor);
    y += LINE_HEIGHT;
    
    // Attack stat
    display->drawText(("ATK: " + String(player->getAttack())).c_str(), 
                     PLAYER_INFO_X, y, TFT_WHITE);
    y += LINE_HEIGHT;
    
    // Defense stat (show total defense including temporary)
    String defText = "DEF: " + String(player->getTotalDefense());
    uint16_t defColor = player->getIsDefending() ? TFT_BLUE : TFT_WHITE;
    display->drawText(defText.c_str(), PLAYER_INFO_X, y, defColor);
}

void CombatHUD::drawEnemyInfo(Enemy* enemy) {
    int y = INFO_START_Y;
    
    // Enemy name in red
    display->drawText(enemy->getName().c_str(), ENEMY_INFO_X, y, TFT_RED);
    y += LINE_HEIGHT;
    
    // Health with color coding
    String hpText = "HP: " + String(enemy->getCurrentHP()) + "/" + String(enemy->getMaxHP());
    uint16_t hpColor = (enemy->getCurrentHP() < enemy->getMaxHP() / 3) ? TFT_RED : TFT_WHITE;
    display->drawText(hpText.c_str(), ENEMY_INFO_X, y, hpColor);
    y += LINE_HEIGHT;
    
    // Attack stat
    display->drawText(("ATK: " + String(enemy->getAttack())).c_str(), 
                     ENEMY_INFO_X, y, TFT_WHITE);
    y += LINE_HEIGHT;
    
    // Defense stat (show total defense including temporary)
    String defText = "DEF: " + String(enemy->getTotalDefense());
    uint16_t defColor = enemy->getIsDefending() ? TFT_BLUE : TFT_WHITE;
    display->drawText(defText.c_str(), ENEMY_INFO_X, y, defColor);
}

void CombatHUD::drawTurnInfo(int turnCounter) {
    // Turn counter in yellow
    display->drawText(("Turn: " + String(turnCounter)).c_str(), 
                     PLAYER_INFO_X, 100, TFT_YELLOW);
}

void CombatHUD::drawInventoryInfo(Player* player) {
    // Show potions in cyan
    String potionText = "Potions: " + String(player->getHealthPotions());
    uint16_t potionColor = (player->getHealthPotions() == 0) ? TFT_RED : TFT_CYAN;
    display->drawText(potionText.c_str(), PLAYER_INFO_X, 120, potionColor);
    
    // Show gold (for future shop integration)
    display->drawText(("Gold: " + String(player->getGold())).c_str(), 
                     PLAYER_INFO_X, 135, TFT_YELLOW);
}

void CombatHUD::drawVictoryScreen() {
    clearCombatArea();
    
    // Large victory text
    display->drawText("VICTORY!", 40, 60, TFT_GREEN, 2);
    
    // Show that progress continues
    display->drawText("Enemy defeated!", 20, 100, TFT_WHITE);
    display->drawText("Progress saved", 25, 115, TFT_CYAN);
    
    // Instructions
    display->drawText("Press any button", 10, 140, TFT_YELLOW);
    display->drawText("to continue", 35, 155, TFT_YELLOW);
    
    // Optional: Add victory effects or stats here later
    display->drawText("Onward!", 60, 180, TFT_GREEN);
}

void CombatHUD::drawDefeatScreen() {
    clearCombatArea();
    
    // Large defeat text
    display->drawText("DEFEAT!", 50, 60, TFT_RED, 2);
    
    // Explain what happens
    display->drawText("You have fallen in", 10, 100, TFT_WHITE);
    display->drawText("the dungeon...", 20, 115, TFT_WHITE);
    
    display->drawText("All progress lost!", 15, 140, TFT_YELLOW);
    display->drawText("Equipment lost!", 20, 155, TFT_YELLOW);
    
    // Instructions
    display->drawText("Press any button", 10, 180, TFT_CYAN);
    display->drawText("to return to town", 10, 195, TFT_CYAN);
}

void CombatHUD::drawNewCombatPrompt() {
    clearCombatArea();
    
    display->drawText("Ready for", 30, 80, TFT_WHITE, 2);
    display->drawText("Adventure?", 25, 100, TFT_WHITE, 2);
    
    display->drawText("Press any button", 10, 140, TFT_YELLOW);
    display->drawText("to start combat", 10, 155, TFT_YELLOW);
}