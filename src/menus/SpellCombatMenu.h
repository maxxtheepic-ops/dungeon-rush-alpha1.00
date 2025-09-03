#ifndef SPELL_COMBAT_MENU_H
#define SPELL_COMBAT_MENU_H

#include "MenuBase.h"

// Forward declarations
class Player;
class Spell;

enum class SpellCombatAction {
    CAST_SPELL_1 = 0,
    CAST_SPELL_2 = 1,
    CAST_SPELL_3 = 2,
    CAST_SPELL_4 = 3,
    DEFEND = 4
};

class SpellCombatMenu : public MenuBase {
private:
    Player* player;  // Need access to player's spells
    int lastRenderedSelection;
    bool needsRedraw;
    
    // Spell display data
    struct SpellDisplayInfo {
        String name;
        String shortName;
        uint16_t color;
        bool available;
        int manaCost;
        int power;
    };
    
    SpellDisplayInfo spellInfo[4];
    
    void updateSpellInfo();
    void drawMenuArea();
    void drawSpellSlot(int slot, int x, int y, bool selected);
    void drawDefendOption(int x, int y, bool selected);
    void drawPlayerStatus();
    void clearMenuArea();
    
public:
    SpellCombatMenu(Display* disp, Input* inp, Player* p);
    
    // Override base class methods
    void render() override;
    MenuResult handleInput() override;
    void activate() override;
    
    // Get the selected combat action
    SpellCombatAction getSelectedAction() const;
    
    // Check if selected action is valid
    bool isSelectedActionValid() const;
    
    // Get spell slot for spell actions (0-3)
    int getSelectedSpellSlot() const;
    
    // Update display when spells change
    void refreshSpellData();
};

#endif // SPELL_COMBAT_MENU_H