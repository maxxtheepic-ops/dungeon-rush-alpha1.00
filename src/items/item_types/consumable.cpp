#include "consumable.h"
#include "../../entities/player.h"
#include "../../utils/constants.h"

// Base Consumable constructor
Consumable::Consumable(int id, String name, ConsumableEffect consumableEffect, int value) 
    : Item(id, name, ITEM_CONSUMABLE) {
    effect = consumableEffect;
    effectValue = value;
    effectDuration = 0; // Default to instant effect
    setStackable(true); // Most consumables can stack
}

// Base consumable use implementation
bool Consumable::use(Player* player) {
    if (!player) return false;
    
    switch(effect) {
        case EFFECT_HEAL_HP:
            {
                int currentHP = player->getCurrentHP();
                int maxHP = player->getMaxHP();
                
                if (currentHP >= maxHP) {
                    Serial.println("Already at full health!");
                    return false; // Can't use if already at full health
                }
                
                player->heal(effectValue);
                Serial.println("Restored " + String(effectValue) + " HP!");
                return true;
            }
            break;
            
        case EFFECT_BOOST_ATTACK:
        case EFFECT_BOOST_DEFENSE: 
        case EFFECT_BOOST_SPEED:
            // These will be handled by specific subclass implementations
            Serial.println("Used " + getName() + "!");
            return true;
            
        default:
            Serial.println("Unknown consumable effect!");
            return false;
    }
}

String Consumable::getUseDescription() const {
    return "Use: " + getEffectDescription();
}

// Consumable-specific getters
ConsumableEffect Consumable::getEffect() const {
    return effect;
}

int Consumable::getEffectValue() const {
    return effectValue;
}

int Consumable::getEffectDuration() const {
    return effectDuration;
}

void Consumable::setEffectDuration(int duration) {
    effectDuration = duration;
}

// Helper method for effect descriptions
String Consumable::getEffectDescription() const {
    switch(effect) {
        case EFFECT_HEAL_HP:
            return "Restore " + String(effectValue) + " HP";
        case EFFECT_BOOST_ATTACK:
            return "Increase Attack by " + String(effectValue);
        case EFFECT_BOOST_DEFENSE:
            return "Increase Defense by " + String(effectValue);
        case EFFECT_BOOST_SPEED:
            return "Increase Speed by " + String(effectValue);
        default:
            return "Unknown effect";
    }
}

// ==============================================
// SPECIFIC CONSUMABLE IMPLEMENTATIONS
// ==============================================

// Health Potion - basic healing
HealthPotion::HealthPotion() : Consumable(1, "Health Potion", EFFECT_HEAL_HP, POTION_HEAL_AMOUNT) {
    setGoldCost(HEALTH_POTION_COST);
    setDescription("A red potion that restores health when consumed.");
    setRarity(RARITY_COMMON);
}

// Greater Health Potion - stronger healing
GreaterHealthPotion::GreaterHealthPotion() : Consumable(2, "Greater Health Potion", EFFECT_HEAL_HP, 60) {
    setGoldCost(60);
    setDescription("A glowing red potion that restores significant health.");
    setRarity(RARITY_UNCOMMON);
}

// Strength Potion - temporary attack boost
StrengthPotion::StrengthPotion() : Consumable(3, "Strength Potion", EFFECT_BOOST_ATTACK, 5) {
    setGoldCost(75);
    setDescription("A bubbling orange potion that enhances physical power.");
    setRarity(RARITY_UNCOMMON);
    setEffectDuration(10); // Lasts for 10 combat rounds
}

bool StrengthPotion::use(Player* player) {
    if (!player) return false;
    
    // Add temporary equipment bonus (this integrates with existing system!)
    player->addEquipmentBonus(0, effectValue, 0, 0);
    Serial.println("Your muscles bulge with power! (+" + String(effectValue) + " Attack)");
    Serial.println("Effect will last for " + String(effectDuration) + " rooms.");
    return true;
}

// Defense Potion - temporary defense boost  
DefensePotion::DefensePotion() : Consumable(4, "Defense Potion", EFFECT_BOOST_DEFENSE, 4) {
    setGoldCost(70);
    setDescription("A metallic blue potion that hardens the skin.");
    setRarity(RARITY_UNCOMMON);
    setEffectDuration(10);
}

bool DefensePotion::use(Player* player) {
    if (!player) return false;
    
    player->addEquipmentBonus(0, 0, effectValue, 0);
    Serial.println("Your skin hardens like steel! (+" + String(effectValue) + " Defense)");
    Serial.println("Effect will last for " + String(effectDuration) + " rooms.");
    return true;
}

// Speed Potion - temporary speed boost
SpeedPotion::SpeedPotion() : Consumable(5, "Speed Potion", EFFECT_BOOST_SPEED, 6) {
    setGoldCost(65);
    setDescription("A swirling green potion that makes you feel lighter.");
    setRarity(RARITY_UNCOMMON);
    setEffectDuration(10);
}

bool SpeedPotion::use(Player* player) {
    if (!player) return false;
    
    player->addEquipmentBonus(0, 0, 0, effectValue);
    Serial.println("You feel incredibly swift! (+" + String(effectValue) + " Speed)");
    Serial.println("Effect will last for " + String(effectDuration) + " rooms.");
    return true;
}