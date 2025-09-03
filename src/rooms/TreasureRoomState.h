// src/rooms/TreasureRoomState.h
#ifndef TREASURE_ROOM_STATE_H
#define TREASURE_ROOM_STATE_H

#include "../game/GameState.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../dungeon/DungeonManager.h"

// Forward declarations
class Spell;
class GameStateManager;

enum class TreasureAction {
    TAKE_TREASURE = 0,
    LEAVE = 1
};

class TreasureRoomState : public GameState {
private:
    // Game entities (same as RoomState pattern)
    Player* player;
    Enemy* currentEnemy;
    DungeonManager* dungeonManager;
    GameStateManager* gameStateManager;
    
    // Treasure room state
    int selectedOption;
    int maxOptions;
    bool screenDrawn;
    int lastSelectedOption;
    bool treasureLooted;
    
    // Drawing methods - UPDATED: Only show scroll result
    void drawTreasureScreen();
    void showTreasureResult(Spell* foundScroll);  // CHANGED: Only scroll parameter
    
    // Input handling
    void handleTreasureInput();
    
    // Treasure actions
    void takeTreasure();
    void completeRoom();
    
    // Scroll generation and management
    Spell* generateRandomScroll();
    void giveScrollToLibrary(Spell* scroll);
    
public:
    TreasureRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm);
    ~TreasureRoomState() = default;
    
    // GameState interface implementation
    void enter() override;
    void update() override;
    void exit() override;
    
    // NEW: Set GameStateManager reference
    void setGameStateManager(GameStateManager* gsm) { gameStateManager = gsm; }
};

#endif