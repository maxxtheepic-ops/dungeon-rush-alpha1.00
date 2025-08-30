#include "Input.h"

Input::Input() {
    lastDebounceTime = 0;
    for (int i = 0; i < 4; i++) {
        buttonStates[i] = false;
        previousButtonStates[i] = false;
    }
}

void Input::init() {
    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);
    pinMode(BUTTON_A, INPUT_PULLUP);
    pinMode(BUTTON_B, INPUT_PULLUP);
}

void Input::update() {
    // Store previous states first
    for (int i = 0; i < 4; i++) {
        previousButtonStates[i] = buttonStates[i];
    }
    
    // Read new states (invert because of pull-up)
    buttonStates[0] = !digitalRead(BUTTON_UP);    // UP
    buttonStates[1] = !digitalRead(BUTTON_DOWN);  // DOWN
    buttonStates[2] = !digitalRead(BUTTON_A);     // A
    buttonStates[3] = !digitalRead(BUTTON_B);     // B
}

bool Input::wasPressed(Button button) {
    // Simple edge detection with basic debounce
    if (millis() - lastDebounceTime < debounceDelay) {
        return false;
    }
    
    int index = getButtonIndex(button);
    bool pressed = buttonStates[index] && !previousButtonStates[index];
    
    if (pressed) {
        lastDebounceTime = millis();
    }
    
    return pressed;
}

int Input::getButtonIndex(Button button) {
    switch (button) {
        case Button::UP: return 0;
        case Button::DOWN: return 1;
        case Button::A: return 2;
        case Button::B: return 3;
        default: return 0;
    }
}