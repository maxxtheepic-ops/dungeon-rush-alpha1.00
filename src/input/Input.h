#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>

// Button definitions
#define BUTTON_UP 18
#define BUTTON_DOWN 17
#define BUTTON_A 21
#define BUTTON_B 38

enum class Button {
    UP,
    DOWN,
    A,
    B
};

class Input {
private:
    bool buttonStates[4];
    bool previousButtonStates[4];
    unsigned long lastDebounceTime;
    static const unsigned long debounceDelay = 50;
    
    int getButtonPin(Button button);
    int getButtonIndex(Button button);

public:
    Input();
    void init();
    void update();
    
    // Check if button was just pressed this frame (press event)
    bool wasPressed(Button button);
};

#endif