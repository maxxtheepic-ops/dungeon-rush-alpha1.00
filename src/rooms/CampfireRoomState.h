#ifndef CAMPFIRE_ROOM_STATE_H
#define CAMPFIRE_ROOM_STATE_H

#include "../game/GameState.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../dungeon/DungeonManager.h"

enum class CampfireAction {
    REST = 0,
    INVENTORY = 1,
    LEAVE = 2
};

class CampfireRoomState : public GameState {
private:
    // Game entities (same as RoomState had)
    Player* player;
    Enemy* currentEnemy;
    DungeonManager* dungeonManager;
    
    // Campfire menu state
    int selectedOption;
    int maxOptions;
    bool screenDrawn;
    int lastSelectedOption;
    
    // Screen state management
    bool showingInventory;
    bool showingRestResult;
    
    // Inventory management
    int inventorySelectedItem;
    int inventoryMaxItems;
    
    // Rest costs
    static const int REST_COST = 20;
    
    // Drawing methods
    void drawCampfireMenu();
    void drawInventoryScreen();
    void drawRestResultScreen(bool success, String message);
    
    // Input handling
    void handleMenuInput();
    void handleInventoryInput();
    void handleRestResultInput();
    
    // Actions
    void performRest();
    void openInventory();
    void useSelectedItem();
    void equipSelectedItem();
    void returnToMenu();
    void completeRoom(); // Handle room completion
    
public:
    CampfireRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm);
    ~CampfireRoomState() = default;
    
    // GameState interface implementation
    void enter() override;
    void update() override;
    void exit() override;
};

#endif