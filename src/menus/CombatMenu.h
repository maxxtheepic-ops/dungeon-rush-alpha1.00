#ifndef COMBAT_MENU_H
#define COMBAT_MENU_H

#include "MenuBase.h"

enum class CombatAction {
    ATTACK = 0,
    DEFEND = 1,
    ITEM = 2
};

class CombatMenu : public MenuBase {
private:
    const char* menuOptions[3] = {"Attack", "Defend", "Item"};
    int lastRenderedSelection;
    bool needsRedraw;
    
    void drawMenuArea();
    void clearMenuArea();
    
public:
    CombatMenu(Display* disp, Input* inp);
    
    // Override base class methods
    void render() override;
    MenuResult handleInput() override;
    void activate() override;
    
    // Get the selected combat action
    CombatAction getSelectedAction() const;
};

#endif