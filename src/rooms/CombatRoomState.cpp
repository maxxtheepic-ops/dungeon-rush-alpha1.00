#include "CombatRoomState.h"
#include "../spells/spell.h"  // For Spell class methods

CombatRoomState::CombatRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) 
    : RoomState(disp, inp, p, e, dm) {
    // Use spell combat menu instead of regular combat menu
    spellCombatMenu = new SpellCombatMenu(display, input, p);
    combatManager = new CombatManager();
    combatHUD = new CombatHUD(display);
    combatTextBox = new CombatTextBox(display);  // Create text box
    combatActive = false;
    showingResultScreen = false;
    
    // Setup text box area (gets coordinates from spell menu)
    int textX, textY, textWidth, textHeight;
    spellCombatMenu->getTextAreaBounds(textX, textY, textWidth, textHeight);
    Serial.println("DEBUG: Text area bounds: x=" + String(textX) + " y=" + String(textY) + 
                   " w=" + String(textWidth) + " h=" + String(textHeight));
    combatTextBox->setTextArea(textX, textY, textWidth, textHeight);
    
    // NEW: Set text box reference in combat manager for synergy display
    combatManager->setTextBox(combatTextBox);
    
    Serial.println("CombatRoomState: Initialized with text box and synergy support");
}

CombatRoomState::~CombatRoomState() {
    delete spellCombatMenu;
    delete combatManager;
    delete combatHUD;
    delete combatTextBox;  // Clean up text box
}

void CombatRoomState::enterRoom() {
    Serial.println("Starting combat in " + currentRoom->getRoomName());
    startCombat();
}

void CombatRoomState::handleRoomInteraction() {
    if (showingResultScreen) {
        // Wait for player input to continue
        if (input->wasPressed(Button::UP) || 
            input->wasPressed(Button::DOWN) ||
            input->wasPressed(Button::A) || 
            input->wasPressed(Button::B)) {
            
            showingResultScreen = false;
            
            // Now process the result
            if (combatManager->getCombatResult() == RESULT_VICTORY) {
                // Check if this was a boss room - Auto go to library
                if (currentRoom && currentRoom->getType() == ROOM_BOSS) {
                    Serial.println("=== BOSS DEFEATED ===");
                    Serial.println("Boss defeated! Automatically going to library...");
                    
                    // Mark room as completed first
                    completeRoom();
                    
                    // Advance to next floor
                    dungeonManager->advanceToNextFloor();
                    Serial.println("Advanced to floor: " + String(dungeonManager->getCurrentFloorNumber()));
                    
                    // Go directly to library (automatic reward for defeating boss)
                    requestStateChange(StateTransition::LIBRARY);
                    Serial.println("=== GOING TO LIBRARY ===");
                    return;
                } else {
                    // Regular room - normal completion, return to door choice
                    Serial.println("Regular enemy defeated - completing room");
                    completeRoom(); // This will trigger return to door choice
                }
            } else {
                // Player was defeated - game over
                Serial.println("Player defeated - game over");
                requestStateChange(StateTransition::GAME_OVER);
            }
        }
    } else if (combatActive && spellCombatMenu->getIsActive()) {
        handleCombatInput();
    }
    
    // Only render text box if it's visible and not showing result screen
    if (!showingResultScreen && combatTextBox->getIsVisible()) {
        combatTextBox->render();
    }
}

void CombatRoomState::exitRoom() {
    Serial.println("Combat completed, exiting room");
    combatActive = false;
    showingResultScreen = false;
    spellCombatMenu->deactivate();
    combatManager->endCombat();
    combatTextBox->show();  // Make sure text box is visible for next combat
}

void CombatRoomState::startCombat() {
    // Create enemy from room
    if (currentRoom) {
        *currentEnemy = currentRoom->createEnemy();
        Serial.println("Combat: Fighting " + currentEnemy->getName());
    } else {
        *currentEnemy = Enemy::createRandomEnemy();
        Serial.println("Combat: Fighting random " + currentEnemy->getName());
    }
    
    // Start combat systems
    combatManager->startCombat(player, currentEnemy);
    
    // DEBUG: Check textBox before setting
    Serial.println("DEBUG CombatRoomState::startCombat() - combatTextBox: " + String(combatTextBox != nullptr ? "NOT NULL" : "NULL"));
    
    // Set text box reference in combat manager for synergy display
    combatManager->setTextBox(combatTextBox);
    
    // DEBUG: Verify it was set
    Serial.println("DEBUG CombatRoomState::startCombat() - Called setTextBox()");
    
    combatHUD->drawFullCombatScreen(player, currentEnemy, combatManager->getTurnCounter());
    
    // Initialize combat text (no stats)
    initializeCombatText();
    
    spellCombatMenu->activate();
    spellCombatMenu->render();
    combatActive = true;
    
    Serial.println("=== COMBAT STARTED ===");
    combatManager->printCombatStatus();
}

void CombatRoomState::initializeCombatText() {
    Serial.println("DEBUG: initializeCombatText() called");
    
    combatTextBox->show();  // Make sure text box is visible
    combatTextBox->clearText();
    Serial.println("DEBUG: Text cleared");
    
    combatTextBox->addText("=== COMBAT BEGINS ===");
    Serial.println("DEBUG: Added combat begins text");
    
    combatTextBox->addText(player->getName() + " vs " + currentEnemy->getName());
    Serial.println("DEBUG: Added vs text");
    
    combatTextBox->render();
    Serial.println("DEBUG: Text box rendered");
}

void CombatRoomState::handleCombatInput() {
    MenuResult result = spellCombatMenu->handleInput();
    spellCombatMenu->render();
    
    if (result == MenuResult::SELECTED) {
        // Convert spell menu selection to PlayerAction
        SpellCombatAction menuAction = spellCombatMenu->getSelectedAction();
        
        PlayerAction playerAction;
        
        switch (menuAction) {
            case SpellCombatAction::CAST_SPELL_1:
                playerAction = ACTION_CAST_SPELL_1;
                break;
            case SpellCombatAction::CAST_SPELL_2:
                playerAction = ACTION_CAST_SPELL_2;
                break;
            case SpellCombatAction::CAST_SPELL_3:
                playerAction = ACTION_CAST_SPELL_3;
                break;
            case SpellCombatAction::CAST_SPELL_4:
                playerAction = ACTION_CAST_SPELL_4;
                break;
            case SpellCombatAction::DEFEND:
                playerAction = ACTION_DEFEND;
                break;
        }
        
        // Show what the player chose in text box BEFORE processing turn
        showPlayerActionText(menuAction);
        
        // Process combat turn (this will now show synergy bonuses in the text box)
        CombatResult combatResult = combatManager->processTurn(playerAction);
        
        // Show what the enemy did after turn processing
        showEnemyActionText();
        
        // Update display
        combatHUD->updateCombatStats(player, currentEnemy, combatManager->getTurnCounter());
        
        // Check if combat is over
        if (combatResult == RESULT_VICTORY) {
            // Hide text box before showing victory screen
            combatTextBox->hide();
            
            combatHUD->drawVictoryScreen();
            combatActive = false;
            spellCombatMenu->deactivate();
            showingResultScreen = true;

            if (currentRoom && currentRoom->getType() == ROOM_BOSS) {
                Serial.println("Boss defeated! Will go to library after button press...");
            } else {
                Serial.println("Regular enemy defeated!");
            }

            // Wait for player input before processing victory
        } else if (combatResult == RESULT_DEFEAT) {
            // Hide text box before showing defeat screen
            combatTextBox->hide();
            
            combatHUD->drawDefeatScreen();
            combatActive = false;
            spellCombatMenu->deactivate();
            showingResultScreen = true;
            
            // Log what type of room we died in
            if (currentRoom) {
                Serial.println("Died in: " + currentRoom->getRoomName());
                if (currentRoom->getType() == ROOM_BOSS) {
                    Serial.println("Death was in boss room!");
                }
            }
        } else {
            // Combat continues
            spellCombatMenu->activate();
            spellCombatMenu->render();
        }
    }
}

void CombatRoomState::showPlayerActionText(SpellCombatAction menuAction) {
    String actionText = "";
    
    switch (menuAction) {
        case SpellCombatAction::CAST_SPELL_1:
        case SpellCombatAction::CAST_SPELL_2:
        case SpellCombatAction::CAST_SPELL_3:
        case SpellCombatAction::CAST_SPELL_4:
            {
                int slotIndex = (int)menuAction;
                auto equippedSpells = player->getEquippedSpells();
                if (slotIndex < equippedSpells.size() && equippedSpells[slotIndex]) {
                    actionText = "casts " + equippedSpells[slotIndex]->getName();
                } else {
                    actionText = "tries to cast empty spell";
                }
            }
            break;
        case SpellCombatAction::DEFEND:
            actionText = "defends with magic";
            break;
    }
    
    if (actionText != "") {
        combatTextBox->showPlayerAction(player->getName(), actionText);
        // Don't render here - let handleRoomInteraction handle it
    }
}

void CombatRoomState::showEnemyActionText() {
    // Get the enemy's last action and show it
    String enemyName = currentEnemy->getName();
    
    // Simple enemy action text based on defending state
    if (currentEnemy->getIsDefending()) {
        combatTextBox->showEnemyAction(enemyName, "defends");
    } else {
        combatTextBox->showEnemyAction(enemyName, "attacks");
    }
    
    // Don't render here - let handleRoomInteraction handle it
}