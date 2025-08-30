#include "Floor.h"
#include "../utils/constants.h"
#include <Arduino.h>

Floor::Floor(int floorNum) {
    floorNumber = floorNum;
    currentRoom = nullptr;
    roomsCompleted = 0;
    rooms.clear();
    currentChoices.clear(); // Initialize the stored choices
}

void Floor::generateFloor() {
    Serial.println("Generating Floor " + String(floorNumber) + "...");
    
    // Clear existing rooms
    for (Room* room : rooms) {
        delete room;
    }
    rooms.clear();
    currentChoices.clear();
    
    // Create boss room (stored permanently)
    Room* bossRoom = new Room(999, ROOM_BOSS);
    bossRoom->setEnemyType(3); // Orc boss
    rooms.push_back(bossRoom);
    
    Serial.println("Floor " + String(floorNumber) + " initialized with boss room.");
}

RoomType Floor::selectRandomRoomType() {
    int roll = random(1, 101); // 1-100
    
    if (roll <= 60) {
        return ROOM_ENEMY;
    } else if (roll <= 80) {
        return ROOM_TREASURE;
    } else {
        return ROOM_SHOP;
    }
}

bool Floor::isFloorComplete() const {
    return roomsCompleted >= ROOMS_PER_FLOOR; // Use constant from constants.h
}

Room* Floor::getCurrentRoom() const {
    return currentRoom;
}

std::vector<DoorChoice> Floor::getAvailableChoices() {
    // If we already have choices generated for this turn, return them
    if (!currentChoices.empty()) {
        Serial.println("DEBUG: Returning cached choices (" + String(currentChoices.size()) + " choices)");
        return currentChoices;
    }
    
    Serial.println("Floor " + String(floorNumber) + ": Generating new choices, rooms completed: " + String(roomsCompleted));
    
    if (isFloorComplete()) {
        Serial.println("Floor complete - offering boss room");
        // Only boss room available
        DoorChoice bossChoice;
        bossChoice.room = getBossRoom();
        bossChoice.icon = ICON_SKULL;
        bossChoice.description = "Final challenge awaits";
        currentChoices.push_back(bossChoice);
    } else {
        Serial.println("Floor incomplete - generating 2 random rooms");
        
        // Clear any existing temporary rooms (keep only boss room)
        while (rooms.size() > 1) {
            delete rooms.back();
            rooms.pop_back();
        }
        
        // Generate two fresh rooms for this choice - STORE THEM PERMANENTLY
        for (int i = 0; i < 2; i++) {
            RoomType roomType = selectRandomRoomType();
            Room* newRoom = new Room(roomsCompleted * 10 + i, roomType);
            
            Serial.println("DEBUG: Generated room " + String(i) + " - Type: " + String(roomType) + " (" + 
                          (roomType == ROOM_ENEMY ? "ENEMY" : 
                           roomType == ROOM_TREASURE ? "TREASURE" : 
                           roomType == ROOM_SHOP ? "SHOP" : "UNKNOWN") + ")");
            
            // Setup room content based on type
            switch(roomType) {
                case ROOM_ENEMY:
                    {
                        int enemyType = random(1, 4);
                        newRoom->setEnemyType(enemyType);
                        Serial.println("  -> Enemy type: " + String(enemyType));
                    }
                    break;
                    
                case ROOM_TREASURE:
                    {
                        int treasureType = random(1, 4);
                        int treasureValue = floorNumber + random(1, 4);
                        newRoom->setTreasure(treasureType, treasureValue);
                        Serial.println("  -> Treasure type: " + String(treasureType) + ", value: " + String(treasureValue));
                    }
                    break;
                    
                case ROOM_SHOP:
                    // Shop setup handled in room
                    Serial.println("  -> Shop room ready");
                    break;
                    
                default:
                    Serial.println("  -> Unknown room type!");
                    break;
            }
            
            // Add to rooms list (permanently stored)
            rooms.push_back(newRoom);
            
            // Create door choice
            DoorChoice choice;
            choice.room = newRoom;
            choice.icon = newRoom->getDoorIcon();
            choice.description = newRoom->getDescription();
            currentChoices.push_back(choice);
            
            Serial.println("  -> Added door choice " + String(i) + ": " + choice.description);
        }
    }
    
    Serial.println("DEBUG: Generated " + String(currentChoices.size()) + " door choices");
    return currentChoices;
}

bool Floor::enterRoom(int choice) {
    Serial.println("DEBUG: enterRoom called with choice " + String(choice));
    
    // Make sure we have current choices
    if (currentChoices.empty()) {
        Serial.println("DEBUG: No current choices, generating them...");
        getAvailableChoices();
    }
    
    if (choice < 0 || choice >= currentChoices.size()) {
        Serial.println("ERROR: Invalid choice index: " + String(choice) + ", available: " + String(currentChoices.size()));
        return false;
    }
    
    currentRoom = currentChoices[choice].room;
    
    if (currentRoom) {
        Serial.println("DEBUG: Successfully entered room - Type: " + String(currentRoom->getType()) + " (" + 
                      (currentRoom->getType() == ROOM_ENEMY ? "ENEMY" : 
                       currentRoom->getType() == ROOM_TREASURE ? "TREASURE" : 
                       currentRoom->getType() == ROOM_SHOP ? "SHOP" : 
                       currentRoom->getType() == ROOM_BOSS ? "BOSS" : "UNKNOWN") + ")");
        
        // IMPORTANT: Clear the current choices so new ones will be generated next time
        currentChoices.clear();
        Serial.println("DEBUG: Cleared current choices for next turn");
        
        return true;
    } else {
        Serial.println("ERROR: Selected room is null!");
        return false;
    }
}

bool Floor::isBossRoomReady() const {
    return isFloorComplete();
}

Room* Floor::getBossRoom() {
    if (rooms.size() > 0) {
        return rooms[0]; // Boss room is always first
    }
    return nullptr;
}

int Floor::getRoomsCompleted() const {
    return roomsCompleted;
}

int Floor::getFloorNumber() const {
    return floorNumber;
}

void Floor::incrementRoomsCompleted() {
    roomsCompleted++;
    Serial.println("DEBUG: Rooms completed incremented to: " + String(roomsCompleted));
    // Clear current choices when a room is completed so new ones are generated
    currentChoices.clear();
    Serial.println("DEBUG: Cleared choices after room completion");
}

Floor::~Floor() {
    for (Room* room : rooms) {
        delete room;
    }
    rooms.clear();
    currentChoices.clear();
}