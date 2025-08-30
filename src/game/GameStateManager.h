#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include "../input/Input.h"
#include "../graphics/Display.h"
#include "GameState.h"
#include "MainMenuState.h"
#include "DoorChoiceState.h"
#include "../rooms/CombatRoomState.h"
#include "../rooms/CampfireRoomState.h"
#include "../rooms/ShopRoomState.h"
#include "../rooms/TreasureRoomState.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../dungeon/DungeonManager.h"

class GameStateManager {
private:
    // Core systems
    Display* display;
    Input* input;
    
    // Game entities (shared between states)
    Player* player;
    Enemy* currentEnemy;
    DungeonManager* dungeonManager;
    
    // State management
    GameState* currentState;
    MainMenuState* mainMenuState;
    DoorChoiceState* doorChoiceState;
    CombatRoomState* combatRoomState;
    CampfireRoomState* campfireRoomState;
    ShopRoomState* shopRoomState;          // NEW: Shop room state
    TreasureRoomState* treasureRoomState;  // NEW: Treasure room state
    
    // Game over handling
    bool handlingGameOver;  // tracks if we're in game over mode
    
    // State transition
    void changeState(StateTransition newState);
    void handlePlaceholderState(StateTransition state);
    
    // Game over screen handling
    void showGameOverScreen();      // show the game over screen
    void handleGameOverScreen();    // handle input on game over screen
    
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
};

#endif