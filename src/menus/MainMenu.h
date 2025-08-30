#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "MenuBase.h"

enum class MainMenuOption {
    START_GAME = 0,
    SETTINGS = 1,
    CREDITS = 2
};

class MainMenu : public MenuBase {
private:
    const char* menuOptions[3] = {"Start Game", "Settings", "Credits"};
    int lastRenderedSelection;
    bool needsRedraw;
    
    void drawTitle();
    void drawMenuOptions();
    
public:
    MainMenu(Display* disp, Input* inp);
    
    // Override base class methods
    void render() override;
    MenuResult handleInput() override;
    void activate() override;
    
    // Get the selected main menu option
    MainMenuOption getSelectedOption() const;
};

#endif