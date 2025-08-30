#include "RoomState.h"

RoomState::RoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) 
    : GameState(disp, inp) {
    player = p;
    currentEnemy = e;
    dungeonManager = dm;
    currentRoom = nullptr;
    roomCompleted = false;
    roomEntered = false;
}

void RoomState::enter() {
    Serial.println("Entering Room State");
    roomCompleted = false;
    roomEntered = false;
    
    // Get the current room from dungeon manager
    if (dungeonManager && dungeonManager->getCurrentFloor()) {
        currentRoom = dungeonManager->getCurrentFloor()->getCurrentRoom();
    }
    
    if (currentRoom) {
        Serial.println("Entering: " + currentRoom->getRoomName());
        enterRoom(); // Call room-specific enter logic
        roomEntered = true;
    } else {
        Serial.println("Error: No current room found!");
        returnToDoorChoice();
    }
}

void RoomState::update() {
    if (!roomEntered || roomCompleted) return;
    
    handleRoomInteraction(); // Call room-specific update logic
    
    // Check if room was completed during interaction
    if (roomCompleted) {
        exitRoom(); // Call room-specific exit logic
        returnToDoorChoice();
    }
}

void RoomState::exit() {
    Serial.println("Exiting Room State");
    roomEntered = false;
}

void RoomState::setCurrentRoom(Room* room) {
    currentRoom = room;
}

void RoomState::completeRoom() {
    roomCompleted = true;
    if (currentRoom) {
        currentRoom->setCompleted(true);
    }
    if (dungeonManager) {
        dungeonManager->markRoomCompleted();
    }
    Serial.println("Room completed!");
}

void RoomState::returnToDoorChoice() {
    requestStateChange(StateTransition::DOOR_CHOICE);
}