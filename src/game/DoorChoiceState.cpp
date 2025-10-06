// src/game/DoorChoiceState.cpp - Fixed version to prevent multiple redraws
#include "DoorChoiceState.h"
#include "../utils/constants.h"

DoorChoiceState::DoorChoiceState(Display* disp, Input* inp, DungeonManager* dm) : GameState(disp, inp) {
    dungeonManager = dm;
    selectedOption = 0;
    maxOptions = 2;  // Only left door and right door
    screenDrawn = false;
    lastSelectedOption = -1;
    needsFullRedraw = true;
}

void DoorChoiceState::enter() {
    Serial.println("Entering Door Choice State");
    selectedOption = 0;
    screenDrawn = false;
    lastSelectedOption = -1;
    needsFullRedraw = true;
    
    generateDoorChoices();
    drawFullScreen();
    
    // FIXED: Set flags to prevent immediate redraw in update()
    needsFullRedraw = false;
    screenDrawn = true;
    lastSelectedOption = selectedOption;
}

void DoorChoiceState::update() {
    handleInput();
    
    // FIXED: Only update if selection actually changed and we're not doing a full redraw
    if (needsFullRedraw) {
        drawFullScreen();
        needsFullRedraw = false;
        screenDrawn = true;
        lastSelectedOption = selectedOption;
    } else if (lastSelectedOption != selectedOption) {
        updateDoorSelection();
    }
}

void DoorChoiceState::exit() {
    Serial.println("Exiting Door Choice State");
}

void DoorChoiceState::generateDoorChoices() {
    availableChoices = dungeonManager->getAvailableRooms();
    
    if (availableChoices.size() >= 2) {
        // Two doors available
        leftDoorIcon = getDoorIconText(availableChoices[0].icon);
        leftDoorDesc = availableChoices[0].description;
        rightDoorIcon = getDoorIconText(availableChoices[1].icon);
        rightDoorDesc = availableChoices[1].description;
    } else if (availableChoices.size() == 1) {
        // Only boss room (floor complete)
        leftDoorIcon = getDoorIconText(availableChoices[0].icon);
        leftDoorDesc = availableChoices[0].description;
        rightDoorIcon = "[X]";
        rightDoorDesc = "No door here";
    }
}

void DoorChoiceState::drawFullScreen() {
    display->clear();
    
    // Draw static elements
    drawHeader();
    drawDoors();
    drawFloorProgress();
    drawControls();
    
    // Draw initial cursors
    drawDoorCursors();
}

void DoorChoiceState::drawHeader() {
    // Header
    display->drawText("Choose Your", 22, 15, TFT_WHITE, 2);
    display->drawText("Path", 60, 35, TFT_WHITE, 2);
}

void DoorChoiceState::drawDoors() {
    // Layout: Two doors side by side (no library)
    int doorWidth = 70;
    int doorHeight = 100;
    int doorSpacing = 20;
    int leftDoorX = (170/2) - doorWidth - (doorSpacing/2);
    int rightDoorX = (170/2) + (doorSpacing/2);
    int doorY = 60;
    
    // Store door positions for cursor drawing
    this->leftDoorX = leftDoorX;
    this->rightDoorX = rightDoorX;
    this->doorY = doorY;
    this->doorWidth = doorWidth;
    this->doorHeight = doorHeight;
    
    // Draw doors (without selection highlighting)
    drawDoorContent(0, leftDoorX, doorY, doorWidth, doorHeight);
    drawDoorContent(1, rightDoorX, doorY, doorWidth, doorHeight);
}

void DoorChoiceState::drawDoorContent(int doorIndex, int x, int y, int width, int height) {
    // Door border (always white - no selection highlighting)
    display->drawRect(x, y, width, height, TFT_WHITE);
    display->fillRect(x+1, y+1, width-2, height-2, TFT_BLACK);
    
    // Door label
    String doorLabel = (doorIndex == 0) ? "LEFT" : "RIGHT";
    display->drawText(doorLabel.c_str(), x + 10, y + 8, TFT_WHITE);
    
    // Icon
    String icon = (doorIndex == 0) ? leftDoorIcon : rightDoorIcon;
    display->drawText(icon.c_str(), x + 10, y + 25, TFT_WHITE, 2);
    
    // Description (more space now with taller doors)
    String desc = (doorIndex == 0) ? leftDoorDesc : rightDoorDesc;
    
    // Break into multiple lines
    int startY = y + 55;
    int lineHeight = 12;
    int maxChars = 10; // More chars per line with wider doors
    
    String currentLine = "";
    int currentY = startY;
    
    for (int i = 0; i < desc.length(); i++) {
        currentLine += desc.charAt(i);
        
        if (currentLine.length() >= maxChars || i == desc.length() - 1) {
            display->drawText(currentLine.c_str(), x + 2, currentY, TFT_WHITE, 1);
            currentY += lineHeight;
            currentLine = "";
            
            if (currentY > y + height - 15) break; // Don't overflow door
        }
    }
}

void DoorChoiceState::drawDoorCursors() {
    // Draw cursor for selected door
    if (selectedOption == 0) {
        drawLeftDoorCursor();
    } else if (selectedOption == 1) {
        drawRightDoorCursor();
    }
}

void DoorChoiceState::drawLeftDoorCursor() {
    int cursorY = doorY + doorHeight + 8; // Below the door
    int cursorX = leftDoorX + (doorWidth / 2) - 6; // Centered under door
    display->drawText("^", cursorX, cursorY, TFT_WHITE, 2);
}

void DoorChoiceState::drawRightDoorCursor() {
    int cursorY = doorY + doorHeight + 8; // Below the door
    int cursorX = rightDoorX + (doorWidth / 2) - 6; // Centered under door
    display->drawText("^", cursorX, cursorY, TFT_WHITE, 2);
}

void DoorChoiceState::clearLeftDoorCursor() {
    int cursorY = doorY + doorHeight + 8;
    int cursorX = leftDoorX + (doorWidth / 2) - 6;
    // Clear cursor area underneath door
    display->fillRect(cursorX, cursorY, 12, 16, TFT_BLACK);
}

void DoorChoiceState::clearRightDoorCursor() {
    int cursorY = doorY + doorHeight + 8;
    int cursorX = rightDoorX + (doorWidth / 2) - 6;
    // Clear cursor area underneath door
    display->fillRect(cursorX, cursorY, 12, 16, TFT_BLACK);
}

void DoorChoiceState::updateDoorSelection() {
    // Clear old cursor
    if (lastSelectedOption == 0) {
        clearLeftDoorCursor();
    } else if (lastSelectedOption == 1) {
        clearRightDoorCursor();
    }
    
    // Draw new cursor
    if (selectedOption == 0) {
        drawLeftDoorCursor();
    } else if (selectedOption == 1) {
        drawRightDoorCursor();
    }
    
    lastSelectedOption = selectedOption;
}

void DoorChoiceState::drawFloorProgress() {
    Floor* currentFloor = dungeonManager->getCurrentFloor();
    if (currentFloor) {
        // Position at bottom of screen, above controls
        int progressY = 250;
        
        // Floor number
        String floorText = "Floor " + String(dungeonManager->getCurrentFloorNumber());
        display->drawText(floorText.c_str(), 10, progressY, TFT_WHITE, 1);
        
        // Room progress - START FROM 0
        int roomsCompleted = currentFloor->getRoomsCompleted();
        String roomText;
        
        if (currentFloor->isFloorComplete()) {
            roomText = "Boss Room Available!";
            display->drawText(roomText.c_str(), 10, progressY + 12, TFT_RED, 1);
        } else {
            // Show current room number starting from 0
            roomText = "Room " + String(roomsCompleted) + "/" + String(ROOMS_PER_FLOOR);
            display->drawText(roomText.c_str(), 10, progressY + 12, TFT_WHITE, 1);
        }
        
        // Progress bar now shows correct progress
        drawProgressBar(10, progressY + 24, 150, 6, roomsCompleted, ROOMS_PER_FLOOR);
    }
}

void DoorChoiceState::drawProgressBar(int x, int y, int width, int height, int current, int max) {
    // Draw progress bar background
    display->drawRect(x, y, width, height, TFT_WHITE);
    
    // Draw progress fill
    if (current > 0 && max > 0) {
        int fillWidth = (width * current) / max;
        uint16_t fillColor = TFT_GREEN;
        
        // Change color based on progress
        if (current >= max) {
            fillColor = TFT_RED; // Boss ready
        } else if (current >= max * 0.7) {
            fillColor = TFT_WHITE; // Getting close
        }
        
        display->fillRect(x + 1, y + 1, fillWidth - 2, height - 2, fillColor);
    }
}

void DoorChoiceState::drawControls() {
    int controlY = 280;

}

void DoorChoiceState::handleInput() {
    // Navigation with UP/DOWN (only 2 options now)
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) {
            selectedOption = maxOptions - 1;  // Wrap to right door
        }
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption >= maxOptions) {
            selectedOption = 0;  // Wrap to left door
        }
    }
    
    // Selection with A button
    if (input->wasPressed(Button::A)) {
        if (selectedOption == 0) {
            // Left door
            Room* selectedRoom = dungeonManager->selectRoom(0);
            if (selectedRoom) {
                Serial.println("Selected LEFT door - " + selectedRoom->getRoomName() + " (Type: " + String(selectedRoom->getType()) + ")");
                
                switch (selectedRoom->getType()) {
                    case ROOM_ENEMY:
                        Serial.println("-> Going to Combat (Enemy Room)");
                        requestStateChange(StateTransition::COMBAT);
                        break;
                    case ROOM_BOSS:
                        Serial.println("-> Going to Combat (Boss Room)");
                        requestStateChange(StateTransition::COMBAT);
                        break;
                    case ROOM_SHOP:
                        Serial.println("-> Going to Library (Shop Room converted to Library)");
                        requestStateChange(StateTransition::LIBRARY);
                        break;
                    case ROOM_TREASURE:
                        Serial.println("-> Going to Treasure Room");
                        requestStateChange(StateTransition::TREASURE);
                        break;
                    default:
                        Serial.println("-> Unknown room type, going to main menu");
                        requestStateChange(StateTransition::MAIN_MENU);
                        break;
                }
            } else {
                Serial.println("ERROR: Failed to select left door room");
            }
        } else if (selectedOption == 1 && availableChoices.size() > 1) {
            // Right door (only if available)
            Room* selectedRoom = dungeonManager->selectRoom(1);
            if (selectedRoom) {
                Serial.println("Selected RIGHT door - " + selectedRoom->getRoomName() + " (Type: " + String(selectedRoom->getType()) + ")");
                
                switch (selectedRoom->getType()) {
                    case ROOM_ENEMY:
                        Serial.println("-> Going to Combat (Enemy Room)");
                        requestStateChange(StateTransition::COMBAT);
                        break;
                    case ROOM_BOSS:
                        Serial.println("-> Going to Combat (Boss Room)");
                        requestStateChange(StateTransition::COMBAT);
                        break;
                    case ROOM_SHOP:
                        Serial.println("-> Going to Library (Shop Room converted to Library)");
                        requestStateChange(StateTransition::LIBRARY);
                        break;
                    case ROOM_TREASURE:
                        Serial.println("-> Going to Treasure Room");
                        requestStateChange(StateTransition::TREASURE);
                        break;
                    default:
                        Serial.println("-> Unknown room type, going to main menu");
                        requestStateChange(StateTransition::MAIN_MENU);
                        break;
                }
            } else {
                Serial.println("ERROR: Failed to select right door room");
            }
        }
    }
    
    // B button for quick exit (optional)
    if (input->wasPressed(Button::B)) {
        requestStateChange(StateTransition::MAIN_MENU);
    }
}

String DoorChoiceState::getDoorIconText(DoorIcon icon) {
    switch(icon) {
        case ICON_SWORD: return "<=|-";
        case ICON_QUESTION: return "???";
        case ICON_SKULL: return "[X]";
        default: return "[*]";
    }
}