#ifndef COMBAT_ROOM_STATE_H
#define COMBAT_ROOM_STATE_H

#include "RoomState.h"
#include "../menus/SpellCombatMenu.h"
#include "../combat/combat_manager.h"
#include "../combat/CombatHUD.h"
#include "../combat/CombatTextBox.h"  // NEW: Include text box

class CombatRoomState : public RoomState {
private:
    // Combat-specific systems
    SpellCombatMenu* spellCombatMenu;
    CombatManager* combatManager;
    CombatHUD* combatHUD;
    CombatTextBox* combatTextBox;  // NEW: Add text box
    
    // Combat state
    bool combatActive;
    bool showingResultScreen;
    
    // NEW: Text box integration helpers
    void initializeCombatText();
    void showPlayerActionText(SpellCombatAction menuAction);
    void showEnemyActionText();  // CHANGED: Method to show enemy actions
    
public:
    CombatRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm);
    ~CombatRoomState();
    
    // RoomState interface implementation
    void enterRoom() override;
    void handleRoomInteraction() override;
    void exitRoom() override;
    
private:
    void startCombat();
    void handleCombatInput();
};

#endif