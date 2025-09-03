// src/combat/CombatHUD.cpp - Updated to remove unused systems
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
    // Removed drawInventoryInfo() - no longer needed
}

void CombatHUD::updateCombatStats(Player* player, Enemy* enemy, int turnCounter) {
    // Only update the stats area, not the whole screen
    display->fillRect(0, INFO_START_Y, Display::WIDTH, 120, TFT_BLACK);
    drawPlayerInfo(player);
    drawEnemyInfo(enemy);
    drawTurnInfo(turnCounter);
    // Removed drawInventoryInfo() - no longer needed
}

void CombatHUD::clearSpriteArea() {
    display->fillRect(0, 0, Display::WIDTH, SPRITE_AREA_HEIGHT, TFT_BLACK);
}

void CombatHUD::clearCombatArea() {
    display->fillRect(0, 0, Display::WIDTH, 200, TFT_BLACK);  // Adjusted for spell menu
}

void CombatHUD::drawPlayerInfo(Player* player) {
    int y = INFO_START_Y;
    
    // Player name in purple (wizard theme)
    display->drawText("WIZARD", PLAYER_INFO_X, y, TFT_PURPLE);
    y += LINE_HEIGHT;
    
    // Health with color coding
    String hpText = "HP: " + String(player->getCurrentHP()) + "/" + String(player->getMaxHP());
    uint16_t hpColor = (player->getCurrentHP() < player->getMaxHP() / 3) ? TFT_RED : TFT_WHITE;
    display->drawText(hpText.c_str(), PLAYER_INFO_X, y, hpColor);
    y += LINE_HEIGHT;
    
    // Mana (for wizard)
    String manaText = "MP: " + String(player->getCurrentMana()) + "/" + String(player->getMaxMana());
    uint16_t manaColor = (player->getCurrentMana() < player->getMaxMana() / 4) ? TFT_RED : TFT_BLUE;
    display->drawText(manaText.c_str(), PLAYER_INFO_X, y, manaColor);
    y += LINE_HEIGHT;
    
    // Show magical defense (show total defense including shields)
    String defText = "DEF: " + String(player->getTotalDefense());
    uint16_t defColor = player->getIsDefending() ? TFT_BLUE : TFT_WHITE;
    if (player->getShieldValue() > 0) {
        defColor = TFT_CYAN; // Show shields in cyan
        defText += " (+" + String(player->getShieldValue()) + " shield)";
    }
    display->drawText(defText.c_str(), PLAYER_INFO_X, y, defColor);
    y += LINE_HEIGHT;
    
    // Show active spell effects count
    auto activeEffects = player->getActiveEffects();
    if (!activeEffects.empty()) {
        display->drawText(("Effects: " + String(activeEffects.size())).c_str(), 
                         PLAYER_INFO_X, y, TFT_PURPLE);
    }
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
    y += LINE_HEIGHT;
    
    // Show AI type
    String aiText = "AI: ";
    switch(enemy->getAIType()) {
        case AI_AGGRESSIVE: aiText += "Aggressive"; break;
        case AI_DEFENSIVE: aiText += "Defensive"; break;
        case AI_BERSERKER: aiText += "Berserker"; break;
        case AI_BALANCED: 
        default: aiText += "Balanced"; break;
    }
    display->drawText(aiText.c_str(), ENEMY_INFO_X, y, TFT_GRAY);
}

void CombatHUD::drawTurnInfo(int turnCounter) {
    // Turn counter in yellow
    display->drawText(("Turn: " + String(turnCounter)).c_str(), 
                     PLAYER_INFO_X, 100, TFT_YELLOW);
}

// REMOVED: drawInventoryInfo() - no longer needed since we removed potions and gold

void CombatHUD::drawVictoryScreen() {
    clearCombatArea();
    
    // Large victory text
    display->drawText("VICTORY!", 40, 60, TFT_GREEN, 2);
    
    // Show magical triumph
    display->drawText("Enemy vanquished", 20, 100, TFT_WHITE);
    display->drawText("by your magic!", 25, 115, TFT_PURPLE);
    
    // Instructions
    display->drawText("Press any button", 10, 140, TFT_YELLOW);
    display->drawText("to continue", 35, 155, TFT_YELLOW);
    
    // Magical flourish
    display->drawText("* Arcane power *", 15, 180, TFT_CYAN);
    display->drawText("* grows stronger *", 10, 195, TFT_CYAN);
}

void CombatHUD::drawDefeatScreen() {
    clearCombatArea();
    
    // Large defeat text
    display->drawText("DEFEAT!", 50, 60, TFT_RED, 2);
    
    // Explain what happens
    display->drawText("Your magic failed", 20, 100, TFT_WHITE);
    display->drawText("in the dungeon...", 20, 115, TFT_WHITE);
    
    display->drawText("All progress lost!", 15, 140, TFT_YELLOW);
    display->drawText("Spells forgotten!", 20, 155, TFT_PURPLE);
    
    // Instructions
    display->drawText("Press any button", 10, 180, TFT_CYAN);
    display->drawText("to return to town", 10, 195, TFT_CYAN);
}

void CombatHUD::drawNewCombatPrompt() {
    clearCombatArea();
    
    display->drawText("Ready your", 30, 80, TFT_WHITE, 2);
    display->drawText("Spells?", 45, 100, TFT_PURPLE, 2);
    
    display->drawText("Press any button", 10, 140, TFT_YELLOW);
    display->drawText("to start combat", 10, 155, TFT_YELLOW);
}