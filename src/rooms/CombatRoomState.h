#ifndef COMBAT_ROOM_STATE_H
#define COMBAT_ROOM_STATE_H

#include "RoomState.h"
#include "../menus/SpellCombatMenu.h"  // CHANGED: Use SpellCombatMenu instead of CombatMenu
#include "../combat/combat_manager.h"
#include "../combat/CombatHUD.h"

class CombatRoomState : public RoomState {
private:
    // Combat-specific systems - UPDATED for spell combat
    SpellCombatMenu* spellCombatMenu;  // CHANGED: Use spell combat menu
    CombatManager* combatManager;
    CombatHUD* combatHUD;
    
    // Combat state
    bool combatActive;
    bool showingResultScreen;  // New: waiting for player input on result
    
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