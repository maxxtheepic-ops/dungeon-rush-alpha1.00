#ifndef ENEMY_H
#define ENEMY_H

#include "entity.h"
#include <Arduino.h>

enum AIType {
    AI_AGGRESSIVE,   // Always attacks (80% attack, 20% defend)
    AI_DEFENSIVE,    // Prefers to defend (40% attack, 60% defend)
    AI_BALANCED,     // Mix of attack/defend (60% attack, 40% defend)
    AI_BERSERKER     // High damage, risky (90% attack, 10% defend)
};

enum EnemyAction {
    ENEMY_ATTACK = 0,
    ENEMY_DEFEND = 1
};

class Enemy : public Entity {
private:
    AIType aiType;
    String spriteFile;
    int experienceValue;
    
public:
    // Constructors
    Enemy();
    Enemy(String enemyName, int hp, int atk, int spd);
    Enemy(String enemyName, int hp, int atk, int spd, AIType ai);
    
    // AI behavior
    EnemyAction chooseAction();
    void setAIType(AIType type);
    AIType getAIType() const;
    
    // Sprite management
    void setSpriteFile(String filename);
    String getSpriteFile() const;
    
    // Experience/rewards (for future leveling system)
    void setExperienceValue(int exp);
    int getExperienceValue() const;
    
    // Combat actions (override parent for AI-specific behavior)
    int performAttack() override;
    int performDefend() override;
    
    // Simple enemy factory (we'll expand this later)
    static Enemy createGoblin();
    static Enemy createSkeleton();
    static Enemy createOrc();
    static Enemy createTroll();       // ID 4
    static Enemy createDragon();      // ID 5
    static Enemy createBandit();      // ID 6
    static Enemy createRandomEnemy();
    
    virtual ~Enemy();
};

#endif