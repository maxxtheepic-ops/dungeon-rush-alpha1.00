#include "Floor.h"
#include "../utils/constants.h"
#include <Arduino.h>

// Enemy spawn data structure and table
struct EnemySpawnData {
    int enemyID;
    int minFloor;        // First floor this enemy can appear
    int baseWeight;      // Base spawn weight
    int weightPerFloor;  // Additional weight gained per floor
    int maxWeight;       // Maximum weight cap
    const char* name;    // For debugging
};

const EnemySpawnData ENEMY_SPAWN_TABLE[] = {
    // ID, MinFloor, BaseWeight, WeightPerFloor, MaxWeight, Name
    {1,  1,  50,  -3,  50,  "Goblin"},      // Common early, becomes rarer
    {2,  1,  20,   5,  40,  "Skeleton"},    // Grows more common over time
    {3,  2,   0,   8,  35,  "Orc Warrior"}, // Rare early, becomes common later
    // Future enemies ready for expansion:
    // {4,  3,   0,   5,  25,  "Troll"},        // Floors 3+
    // {5,  4,   0,   3,  20,  "Dragon"},      // Floors 4+, very rare
    // {6,  1,  15,   2,  30,  "Bandit"},      // Available early, steady growth
};

const int ENEMY_SPAWN_TABLE_SIZE = sizeof(ENEMY_SPAWN_TABLE) / sizeof(EnemySpawnData);

// Constructor
Floor::Floor(int floorNum) {
    floorNumber = floorNum;
    currentRoom = nullptr;
    roomsCompleted = 0;
    rooms.clear();
    currentChoices.clear(); // Initialize the stored choices
}

// Generate floor with boss room
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

// Legacy room type selection (kept for compatibility)
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

// Position-based room type selection (new system)
RoomType Floor::selectRoomTypeForPosition(int roomPosition) {
    // Convert 0-based position to 1-based room number for clarity
    int roomNumber = roomPosition + 1;
    Serial.println("DEBUG: Selecting room type for room " + String(roomNumber) + " (position " + String(roomPosition) + ") on floor " + String(floorNumber));
    
    // Room 10 and above should not be generated through this method during regular room generation
    // This method should only handle rooms 1-10 for regular floor progression
    if (roomNumber > ROOMS_PER_FLOOR) {
        Serial.println("ERROR: selectRoomTypeForPosition called for room " + String(roomNumber) + " which exceeds ROOMS_PER_FLOOR (" + String(ROOMS_PER_FLOOR) + ")");
        return ROOM_ENEMY; // Fallback to enemy room
    }
    
    // All rooms: 10% treasure, 90% enemy
    int roll = random(1, 101); // 1-100
    
    if (roll <= 10) {
        Serial.println("DEBUG: Room " + String(roomNumber) + " - Rolled " + String(roll) + " - TREASURE room");
        return ROOM_TREASURE;
    } else {
        Serial.println("DEBUG: Room " + String(roomNumber) + " - Rolled " + String(roll) + " - ENEMY room");
        return ROOM_ENEMY;
    }
}

// Floor completion check - boss appears when counter shows 10/10
bool Floor::isFloorComplete() const {
    return roomsCompleted >= 10; // Boss appears after 10 regular rooms
}

// Current room getter
Room* Floor::getCurrentRoom() const {
    return currentRoom;
}

// Main room generation method with door selection logic
std::vector<DoorChoice> Floor::getAvailableChoices() {
    // If we already have choices generated for this turn, return them
    if (!currentChoices.empty()) {
        Serial.println("DEBUG: Returning cached choices (" + String(currentChoices.size()) + " choices)");
        return currentChoices;
    }
    
    // Calculate which door selection this is (1-based)
    int doorSelectionNumber = roomsCompleted + 1;
    Serial.println("Floor " + String(floorNumber) + ": Door selection #" + String(doorSelectionNumber) + ", rooms completed: " + String(roomsCompleted));
    
    // Check if we should show boss room (when counter shows 10/10)  
    if (roomsCompleted >= 10) {
        Serial.println("Floor complete - offering boss room (rooms completed: " + String(roomsCompleted) + "/10)");
        // Only boss room available
        DoorChoice bossChoice;
        bossChoice.room = getBossRoom();
        bossChoice.icon = ICON_SKULL;
        bossChoice.description = "Final challenge awaits";
        currentChoices.push_back(bossChoice);
    } else {
        Serial.println("Floor incomplete - generating rooms for positions " + String(roomsCompleted) + " and " + String(roomsCompleted + 1) + 
                      " (rooms " + String(roomsCompleted + 1) + " and " + String(roomsCompleted + 2) + ")");
        
        // Clear any existing temporary rooms (keep only boss room)
        while (rooms.size() > 1) {
            delete rooms.back();
            rooms.pop_back();
        }
        
        // Special case: Door selection 5 gets one guaranteed shop
        bool forceShopThisSelection = (doorSelectionNumber == 5);
        bool shopGenerated = false;
        
        // Always generate 2 rooms for regular progression (0-9/10)
        for (int i = 0; i < 2; i++) {
            int roomPosition = roomsCompleted + i; // Current room position on this floor  
            
            RoomType roomType;
            
            // Force shop on door selection 5 (for first room only, to guarantee it appears)
            if (forceShopThisSelection && i == 0 && !shopGenerated) {
                roomType = ROOM_SHOP;
                shopGenerated = true;
                Serial.println("DEBUG: Door selection 5 - forcing SHOP for choice " + String(i));
            } else {
                roomType = selectRoomTypeForPosition(roomPosition);
            }
            
            Room* newRoom = new Room(roomsCompleted * 10 + i, roomType);
            
            Serial.println("DEBUG: Generated room " + String(i) + " - Room " + String(roomPosition + 1) + 
                          " - Type: " + String(roomType) + " (" + 
                          (roomType == ROOM_ENEMY ? "ENEMY" : 
                           roomType == ROOM_TREASURE ? "TREASURE" : 
                           roomType == ROOM_SHOP ? "SHOP" : "UNKNOWN") + ")");
            
            // Setup room content based on type
            switch(roomType) {
                case ROOM_ENEMY:
                    {
                        int enemyType = selectFloorScaledEnemy(); // Use floor-based enemy selection
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
            Serial.println("  -> Door icon: " + String(choice.icon == ICON_SWORD ? "SWORD" : choice.icon == ICON_QUESTION ? "QUESTION" : choice.icon == ICON_SKULL ? "SKULL" : "UNKNOWN"));
            Serial.println("  -> Room type verification: " + String(newRoom->getType()));
        }
    }
    
    Serial.println("DEBUG: Generated " + String(currentChoices.size()) + " door choices");
    return currentChoices;
}

// Room entry logic
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

// Boss room availability check
bool Floor::isBossRoomReady() const {
    return isFloorComplete();
}

// Get boss room
Room* Floor::getBossRoom() {
    if (rooms.size() > 0) {
        return rooms[0]; // Boss room is always first
    }
    return nullptr;
}

// Room completion tracking
int Floor::getRoomsCompleted() const {
    return roomsCompleted;
}

// Floor number getter
int Floor::getFloorNumber() const {
    return floorNumber;
}

// Increment room completion counter
void Floor::incrementRoomsCompleted() {
    roomsCompleted++;
    Serial.println("DEBUG: Rooms completed incremented to: " + String(roomsCompleted));
    // Clear current choices when a room is completed so new ones are generated
    currentChoices.clear();
    Serial.println("DEBUG: Cleared choices after room completion");
}

// Floor-based enemy selection system
int Floor::selectFloorScaledEnemy() {
    Serial.println("DEBUG: Selecting enemy for floor " + String(floorNumber));
    
    // Build weighted list for current floor
    struct WeightedEnemy {
        int enemyID;
        int weight;
        const char* name;
    };
    
    WeightedEnemy availableEnemies[10]; // Max 10 enemy types
    int availableCount = 0;
    int totalWeight = 0;
    
    // Calculate weights for each enemy type on this floor
    for (int i = 0; i < ENEMY_SPAWN_TABLE_SIZE; i++) {
        const EnemySpawnData& enemy = ENEMY_SPAWN_TABLE[i];
        
        // Skip if enemy isn't available on this floor yet
        if (floorNumber < enemy.minFloor) {
            continue;
        }
        
        // Calculate current weight for this enemy
        int floorsActive = floorNumber - enemy.minFloor + 1;
        int currentWeight = enemy.baseWeight + (enemy.weightPerFloor * (floorsActive - 1));
        
        // Apply weight cap
        if (currentWeight > enemy.maxWeight) {
            currentWeight = enemy.maxWeight;
        }
        
        // Don't spawn enemies with 0 or negative weight
        if (currentWeight <= 0) {
            continue;
        }
        
        // Add to available enemies
        availableEnemies[availableCount] = {enemy.enemyID, currentWeight, enemy.name};
        totalWeight += currentWeight;
        availableCount++;
        
        Serial.println("DEBUG: " + String(enemy.name) + " (ID:" + String(enemy.enemyID) + 
                      ") weight: " + String(currentWeight));
    }
    
    // Fallback if no enemies available (shouldn't happen)
    if (availableCount == 0 || totalWeight == 0) {
        Serial.println("WARNING: No enemies available for floor " + String(floorNumber) + ", defaulting to Goblin");
        return 1; // Default to Goblin
    }
    
    // Select enemy based on weighted random
    int roll = random(1, totalWeight + 1);
    int currentSum = 0;
    
    for (int i = 0; i < availableCount; i++) {
        currentSum += availableEnemies[i].weight;
        if (roll <= currentSum) {
            Serial.println("DEBUG: Selected " + String(availableEnemies[i].name) + 
                          " (rolled " + String(roll) + "/" + String(totalWeight) + ")");
            return availableEnemies[i].enemyID;
        }
    }
    
    // Fallback (shouldn't reach here)
    Serial.println("WARNING: Enemy selection fallback triggered");
    return availableEnemies[0].enemyID;
}

// Destructor
Floor::~Floor() {
    for (Room* room : rooms) {
        delete room;
    }
    rooms.clear();
    currentChoices.clear();
}