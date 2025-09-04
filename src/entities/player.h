#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "../spells/spell_types.h"  // Include shared spell types
#include <Arduino.h>
#include <vector>

// Forward declarations to avoid circular dependencies
class Spell;
class SpellLibrary;
class Enemy;
class CombatTextBox;  // NEW: Forward declaration for text box

// Updated PlayerAction enum for spell casting
enum PlayerAction {
    ACTION_CAST_SPELL_1 = 0,  // Cast equipped spell slot 1
    ACTION_CAST_SPELL_2 = 1,  // Cast equipped spell slot 2  
    ACTION_CAST_SPELL_3 = 2,  // Cast equipped spell slot 3
    ACTION_CAST_SPELL_4 = 3,  // Cast equipped spell slot 4
    ACTION_DEFEND = 4         // Magical defense
};

// Spell effect structure for temporary effects
struct ActiveSpellEffect {
    SpellEffect effect;
    int value;
    int remainingDuration;
    String sourceName;
    
    ActiveSpellEffect(SpellEffect e, int v, int d, String name = "") 
        : effect(e), value(v), remainingDuration(d), sourceName(name) {}
};

class Player : public Entity {
private:
    // Base wizard stats (without equipment/spells)
    int baseHP;
    int baseAttack;
    int baseDefense;
    int baseSpeed;
    int baseMana;
    
    // Current mana
    int currentMana;
    int maxMana;
    
    // Equipment bonuses (magical items)
    int equipmentHP;
    int equipmentAttack;
    int equipmentDefense;
    int equipmentSpeed;
    int equipmentMana;
    
    // Spell system
    SpellLibrary* spellLibrary;
    
    // Scroll inventory system
    std::vector<Spell*> scrollInventory;
    static const int MAX_SCROLLS = 20;  // Maximum scrolls player can carry
    
    // Active spell effects (buffs/debuffs)
    std::vector<ActiveSpellEffect> activeEffects;
    
    // Simple inventory (potions only now)
    int healthPotions;
    int manaPotions;
    
    // Currency system
    int gold;
    
public:
    // Constructors
    Player();
    Player(String playerName);
    Player(String playerName, int hp, int atk, int def, int spd, int mana);
    
    // Equipment system (magical items)
    void addEquipmentBonus(int hpBonus, int atkBonus, int defBonus, int spdBonus, int manaBonus = 0);
    void removeEquipmentBonus(int hpBonus, int atkBonus, int defBonus, int spdBonus, int manaBonus = 0);
    void updateStatsFromEquipment();
    
    // Base stats (without equipment/spells)
    int getBaseHP() const;
    int getBaseAttack() const;
    int getBaseDefense() const;
    int getBaseSpeed() const;
    int getBaseMana() const;
    
    // Equipment bonuses
    int getEquipmentHP() const;
    int getEquipmentAttack() const;
    int getEquipmentDefense() const;
    int getEquipmentSpeed() const;
    int getEquipmentMana() const;
    
    // Mana management
    int getCurrentMana() const;
    int getMaxMana() const;
    void spendMana(int amount);
    void restoreMana(int amount);
    void restoreAllMana();
    bool hasEnoughMana(int amount) const;
    
    // Spell system
    SpellLibrary* getSpellLibrary() const;
    bool learnSpell(Spell* spell);
    bool castSpell(int spellSlot, Enemy* target, CombatTextBox* textBox = nullptr);  // UPDATED: Now takes text box parameter
    std::vector<Spell*> getEquippedSpells() const;
    bool hasSpellEquipped() const;
    
    // Scroll inventory management
    bool addScroll(Spell* scroll);              // Add scroll to inventory
    bool removeScroll(int scrollIndex);         // Remove scroll by index
    bool learnSpellFromScroll(int scrollIndex); // Learn spell and consume scroll
    std::vector<Spell*> getScrollInventory() const;  // Get all scrolls
    int getScrollCount() const;                 // Get number of scrolls
    bool hasScrolls() const;                    // Check if player has any scrolls
    bool hasScrollSpace() const;                // Check if can carry more scrolls
    void clearAllScrolls();                     // Clear all scrolls (for game reset)
    
    // Spell effect management
    void addSpellEffect(SpellEffect effect, int value, int duration, String sourceName = "");
    void updateSpellEffects(); // Called each turn
    void clearSpellEffects();
    bool hasActiveEffect(SpellEffect effect) const;
    int getActiveEffectValue(SpellEffect effect) const;
    std::vector<ActiveSpellEffect> getActiveEffects() const;
    
    // Combat stats with spell effects applied
    int getEffectiveAttack() const;    // Base attack + spell buffs
    int getEffectiveDefense() const;   // Base defense + spell shields
    int getEffectiveSpeed() const;     // Base speed + spell effects
    int getShieldValue() const;        // Current magical shield strength
    
    // Potion management
    void addHealthPotions(int amount);
    void addManaPotions(int amount);
    bool useHealthPotion();
    bool useManaPotion();
    int getHealthPotions() const;
    int getManaPotions() const;
    
    // Currency system
    int getGold() const;
    void addGold(int amount);
    bool spendGold(int amount);
    bool hasEnoughGold(int amount) const;
    
    // Wizard combat actions
    int performAttack() override;      // Magical melee attack (weak)
    int performDefend() override;      // Magical defense
    bool performUseItem();            // Use potion
    bool performCastSpell(int slot, Enemy* target, CombatTextBox* textBox = nullptr); // UPDATED: Cast equipped spell with text box
    
    // Turn management
    void startTurn();  // Called at start of each combat turn
    void endTurn();    // Called at end of each combat turn
    
    // Reset methods
    void resetToBaseStats();
    void resetMana();
    
    // Display helpers
    void displaySpellStatus() const;
    void displayActiveEffects() const;
    void displayScrollInventory() const;  // Display scroll inventory
    
    virtual ~Player();
};

#endif