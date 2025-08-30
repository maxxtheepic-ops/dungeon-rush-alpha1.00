#ifndef ROOM_H
#define ROOM_H

#include "../entities/player.h"
#include "../entities/enemy.h"
#include <Arduino.h>

enum RoomType {
    ROOM_ENEMY,
    ROOM_TREASURE,
    ROOM_SHOP,
    ROOM_BOSS
};

enum DoorIcon {
    ICON_SWORD,        // Enemy room
    ICON_QUESTION,     // Treasure or Shop
    ICON_SKULL         // Boss room
};

class Room {
private:
    int roomID;
    RoomType type;
    bool completed;
    
    // Enemy rooms
    int enemyTypeID;
    
    // Treasure rooms
    int treasureType;
    int treasureValue;
    
    // Shop rooms
    bool shopVisited;
    
public:
    // Constructor
    Room(int id, RoomType roomType);
    
    // Room properties
    RoomType getType() const;
    int getRoomID() const;
    bool isCompleted() const;
    void setCompleted(bool complete);
    
    // Room content
    Enemy createEnemy() const;
    void giveTreasure(Player* player);
    void openShop(Player* player);
    
    // Display
    DoorIcon getDoorIcon() const;
    String getDescription() const;
    String getRoomName() const;
    
    // Room setup
    void setEnemyType(int enemyID);
    void setTreasure(int type, int value);
};

#endif