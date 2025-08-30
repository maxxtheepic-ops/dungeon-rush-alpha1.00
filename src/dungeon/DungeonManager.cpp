#include "DungeonManager.h"
#include "../utils/constants.h"

DungeonManager::DungeonManager(Player* p) {
    player = p;
    currentFloor = nullptr;
    currentFloorNumber = 0;
    totalRoomsCompleted = 0;
}

void DungeonManager::startNewFloor() {
    if (currentFloor) {
        delete currentFloor;
        currentFloor = nullptr;  // Ensure clean state
    }
    
    currentFloorNumber++;
    currentFloor = new Floor(currentFloorNumber);
    currentFloor->generateFloor();
    
    Serial.println("DungeonManager: Started fresh floor " + String(currentFloorNumber));
    Serial.println("Rooms completed reset to 0");
}

std::vector<DoorChoice> DungeonManager::getAvailableRooms() {
    if (!currentFloor) {
        startNewFloor();
    }
    return currentFloor->getAvailableChoices();
}

Room* DungeonManager::selectRoom(int choice) {
    if (!currentFloor) return nullptr;
    
    if (currentFloor->enterRoom(choice)) {
        return currentFloor->getCurrentRoom();
    }
    return nullptr;
}

void DungeonManager::markRoomCompleted() {
    if (currentFloor) {
        currentFloor->incrementRoomsCompleted();
        totalRoomsCompleted++;
        
        if (currentFloor->isFloorComplete() && !currentFloor->isBossRoomReady()) {
            Serial.println("Floor " + String(currentFloorNumber) + " complete!");
        }
    }
}

void DungeonManager::advanceToNextFloor() {
    startNewFloor();
}

void DungeonManager::resetToFirstFloor() {
    // Complete reset - go back to floor 1
    if (currentFloor) {
        delete currentFloor;
        currentFloor = nullptr;
    }
    
    currentFloorNumber = 0;  // Will become 1 when startNewFloor is called
    totalRoomsCompleted = 0;
    
    startNewFloor();
    Serial.println("DungeonManager: Complete reset to Floor 1");
}

// Getters
Floor* DungeonManager::getCurrentFloor() const { 
    return currentFloor; 
}

int DungeonManager::getCurrentFloorNumber() const { 
    return currentFloorNumber; 
}

int DungeonManager::getRoomsCompletedThisFloor() const {
    return currentFloor ? currentFloor->getRoomsCompleted() : 0;
}

int DungeonManager::getTotalRoomsCompleted() const { 
    return totalRoomsCompleted; 
}

bool DungeonManager::isBossRoomAvailable() const {
    return currentFloor ? currentFloor->isBossRoomReady() : false;
}

DungeonManager::~DungeonManager() {
    if (currentFloor) {
        delete currentFloor;
    }
}