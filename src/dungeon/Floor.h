#ifndef FLOOR_H
#define FLOOR_H

#include "Room.h"
#include "../entities/player.h"
#include <vector>

struct DoorChoice {
    Room* room;
    DoorIcon icon;
    String description;
};

class Floor {
private:
    int floorNumber;
    std::vector<Room*> rooms;
    Room* currentRoom;
    int roomsCompleted;
    
    // Cache the current door choices to prevent regeneration issues
    std::vector<DoorChoice> currentChoices;
    
    // Room generation
    void generateRandomRoom(int roomID);
    RoomType selectRandomRoomType();
    RoomType selectRoomTypeForPosition(int roomPosition); // NEW: Position-based room selection
    
    // Enemy selection with floor-based scaling
    int selectFloorScaledEnemy();
    
public:
    // Constructor
    Floor(int floorNum);
    
    // Floor management
    void generateFloor();
    bool isFloorComplete() const;
    int getRoomsCompleted() const;
    void incrementRoomsCompleted();
    int getFloorNumber() const;
    
    // Room navigation
    Room* getCurrentRoom() const;
    std::vector<DoorChoice> getAvailableChoices();
    bool enterRoom(int choice);
    
    // Boss room access
    bool isBossRoomReady() const;
    Room* getBossRoom();
    
    // Cleanup
    ~Floor();
};

#endif