// src/game/GameStateManager.h
#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include "../input/Input.h"
#include "../graphics/Display.h"
#include "GameState.h"
#include "MainMenuState.h"
#include "DoorChoiceState.h"
#include "../rooms/CombatRoomState.h"
#include "../rooms/LibraryRoomState.h"      // CHANGED: Library replaces CampfireRoomState
#include "../rooms/ShopRoomState.h"
#include "../rooms/TreasureRoomState.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../dungeon/DungeonManager.h"
#include "../spells/spell.h"  // ADDED: Need this for SpellFactory
#include <vector>

// Forward declarations
class Spell;

class GameStateManager {
private:
    // Core systems
    Display* display;
    Input* input;
    
    // Game entities (shared between states)
    Player* player;
    Enemy* currentEnemy;
    DungeonManager* dungeonManager;
    
    // Global scroll inventory system
    std::vector<Spell*> availableScrolls;  // NEW: Scrolls found but not yet learned
    
    // State management
    GameState* currentState;
    MainMenuState* mainMenuState;
    DoorChoiceState* doorChoiceState;
    CombatRoomState* combatRoomState;
    LibraryRoomState* libraryRoomState;    // CHANGED: Library replaces campfire
    ShopRoomState* shopRoomState;
    TreasureRoomState* treasureRoomState;
    
    // Game over handling
    bool handlingGameOver;  // tracks if we're in game over mode
    
    // State transition
    void changeState(StateTransition newState);
    void handlePlaceholderState(StateTransition state);
    
    // Game over screen handling
    void showGameOverScreen();      // show the game over screen
    void handleGameOverScreen();    // handle input on game over screen
    
    // Scroll inventory management
    void clearAllScrolls();  // Called during reset
    
public:
    GameStateManager(Display* disp, Input* inp);
    ~GameStateManager();
    
    // Core game loop
    void initialize();
    void update();
    
    // Utility
    void resetPlayer();
    void resetDungeonProgress();
    void fullGameReset();  // Reset everything for new game
    
    // Dungeon access (for states that need it)
    DungeonManager* getDungeonManager() const { return dungeonManager; }
    
    // NEW: Global scroll inventory system
    void addScroll(Spell* scroll);              // Add scroll to global inventory
    std::vector<Spell*> getAvailableScrolls();  // Get all available scrolls
    void removeScroll(int index);               // Remove scroll by index
    void transferScrollsToLibrary();            // Transfer scrolls to library state
    bool hasScrolls() const { return !availableScrolls.empty(); }  // ADDED: Check if any scrolls available
};

#endif