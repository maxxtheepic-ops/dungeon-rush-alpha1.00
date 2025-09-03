// src/game/DoorChoiceState.h - Updated header with partial update methods
#ifndef DOOR_CHOICE_STATE_H
#define DOOR_CHOICE_STATE_H

#include "GameState.h"
#include "../dungeon/DungeonManager.h"

class DoorChoiceState : public GameState {
private:
    DungeonManager* dungeonManager;
    
    // Door choice data
    std::vector<DoorChoice> availableChoices;
    int selectedOption;  // 0=left door, 1=right door (no library option)
    int maxOptions;      // Will be 2 (left, right only)
    bool screenDrawn;
    int lastSelectedOption;
    bool needsFullRedraw;  // NEW: Flag for full screen redraw
    
    // Door content
    String leftDoorIcon, rightDoorIcon;
    String leftDoorDesc, rightDoorDesc;
    
    // Door layout (stored for cursor positioning)
    int leftDoorX, rightDoorX, doorY, doorWidth, doorHeight;
    
    // Full screen drawing methods
    void drawFullScreen();
    void drawHeader();
    void drawDoors();
    void drawFloorProgress();
    void drawControls();
    
    // Door drawing helper
    void drawDoorContent(int doorIndex, int x, int y, int width, int height);
    
    // NEW: Partial update drawing methods
    void drawDoorCursors();
    void drawLeftDoorCursor();
    void drawRightDoorCursor();
    void clearLeftDoorCursor();
    void clearRightDoorCursor();
    void updateDoorSelection();
    
    // Helper methods
    void generateDoorChoices();
    void drawProgressBar(int x, int y, int width, int height, int current, int max);
    String getDoorIconText(DoorIcon icon);
    
public:
    DoorChoiceState(Display* disp, Input* inp, DungeonManager* dm);
    ~DoorChoiceState() = default;
    
    void enter() override;
    void update() override;
    void exit() override;
    
private:
    void handleInput();
};

#endif