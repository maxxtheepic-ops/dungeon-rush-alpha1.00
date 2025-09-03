// src/rooms/LibraryRoomState.h - Updated header with partial update methods
#ifndef LIBRARY_ROOM_STATE_H
#define LIBRARY_ROOM_STATE_H

#include "../game/GameState.h"
#include <vector>

// Forward declarations
class Player;
class Enemy;
class DungeonManager;
class Spell;
class GameStateManager;

enum class LibraryAction {
    REST = 0,           // Restore health and mana
    READ_SCROLLS = 1,   // Learn new spells
    MANAGE_SPELLS = 2,  // Equip/unequip spells
    LEAVE = 3
};

enum LibraryScreen {
    SCREEN_MAIN_MENU,
    SCREEN_SCROLL_SELECTION,
    SCREEN_SPELL_MANAGEMENT,
    SCREEN_SPELL_REPLACEMENT,
    SCREEN_REST_RESULT
};

class LibraryRoomState : public GameState {
private:
    // Game entities
    Player* player;
    Enemy* currentEnemy;
    DungeonManager* dungeonManager;
    GameStateManager* gameStateManager;
    
    // Available scrolls (found in chests/boss drops)
    std::vector<Spell*> availableScrolls;
    
    // UI state
    LibraryScreen currentScreen;
    int selectedOption;
    int maxOptions;
    bool screenDrawn;
    int lastSelectedOption;
    
    // Spell management state
    int selectedSpellSlot;      // Which slot to replace (0-3)
    int selectedScrollIndex;    // Which scroll to learn
    int selectedKnownSpell;     // Which known spell to manage
    
    // Rest costs
    static const int REST_COST = 20;
    
    // Full screen drawing methods
    void drawMainMenu();
    void drawScrollSelection();
    void drawSpellManagement();
    void drawSpellReplacement();
    void drawRestResult(bool success, String message);
    void showSpellLearned(Spell* spell);
    
    // NEW: Partial update drawing methods for main menu
    void drawMainMenuOptions();
    void drawMainMenuCursor(int option);
    void clearMainMenuCursor(int option);
    void updateMainMenuSelection();
    void drawEquippedSpellsFooter();
    
    // NEW: Partial update methods for scroll selection
    void drawScrollOptions();
    void drawScrollCursor(int scrollIndex);
    void clearScrollCursor(int scrollIndex);
    void updateScrollSelection();
    
    // NEW: Partial update methods for spell management
    void drawSpellSlots();
    void drawSpellSlotCursor(int slot);
    void clearSpellSlotCursor(int slot);
    void updateSpellManagementSelection();
    void drawKnownSpellsSummary();
    
    // NEW: Partial update methods for spell replacement
    void drawCurrentSpellInSlot();
    void drawAvailableSpells();
    void drawSpellReplacementCursor(int spellIndex);
    void clearSpellReplacementCursor(int spellIndex);
    void updateSpellReplacementSelection();
    
    // Input handling
    void handleMainMenuInput();
    void handleScrollSelectionInput();
    void handleSpellManagementInput();
    void handleSpellReplacementInput();
    void handleRestResultInput();
    
    // Actions
    void performRest();
    void openScrolls();
    void openSpellManagement();
    void readSelectedScroll();
    void equipSpellToSlot();
    void returnToMainMenu();
    void completeRoom();
    
    // Scroll management
    void removeScroll(int index);
    bool hasScrolls() const;
    
public:
    LibraryRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm);
    ~LibraryRoomState();
    
    // GameState interface
    void enter() override;
    void update() override;
    void exit() override;
    
    // Scroll management (called by other systems)
    void addAvailableScroll(Spell* spell);
    void giveScrollReward(int spellID);
    void giveBossScrollReward();
    void giveRandomScroll();
    
    // GameStateManager access
    void setGameStateManager(GameStateManager* gsm) { gameStateManager = gsm; }
};

#endif // LIBRARY_ROOM_STATE_H