#include "GameState.h"

GameState::GameState(Display* disp, Input* inp) {
    display = disp;
    input = inp;
    nextState = StateTransition::NONE;
}