#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "../input/Input.h"
#include "../graphics/Display.h"

enum class StateTransition {
    NONE,
    MAIN_MENU,
    DOOR_CHOICE,
    COMBAT,
    LIBRARY,       // CHANGED: Library replaces campfire
    SHOP,
    TREASURE,
    GAME_OVER,
    SETTINGS,
    CREDITS,
    QUIT
};

class GameState {
protected:
    Display* display;
    Input* input;
    StateTransition nextState;
    
public:
    GameState(Display* disp, Input* inp);
    virtual ~GameState() = default;
    
    // Core state interface
    virtual void enter() = 0;
    virtual void update() = 0;
    virtual void exit() = 0;
    
    // State transition
    StateTransition getNextState() const { return nextState; }
    void clearTransition() { nextState = StateTransition::NONE; }
    
protected:
    void requestStateChange(StateTransition newState) { nextState = newState; }
};

#endif