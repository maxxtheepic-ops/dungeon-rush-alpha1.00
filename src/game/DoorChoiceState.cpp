#include "DoorChoiceState.h"
#include "../utils/constants.h"

DoorChoiceState::DoorChoiceState(Display* disp, Input* inp, DungeonManager* dm) : GameState(disp, inp) {
    dungeonManager = dm;
    selectedOption = 0;
    maxOptions = 3;  // Left door, right door, library
    screenDrawn = false;
    lastSelectedOption = -1;
}

void DoorChoiceState::enter() {
    Serial.println("Entering Door Choice State");
    selectedOption = 0;
    screenDrawn = false;
    lastSelectedOption = -1;
    
    generateDoorChoices();
    drawScreen();
}

void DoorChoiceState::update() {
    handleInput();
    
    // Redraw if selection changed
    if (lastSelectedOption != selectedOption || !screenDrawn) {
        drawScreen();
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

void DoorChoiceState::drawScreen() {
    display->clear();
    
    // Header
    display->drawText("Choose Your Path", 30, 15, TFT_CYAN, 2);
    
    // Layout: Two doors side by side, library below center
    int doorWidth = 60;
    int doorHeight = 80;
    int doorSpacing = 20;
    int leftDoorX = (170/2) - doorWidth - (doorSpacing/2);
    int rightDoorX = (170/2) + (doorSpacing/2);
    int doorY = 50; // Moved up slightly to make room at bottom
    
    // Library position (centered below doors)
    int libraryWidth = 80;
    int libraryHeight = 50;
    int libraryX = (170 - libraryWidth) / 2;
    int libraryY = doorY + doorHeight + 15; // Reduced spacing
    
    // Draw doors
    drawDoor(0, leftDoorX, doorY, doorWidth, doorHeight, selectedOption == 0);
    drawDoor(1, rightDoorX, doorY, doorWidth, doorHeight, selectedOption == 1);
    
    // Draw library
    drawLibrary(libraryX, libraryY, libraryWidth, libraryHeight, selectedOption == 2);
    
    // Draw floor progress at the bottom
    drawFloorProgress();
    
    // Controls at very bottom
    int controlY = 280; // Near bottom of screen
    display->drawText("UP/DOWN: Navigate", 10, controlY, TFT_WHITE, 1);
    display->drawText("A: Select", 10, controlY + 12, TFT_WHITE, 1);
    
    screenDrawn = true;
    lastSelectedOption = selectedOption;
}

void DoorChoiceState::drawFloorProgress() {
    Floor* currentFloor = dungeonManager->getCurrentFloor();
    if (currentFloor) {
        // Position at bottom of screen, above controls
        int progressY = 250; // Above the control text
        
        // Floor number
        String floorText = "Floor " + String(dungeonManager->getCurrentFloorNumber());
        display->drawText(floorText.c_str(), 10, progressY, TFT_YELLOW, 1);
        
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
            fillColor = TFT_YELLOW; // Getting close
        }
        
        display->fillRect(x + 1, y + 1, fillWidth - 2, height - 2, fillColor);
    }
}

void DoorChoiceState::drawDoor(int doorIndex, int x, int y, int width, int height, bool selected) {
    // Door border
    uint16_t borderColor = selected ? TFT_YELLOW : TFT_WHITE;
    uint16_t bgColor = selected ? TFT_BLUE : TFT_BLACK;
    
    // Draw door frame
    display->drawRect(x, y, width, height, borderColor);
    display->fillRect(x+1, y+1, width-2, height-2, bgColor);
    
    // Door label
    String doorLabel = (doorIndex == 0) ? "LEFT" : "RIGHT";
    display->drawText(doorLabel.c_str(), x + 10, y + 8, TFT_WHITE);
    
    // Icon
    String icon = (doorIndex == 0) ? leftDoorIcon : rightDoorIcon;
    display->drawText(icon.c_str(), x + 20, y + 25, TFT_CYAN, 2);
    
    // Description (simplified for narrow doors)
    String desc = (doorIndex == 0) ? leftDoorDesc : rightDoorDesc;
    
    // Break into multiple lines
    int startY = y + 50;
    int lineHeight = 12;
    int maxChars = 8; // Max chars per line for narrow door
    
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
    
    // Selection indicator
    if (selected) {
        display->drawText(">", x - 8, y + height/2, TFT_YELLOW);
        display->drawText("<", x + width + 1, y + height/2, TFT_YELLOW);
    }
}

void DoorChoiceState::handleInput() {
    // Navigation with UP/DOWN
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) {
            selectedOption = maxOptions - 1;  // Wrap to bottom (library)
        }
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption >= maxOptions) {
            selectedOption = 0;  // Wrap to top (left door)
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
                        Serial.println("-> Going to Shop Room");
                        requestStateChange(StateTransition::SHOP);
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
                        Serial.println("-> Going to Shop Room");
                        requestStateChange(StateTransition::SHOP);
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
        } else if (selectedOption == 2) {
            // Library - go to library room state
            Serial.println("DEBUG: Selected LIBRARY - about to request state change");
            Serial.println("DEBUG: Current state before transition: " + String((int)StateTransition::LIBRARY));
            requestStateChange(StateTransition::LIBRARY);
            Serial.println("DEBUG: State change requested successfully");
        }
    }
    
    // B button for quick exit (optional)
    if (input->wasPressed(Button::B)) {
        requestStateChange(StateTransition::MAIN_MENU);
    }
}

void DoorChoiceState::drawLibrary(int x, int y, int width, int height, bool selected) {
    // Library border
    uint16_t borderColor = selected ? TFT_YELLOW : TFT_WHITE;
    uint16_t bgColor = selected ? TFT_BLUE : TFT_BLACK;
    
    // Draw library frame
    display->drawRect(x, y, width, height, borderColor);
    display->fillRect(x+1, y+1, width-2, height-2, bgColor);
    
    // Library icon (magical book)
    display->drawText("[*]", x + width/2 - 8, y + 8, TFT_PURPLE, 2);
    
    // Label
    display->drawText("LIBRARY", x + 15, y + 25, TFT_WHITE);
    
    // Description
    display->drawText("Learn & Rest", x + 8, y + 35, TFT_CYAN, 1);
    
    // Selection indicator
    if (selected) {
        display->drawText(">", x - 8, y + height/2, TFT_YELLOW);
        display->drawText("<", x + width + 1, y + height/2, TFT_YELLOW);
    }
}

String DoorChoiceState::getDoorIconText(DoorIcon icon) {
    switch(icon) {
        case ICON_SWORD: return "[!]";
        case ICON_QUESTION: return "[?]";
        case ICON_SKULL: return "[X]";
        default: return "[*]";
    }
}