// src/combat/CombatHUD.cpp - Updated to clear all areas on victory/defeat screens
#include "CombatHUD.h"
#include "../graphics/VictoryScreen.h"
#include "../utils/constants.h"

CombatHUD::CombatHUD(Display* disp) {
    display = disp;
}

void CombatHUD::drawFullCombatScreen(Player* player, Enemy* enemy, int turnCounter) {
    // Clear the entire screen first to remove ALL door choice remnants
    clearEntireScreen();
    
    // Draw combat info
    drawPlayerInfo(player);
    drawEnemyInfo(enemy);
    drawTurnInfo(turnCounter);
    
    // Note: Text box and spell menu will be drawn by their respective systems
}

void CombatHUD::updateCombatStats(Player* player, Enemy* enemy, int turnCounter) {
    // Only update the HUD info area, not the whole screen
    clearHUDInfoArea();
    drawPlayerInfo(player);
    drawEnemyInfo(enemy);
    drawTurnInfo(turnCounter);
    
    // Note: This preserves both text box and spell menu areas
}

void CombatHUD::clearSpriteAndHUDArea() {
    // Clear the top area including where floor progress text might be
    // This needs to clear up to y=210 to remove any floor progress text from DoorChoice
    display->fillRect(0, 0, Display::WIDTH, 210, TFT_BLACK);
}

void CombatHUD::clearHUDInfoArea() {
    // Clear only the HUD info section, preserving sprites above and text/menu below
    display->fillRect(0, INFO_START_Y, Display::WIDTH, 120, TFT_BLACK);
}

void CombatHUD::clearCombatArea() {
    // Legacy method - clear everything except spell menu
    display->fillRect(0, 0, Display::WIDTH, 260, TFT_BLACK);
}

void CombatHUD::clearEntireScreen() {
    // NEW: Clear the entire screen including text box and spell menu areas
    display->fillRect(0, 0, Display::WIDTH, Display::HEIGHT, TFT_BLACK);
}

void CombatHUD::drawPlayerInfo(Player* player) {
    int y = INFO_START_Y;
    
    // Player name in purple (wizard theme)
    display->drawText("WIZARD", PLAYER_INFO_X, y, TFT_WHITE);
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
        defColor = TFT_WHITE; // Show shields in cyan
        defText += " (+" + String(player->getShieldValue()) + " shield)";
    }
    display->drawText(defText.c_str(), PLAYER_INFO_X, y, defColor);
    y += LINE_HEIGHT;
    
    // Show active spell effects count
    auto activeEffects = player->getActiveEffects();
    if (!activeEffects.empty()) {
        display->drawText(("Effects: " + String(activeEffects.size())).c_str(), 
                         PLAYER_INFO_X, y, TFT_WHITE);
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
    display->drawText(aiText.c_str(), ENEMY_INFO_X, y, 0x8410); // Gray color for AI type
}

void CombatHUD::drawTurnInfo(int turnCounter) {
    // Turn counter in yellow (positioned to not overlap with other info)
    display->drawText(("Turn: " + String(turnCounter)).c_str(), 
                     PLAYER_INFO_X, 180, TFT_WHITE);
}

void CombatHUD::drawVictoryScreen() {
    // Clear the entire screen including text box and spell menu
    clearEntireScreen();
    
    // Draw the victory image
    drawVictoryImage();
    
    // Draw "Press any button to continue" at the bottom, below the image
    display->drawText("Press any button", 25, 280, TFT_WHITE, 1);
    display->drawText("to continue", 45, 295, TFT_WHITE, 1);
}


void CombatHUD::drawDefeatScreen() {
    // UPDATED: Clear the entire screen including text box and spell menu
    clearEntireScreen();
    
    // Large defeat text
    display->drawText("DEFEAT!", 50, 60, TFT_RED, 2);
    
    // Explain what happens
    display->drawText("Your magic failed", 20, 100, TFT_WHITE);
    display->drawText("in the dungeon...", 20, 115, TFT_WHITE);
    
    display->drawText("All progress lost!", 15, 140, TFT_WHITE);
    display->drawText("Spells forgotten!", 20, 155, TFT_WHITE);
    
    // Instructions
    display->drawText("Press any button", 10, 175, TFT_WHITE);
}

void CombatHUD::drawVictoryImage() {
    // Display the victory image from victory_screen.h
    int imageX = 0;  // Start at left edge (full width)
    int imageY = 10; // Small margin from top
    
    // Method 1: Draw pixel by pixel (if you have RGB565 data)
    for (int y = 0; y < VICTORY_IMAGE_HEIGHT && y + imageY < 260; y++) {
        for (int x = 0; x < VICTORY_IMAGE_WIDTH && x < Display::WIDTH; x++) {
            // Read pixel from PROGMEM 
            uint16_t pixelColor = pgm_read_word(&victoryImageData[y * VICTORY_IMAGE_WIDTH + x]);
            
            // Fix blue tinting issue - check if your image data is in correct RGB565 format
            // If the image appears blue, your data might be BGR instead of RGB
            // You can swap the bytes here if needed:
            // pixelColor = ((pixelColor & 0x00FF) << 8) | ((pixelColor & 0xFF00) >> 8);
            
            display->drawPixel(imageX + x, imageY + y, pixelColor);
        }
    }
}

void CombatHUD::drawNewCombatPrompt() {
    clearSpriteAndHUDArea();
    
    display->drawText("Ready your", 30, 80, TFT_WHITE, 2);
    display->drawText("Spells?", 45, 100, TFT_WHITE, 2);
    
    display->drawText("Press any button", 10, 140, TFT_WHITE);
    display->drawText("to start combat", 10, 155, TFT_WHITE);
}