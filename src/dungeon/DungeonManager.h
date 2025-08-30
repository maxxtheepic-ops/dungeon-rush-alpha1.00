#ifndef DUNGEON_MANAGER_H
#define DUNGEON_MANAGER_H

#include "Floor.h"
#include "../entities/player.h"
#include <vector>

class DungeonManager {
private:
    Player* player;
    Floor* currentFloor;
    int currentFloorNumber;
    int totalRoomsCompleted;
    
public:
    // Constructor
    DungeonManager(Player* p);
    
    // Floor management
    void startNewFloor();
    void advanceToNextFloor();
    void resetToFirstFloor();  // Add explicit reset method
    
    // Room management
    std::vector<DoorChoice> getAvailableRooms();
    Room* selectRoom(int choice);  // 0 = left, 1 = right
    void markRoomCompleted();
    
    // Getters
    Floor* getCurrentFloor() const;
    int getCurrentFloorNumber() const;
    int getRoomsCompletedThisFloor() const;
    int getTotalRoomsCompleted() const;
    bool isBossRoomAvailable() const;
    
    // Cleanup
    ~DungeonManager();
};

#endif