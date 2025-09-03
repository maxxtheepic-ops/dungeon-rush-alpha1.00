#ifndef ROOM_STATE_H
#define ROOM_STATE_H

#include "../game/GameState.h"
#include "../dungeon/Room.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../dungeon/DungeonManager.h"

// Forward declaration
class GameStateManager;

class RoomState : public GameState {
protected:
    // Shared room data
    Room* currentRoom;
    Player* player;
    Enemy* currentEnemy;
    DungeonManager* dungeonManager;
    GameStateManager* gameStateManager;  // NEW: Access to global systems
    
    // Room state
    bool roomCompleted;
    bool roomEntered;
    
public:
    RoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm, GameStateManager* gsm = nullptr);
    virtual ~RoomState() = default;
    
    // Base GameState interface
    void enter() override;
    void update() override;
    void exit() override;
    
    // Room-specific interface (to be implemented by each room type)
    virtual void enterRoom() = 0;
    virtual void handleRoomInteraction() = 0;
    virtual void exitRoom() = 0;
    
    // Room setup
    void setCurrentRoom(Room* room);
    Room* getCurrentRoom() const { return currentRoom; }
    
    // Common room completion
    void completeRoom();
    bool isRoomCompleted() const { return roomCompleted; }
    
    // NEW: Set GameStateManager reference
    void setGameStateManager(GameStateManager* gsm) { gameStateManager = gsm; }
    
protected:
    // Helper for transitioning back to door choice
    void returnToDoorChoice();
};

#endif