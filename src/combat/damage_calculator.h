#ifndef DAMAGE_CALCULATOR_H
#define DAMAGE_CALCULATOR_H

#include "../entities/player.h"
#include "../entities/enemy.h"

class DamageCalculator {
public:
    // Attack damage calculations
    static int calculatePlayerAttackDamage(Player* player);
    static int calculateEnemyAttackDamage(Enemy* enemy);
    
    // Defense calculations
    static int calculatePlayerDefenseBonus(Player* player);
    static int calculateEnemyDefenseBonus(Enemy* enemy);
    
    // Final damage after defense
    static int calculateFinalDamage(int baseDamage, int totalDefense);
    
    // AI-specific modifiers
    static int applyAIAttackModifier(int baseDamage, AIType aiType);
    static int applyAIDefenseModifier(int baseDefense, AIType aiType);
    
    // Healing calculations
    static int calculatePotionHealing();
    
private:
    // Helper functions
    static float getAIAttackMultiplier(AIType aiType);
    static float getAIDefenseMultiplier(AIType aiType);
};

#endif