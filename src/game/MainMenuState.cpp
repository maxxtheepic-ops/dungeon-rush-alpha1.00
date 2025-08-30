#include "MainMenuState.h"

MainMenuState::MainMenuState(Display* disp, Input* inp) : GameState(disp, inp) {
    mainMenu = new MainMenu(display, input);
}

MainMenuState::~MainMenuState() {
    delete mainMenu;
}

void MainMenuState::enter() {
    Serial.println("DEBUG: Entering Main Menu State - resetting selection");
    Serial.println("DEBUG: Current nextState before reset: " + String((int)nextState));
    nextState = StateTransition::NONE; // Force clear any pending transitions
    mainMenu->reset(); // Reset the menu selection to prevent auto-selection
    mainMenu->activate();
    mainMenu->render();
    Serial.println("DEBUG: Main Menu entered - nextState is now: " + String((int)nextState));
}

void MainMenuState::update() {
    // Log if there's already a pending state transition (but don't block)
    if (nextState != StateTransition::NONE) {
        Serial.println("WARNING: MainMenuState::update() called with pending nextState: " + String((int)nextState));
        // Don't return here - let it process normally
    }
    
    MenuResult result = mainMenu->handleInput();
    
    // ALWAYS render after handling input to ensure visual updates
    mainMenu->render();
    
    if (result == MenuResult::SELECTED) {
        MainMenuOption option = mainMenu->getSelectedOption();
        
        Serial.println("DEBUG: Main menu option selected: " + String((int)option));
        
        switch (option) {
            case MainMenuOption::START_GAME:
                Serial.println("DEBUG: Start Game selected - going to door choice");
                requestStateChange(StateTransition::DOOR_CHOICE);
                break;
                
            case MainMenuOption::SETTINGS:
                Serial.println("DEBUG: Settings selected");
                requestStateChange(StateTransition::SETTINGS);
                break;
                
            case MainMenuOption::CREDITS:
                Serial.println("DEBUG: Credits selected");
                requestStateChange(StateTransition::CREDITS);
                break;
        }
    }
}

void MainMenuState::exit() {
    Serial.println("DEBUG: Exiting Main Menu State");
    mainMenu->deactivate();
}