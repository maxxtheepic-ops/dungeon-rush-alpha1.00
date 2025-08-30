#include <Arduino.h>
#include <SPI.h>
#include "input/Input.h"
#include "graphics/Display.h"
#include "game/GameStateManager.h"

// Core systems
Input input;
Display display;

// Game state manager handles everything
GameStateManager gameState(&display, &input);

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    // Initialize hardware
    display.init();
    input.init();
    
    // Initialize game
    gameState.initialize();
    
    Serial.println("Setup complete!");
}

void loop() {
    // Update input
    input.update();
    
    // Update game state
    gameState.update();
    
    delay(10);
}