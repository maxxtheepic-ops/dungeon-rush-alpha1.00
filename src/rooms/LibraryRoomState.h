#ifndef LIBRARY_ROOM_STATE_H
#define LIBRARY_ROOM_STATE_H

#include "../game/GameState.h"

// Forward declarations
class Player;
class Enemy;
class DungeonManager;
class Spell;

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
    
    // Drawing methods
    void drawMainMenu();
    void drawScrollSelection();
    void drawSpellManagement();
    void drawSpellReplacement();
    void drawRestResult(bool success, String message);
    void drawSpellDetails(Spell* spell, int x, int y);
    void drawEquippedSpells();
    void drawSpellSynergies();
    
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
    void learnSelectedSpell();
    void equipSpellToSlot();
    void showSpellLearned(Spell* spell);
    void returnToMainMenu();
    void completeRoom();
    
    // Scroll management
    void addAvailableScroll(Spell* spell);
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
    void giveScrollReward(int spellID);
    void giveBossScrollReward();
    void giveRandomScroll();
};

#endif // LIBRARY_ROOM_STATE_H