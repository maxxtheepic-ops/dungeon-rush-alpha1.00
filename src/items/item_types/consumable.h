#ifndef CONSUMABLE_H
#define CONSUMABLE_H

#include "../item.h"

// Forward declaration
class Player;

enum ConsumableEffect {
    EFFECT_HEAL_HP,
    EFFECT_BOOST_ATTACK,
    EFFECT_BOOST_DEFENSE,
    EFFECT_BOOST_SPEED
};

class Consumable : public Item {
protected:
    ConsumableEffect effect;
    int effectValue;
    int effectDuration; // For temporary effects (0 = permanent/instant)
    
public:
    // Constructor
    Consumable(int id, String name, ConsumableEffect consumableEffect, int value);
    
    // Item interface implementation
    bool use(Player* player) override;
    String getUseDescription() const override;
    
    // Consumable-specific properties
    ConsumableEffect getEffect() const;
    int getEffectValue() const;
    int getEffectDuration() const;
    void setEffectDuration(int duration);
    
    // Helper methods
    String getEffectDescription() const;
};

// ==============================================
// SPECIFIC CONSUMABLE ITEMS
// ==============================================

class HealthPotion : public Consumable {
public:
    HealthPotion();
};

class GreaterHealthPotion : public Consumable {
public:
    GreaterHealthPotion();
};

class StrengthPotion : public Consumable {
public:
    StrengthPotion();
    bool use(Player* player) override; // Custom implementation for attack boost
};

class DefensePotion : public Consumable {
public:
    DefensePotion();
    bool use(Player* player) override; // Custom implementation for defense boost
};

class SpeedPotion : public Consumable {
public:
    SpeedPotion();
    bool use(Player* player) override; // Custom implementation for speed boost
};

#endif