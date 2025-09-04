// src/game/MainMenuState.cpp - Fixed version to prevent multiple state changes
#include "MainMenuState.h"

MainMenuState::MainMenuState(Display* disp, Input* inp) : GameState(disp, inp) {
    mainMenu = new MainMenu(display, input);
}

MainMenuState::~MainMenuState() {
    delete mainMenu;
}

void MainMenuState::enter() {
    Serial.println("DEBUG: Entering Main Menu State - resetting selection");
    
    // FIXED: Clear any pending transitions immediately
    nextState = StateTransition::NONE;
    
    mainMenu->reset(); // Reset the menu selection to prevent auto-selection
    mainMenu->activate();
    mainMenu->render();
    
    Serial.println("DEBUG: Main Menu entered - nextState is: " + String((int)nextState));
}

void MainMenuState::update() {
    // FIXED: Don't process input if we already have a pending state transition
    if (nextState != StateTransition::NONE) {
        Serial.println("DEBUG: MainMenuState::update() - already has pending state transition: " + String((int)nextState));
        return; // Exit early to prevent multiple selections
    }
    
    MenuResult result = mainMenu->handleInput();
    
    // ALWAYS render after handling input to ensure visual updates
    mainMenu->render();
    
    // FIXED: Only process selection if we don't already have a pending transition
    if (result == MenuResult::SELECTED && nextState == StateTransition::NONE) {
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