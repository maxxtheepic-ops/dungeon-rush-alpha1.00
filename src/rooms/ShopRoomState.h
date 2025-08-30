#ifndef SHOP_ROOM_STATE_H
#define SHOP_ROOM_STATE_H

#include "RoomState.h"

enum class ShopAction {
    BUY_POTION = 0,
    LEAVE = 1
};

class ShopRoomState : public RoomState {
private:
    // Shop menu state
    int selectedOption;
    int maxOptions;
    bool screenDrawn;
    int lastSelectedOption;
    
    // Drawing methods
    void drawShopScreen();
    void showPurchaseResult(bool success, String message);
    
    // Input handling
    void handleShopInput();
    
    // Shop actions
    void buyHealthPotion();
    
public:
    ShopRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm);
    ~ShopRoomState() = default;
    
    // RoomState interface implementation
    void enterRoom() override;
    void handleRoomInteraction() override;
    void exitRoom() override;
};

#endif