#ifndef MAIN_MENU_STATE_H
#define MAIN_MENU_STATE_H

#include "GameState.h"
#include "../menus/MainMenu.h"

class MainMenuState : public GameState {
private:
    MainMenu* mainMenu;
    
public:
    MainMenuState(Display* disp, Input* inp);
    ~MainMenuState();
    
    void enter() override;
    void update() override;
    void exit() override;
};

#endif