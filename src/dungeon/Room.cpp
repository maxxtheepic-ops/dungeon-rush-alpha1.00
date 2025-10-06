#include "Room.h"
#include "../utils/constants.h"

// Constructor
Room::Room(int id, RoomType roomType) {
    roomID = id;
    type = roomType;
    completed = false;
    enemyTypeID = 1; // Default to Goblin
    treasureType = 0;
    treasureValue = 0;
    shopVisited = false;
}

// Room properties
RoomType Room::getType() const {
    return type;
}

int Room::getRoomID() const {
    return roomID;
}

bool Room::isCompleted() const {
    return completed;
}

void Room::setCompleted(bool complete) {
    completed = complete;
}

// Create enemy based on room's enemy type
Enemy Room::createEnemy() const {
    switch(enemyTypeID) {
        case 1:
            return Enemy::createGoblin();
        case 2:
            return Enemy::createSkeleton();
        case 3:
            return Enemy::createOrc();
        case 4: 
            return Enemy::createTroll();
        case 5: 
            return Enemy::createDragon();
        case 6: 
            return Enemy::createBandit();
        default:
            return Enemy::createGoblin();
    }
}

// Give treasure to player
void Room::giveTreasure(Player* player) {
    if (completed) return; // Already looted
    
    switch(treasureType) {
        case 1: // Health Potions
            player->addHealthPotions(treasureValue);
            Serial.println("Found " + String(treasureValue) + " health potions!");
            break;
        case 2: // Equipment Bonus
            player->addEquipmentBonus(treasureValue, treasureValue, treasureValue, treasureValue);
            Serial.println("Found magical equipment! (+" + String(treasureValue) + " to all stats)");
            break;
        case 3: // Large Equipment Bonus
            player->addEquipmentBonus(treasureValue * 2, treasureValue, treasureValue, 0);
            Serial.println("Found powerful armor! (+" + String(treasureValue * 2) + " HP, +" + String(treasureValue) + " ATK/DEF)");
            break;
        default:
            player->addHealthPotions(2);
            Serial.println("Found 2 health potions!");
            break;
    }
    
    setCompleted(true);
}

// Open shop for player (now represents library access)
void Room::openShop(Player* player) {
    if (shopVisited) {
        Serial.println("The library has already been visited!");
        return;
    }
    
    // Library visit - this is handled by LibraryRoomState now
    Serial.println("Library: Entering the mystical library...");
    
    shopVisited = true;
    setCompleted(true);
}

// Get door icon for this room type
DoorIcon Room::getDoorIcon() const {
    switch(type) {
        case ROOM_ENEMY:
            return ICON_SWORD;
        case ROOM_TREASURE:
            return ICON_QUESTION;
        case ROOM_SHOP:  // Shop rooms are now library rooms
            return ICON_QUESTION;  // Use same icon as treasure
        case ROOM_BOSS:
            return ICON_SKULL;
        default:
            return ICON_SWORD;
    }
}

// Get room description - UPDATED for library theme
String Room::getDescription() const {
    switch(type) {
        case ROOM_ENEMY:
            return "Growling  echoes    within";
        case ROOM_TREASURE:
            return "Something glints    inside";
        case ROOM_SHOP:  // Shop is now library
            return "Ancient   knowledge awaits";  // CHANGED from "You smell incense"
        case ROOM_BOSS:
            return "Ominous presence lurks";
        default:
            return "Mysterious room";
    }
}

// Get room name - UPDATED for library theme
String Room::getRoomName() const {
    switch(type) {
        case ROOM_ENEMY:
            return "Combat Room";
        case ROOM_TREASURE:
            return "Treasure Room";
        case ROOM_SHOP:  // Shop is now library
            return "Arcane Library";  // CHANGED from "Shop"
        case ROOM_BOSS:
            return "Boss Chamber";
        default:
            return "Unknown Room";
    }
}

// Setup methods
void Room::setEnemyType(int enemyID) {
    enemyTypeID = enemyID;
}

void Room::setTreasure(int type, int value) {
    treasureType = type;
    treasureValue = value;
}