// src/menus/MainMenu.h - Updated header with partial update methods
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
    
    // Full screen drawing methods
    void drawFullMenu();
    void drawTitle();
    void drawMenuOptions();
    void drawInstructions();
    
    // NEW: Partial update drawing methods
    void drawMenuCursor(int option);
    void clearMenuCursor(int option);
    void updateMenuSelection();
    
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