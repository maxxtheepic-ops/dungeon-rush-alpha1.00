#ifndef TREASURE_ROOM_STATE_H
#define TREASURE_ROOM_STATE_H

#include "RoomState.h"

enum class TreasureAction {
    TAKE_TREASURE = 0,
    LEAVE = 1
};

class TreasureRoomState : public RoomState {
private:
    // Treasure room state
    int selectedOption;
    int maxOptions;
    bool screenDrawn;
    int lastSelectedOption;
    bool treasureLooted;
    
    // Drawing methods
    void drawTreasureScreen();
    void showTreasureResult(int gold, int potions);
    
    // Input handling
    void handleTreasureInput();
    
    // Treasure actions
    void takeTreasure();
    
public:
    TreasureRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm);
    ~TreasureRoomState() = default;
    
    // RoomState interface implementation
    void enterRoom() override;
    void handleRoomInteraction() override;
    void exitRoom() override;
};

#endif