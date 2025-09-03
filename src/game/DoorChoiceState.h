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
    
    // Door content
    String leftDoorIcon, rightDoorIcon;
    String leftDoorDesc, rightDoorDesc;
    
    // Helper methods
    void generateDoorChoices();
    void drawDoor(int doorIndex, int x, int y, int width, int height, bool selected);
    String getDoorIconText(DoorIcon icon);
    void drawFloorProgress();
    void drawProgressBar(int x, int y, int width, int height, int current, int max);
    
public:
    DoorChoiceState(Display* disp, Input* inp, DungeonManager* dm);
    ~DoorChoiceState() = default;
    
    void enter() override;
    void update() override;
    void exit() override;
    
private:
    void handleInput();
    void drawScreen();
};

#endif