#include "damage_calculator.h"
#include "../utils/constants.h"

// Player attack damage (no AI modifiers)
int DamageCalculator::calculatePlayerAttackDamage(Player* player) {
    if (!player) return 0;
    return player->getAttack();
}

// Enemy attack damage with AI modifiers
int DamageCalculator::calculateEnemyAttackDamage(Enemy* enemy) {
    if (!enemy) return 0;
    
    int baseDamage = enemy->getAttack();
    return applyAIAttackModifier(baseDamage, enemy->getAIType());
}

// Player defense bonus (no AI modifiers)
int DamageCalculator::calculatePlayerDefenseBonus(Player* player) {
    if (!player) return 0;
    return player->getDefense();
}

// Enemy defense bonus with AI modifiers
int DamageCalculator::calculateEnemyDefenseBonus(Enemy* enemy) {
    if (!enemy) return 0;
    
    int baseDefense = enemy->getDefense();
    return applyAIDefenseModifier(baseDefense, enemy->getAIType());
}

// Calculate final damage after applying defense
int DamageCalculator::calculateFinalDamage(int baseDamage, int totalDefense) {
    int finalDamage = baseDamage - totalDefense;
    
    // Ensure minimum damage
    if (finalDamage < MIN_DAMAGE) {
        finalDamage = MIN_DAMAGE;
    }
    
    return finalDamage;
}

// Apply AI-specific attack modifiers
int DamageCalculator::applyAIAttackModifier(int baseDamage, AIType aiType) {
    float multiplier = getAIAttackMultiplier(aiType);
    return (int)(baseDamage * multiplier);
}

// Apply AI-specific defense modifiers
int DamageCalculator::applyAIDefenseModifier(int baseDefense, AIType aiType) {
    float multiplier = getAIDefenseMultiplier(aiType);
    return (int)(baseDefense * multiplier);
}

// Get healing amount from potions
int DamageCalculator::calculatePotionHealing() {
    return POTION_HEAL_AMOUNT;
}

// AI attack multipliers
float DamageCalculator::getAIAttackMultiplier(AIType aiType) {
    switch(aiType) {
        case AI_BERSERKER:
            return 1.2f;  // +20% attack damage
        case AI_AGGRESSIVE:
            return 1.1f;  // +10% attack damage
        case AI_DEFENSIVE:
            return 0.9f;  // -10% attack damage
        case AI_BALANCED:
        default:
            return 1.0f;  // No modifier
    }
}

// AI defense multipliers
float DamageCalculator::getAIDefenseMultiplier(AIType aiType) {
    switch(aiType) {
        case AI_DEFENSIVE:
            return 1.5f;  // +50% defense
        case AI_BALANCED:
            return 1.0f;  // No modifier
        case AI_AGGRESSIVE:
            return 0.9f;  // -10% defense
        case AI_BERSERKER:
            return 0.7f;  // -30% defense
        default:
            return 1.0f;
    }
}