#include "enemy.h"
#include "../utils/constants.h"

// Default constructor
Enemy::Enemy() : Entity("Unknown Enemy", 20, 8, 4, 6) {
    aiType = AI_BALANCED;
    spriteFile = "enemies/default.bmp";
    experienceValue = 10;
}

// Basic constructor
Enemy::Enemy(String enemyName, int hp, int atk, int spd) 
    : Entity(enemyName, hp, atk, 4, spd) {  // Default defense of 4
    aiType = AI_BALANCED;
    spriteFile = "enemies/" + enemyName + ".bmp";
    experienceValue = (hp + atk + spd) / 3; // Simple exp calculation
}

// Full constructor with AI type
Enemy::Enemy(String enemyName, int hp, int atk, int spd, AIType ai) 
    : Entity(enemyName, hp, atk, 4, spd) {  // Default defense of 4
    aiType = ai;
    spriteFile = "enemies/" + enemyName + ".bmp";
    experienceValue = (hp + atk + spd) / 3;
}

// AI Decision Making
EnemyAction Enemy::chooseAction() {
    int roll = random(1, 101); // Random number 1-100
    
    switch(aiType) {
        case AI_AGGRESSIVE:
            return (roll <= 80) ? ENEMY_ATTACK : ENEMY_DEFEND;
            
        case AI_DEFENSIVE:
            return (roll <= 40) ? ENEMY_ATTACK : ENEMY_DEFEND;
            
        case AI_BERSERKER:
            return (roll <= 90) ? ENEMY_ATTACK : ENEMY_DEFEND;
            
        case AI_BALANCED:
        default:
            return (roll <= 60) ? ENEMY_ATTACK : ENEMY_DEFEND;
    }
}

// Combat Actions with AI-specific modifiers
int Enemy::performAttack() {
    // Call parent to handle defense reset
    int baseDamage = Entity::performAttack();
    
    // Apply AI-specific attack modifiers
    switch(aiType) {
        case AI_BERSERKER:
            return (baseDamage * 120) / 100; // +20% damage
            
        case AI_DEFENSIVE:
            return (baseDamage * 90) / 100;  // -10% damage
            
        case AI_AGGRESSIVE:
            return (baseDamage * 110) / 100; // +10% damage
            
        case AI_BALANCED:
        default:
            return baseDamage; // No modifier
    }
}

int Enemy::performDefend() {
    // Call parent to handle defense logic
    int baseDefense = Entity::performDefend();
    
    // Apply AI-specific defense modifiers
    switch(aiType) {
        case AI_DEFENSIVE:
            return (baseDefense * 150) / 100; // +50% defense
            
        case AI_BERSERKER:
            return (baseDefense * 70) / 100;  // -30% defense
            
        case AI_AGGRESSIVE:
            return (baseDefense * 90) / 100;  // -10% defense
            
        case AI_BALANCED:
        default:
            return baseDefense; // No modifier
    }
}

// Getters/Setters
void Enemy::setAIType(AIType type) {
    aiType = type;
}

AIType Enemy::getAIType() const {
    return aiType;
}

void Enemy::setSpriteFile(String filename) {
    spriteFile = filename;
}

String Enemy::getSpriteFile() const {
    return spriteFile;
}

void Enemy::setExperienceValue(int exp) {
    experienceValue = exp;
}

int Enemy::getExperienceValue() const {
    return experienceValue;
}

// Simple Enemy Factory Methods
Enemy Enemy::createGoblin() {
    Enemy goblin("Goblin", GOBLIN_HP, GOBLIN_ATK, GOBLIN_SPD, AI_AGGRESSIVE);
    goblin.defense = GOBLIN_DEF;  // Set defense using constant
    goblin.setSpriteFile("enemies/goblin.bmp");
    goblin.setExperienceValue(15);
    return goblin;
}

Enemy Enemy::createSkeleton() {
    Enemy skeleton("Skeleton", SKELETON_HP, SKELETON_ATK, SKELETON_SPD, AI_DEFENSIVE);
    skeleton.defense = SKELETON_DEF;  // Set defense using constant
    skeleton.setSpriteFile("enemies/skeleton.bmp");
    skeleton.setExperienceValue(25);
    return skeleton;
}

Enemy Enemy::createOrc() {
    Enemy orc("Orc Warrior", ORC_HP, ORC_ATK, ORC_SPD, AI_BERSERKER);
    orc.defense = ORC_DEF;  // Set defense using constant
    orc.setSpriteFile("enemies/orc.bmp");
    orc.setExperienceValue(40);
    return orc;
}

Enemy Enemy::createTroll() {
    Enemy troll("Cave Troll", TROLL_HP, TROLL_ATK, TROLL_SPD, AI_BERSERKER);
    troll.defense = TROLL_DEF;
    troll.setSpriteFile("enemies/troll.bmp");
    troll.setExperienceValue(75);
    return troll;
}

Enemy Enemy::createDragon() {
    Enemy dragon("Young Dragon", DRAGON_HP, DRAGON_ATK, DRAGON_SPD, AI_BALANCED);
    dragon.defense = DRAGON_DEF;
    dragon.setSpriteFile("enemies/dragon.bmp");
    dragon.setExperienceValue(100);
    return dragon;
}

Enemy Enemy::createBandit() {
    Enemy bandit("Bandit", 35, 12, 8, AI_AGGRESSIVE);
    bandit.defense = 5;
    bandit.setSpriteFile("enemies/bandit.bmp");
    bandit.setExperienceValue(20);
    return bandit;
}

Enemy Enemy::createRandomEnemy() {
    int enemyType = random(1, 4); // Random 1-3
    
    switch(enemyType) {
        case 1:
            return createGoblin();
        case 2:
            return createSkeleton();
        case 3:
            return createOrc();
        default:
            return createGoblin(); // Fallback
    }
}

// Destructor
Enemy::~Enemy() {
    // Nothing special to clean up
}