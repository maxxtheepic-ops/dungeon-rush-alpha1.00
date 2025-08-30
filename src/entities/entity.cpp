#include "entity.h"
#include "../utils/constants.h"

// Default constructor
Entity::Entity() {
    name = "Unknown";
    maxHP = 10;
    currentHP = 10;
    attack = 5;
    defense = 3;
    speed = 5;
    isDefending = false;
    temporaryDefense = 0;
}

// Constructor with parameters
Entity::Entity(String entityName, int hp, int atk, int def, int spd) {
    name = entityName;
    maxHP = hp;
    currentHP = hp;  // Start at full health
    attack = atk;
    defense = def;
    speed = spd;
    isDefending = false;
    temporaryDefense = 0;
}

// Basic getters
String Entity::getName() const {
    return name;
}

int Entity::getCurrentHP() const {
    return currentHP;
}

int Entity::getMaxHP() const {
    return maxHP;
}

int Entity::getAttack() const {
    return attack;
}

int Entity::getDefense() const {
    return defense;
}

// Basic setters
void Entity::setName(String newName) {
    name = newName;
}

int Entity::getSpeed() const {
    return speed;
}

void Entity::setStats(int hp, int atk, int def, int spd) {
    maxHP = hp;
    currentHP = hp;  // Reset to full health when stats change
    attack = atk;
    defense = def;
    speed = spd;
}

// Health management
void Entity::takeDamage(int damage) {
    // Apply base defense + temporary defense
    int totalDefenseValue = defense + temporaryDefense;
    int actualDamage = damage - totalDefenseValue;
    
    // Use constant for minimum damage
    if (actualDamage < MIN_DAMAGE) {
        actualDamage = MIN_DAMAGE;
    }
    
    currentHP -= actualDamage;
    
    // Can't go below 0 HP
    if (currentHP < 0) {
        currentHP = 0;
    }
    
    // Reset temporary defense after taking damage
    resetDefense();
}

void Entity::heal(int amount) {
    currentHP += amount;
    
    // Can't heal above max HP
    if (currentHP > maxHP) {
        currentHP = maxHP;
    }
}

bool Entity::isAlive() const {
    return currentHP > 0;
}

// Combat actions
int Entity::performAttack() {
    // Reset defense when attacking
    resetDefense();
    return attack;  // Basic attack just returns attack value
}

int Entity::performDefend() {
    setDefending(true);
    int defenseValue = defense; // Use base defense stat instead of attack/2
    addTemporaryDefense(defenseValue);
    return defenseValue;
}

void Entity::resetDefense() {
    isDefending = false;
    temporaryDefense = 0;
}

// Defense state management
void Entity::setDefending(bool defending) {
    isDefending = defending;
}

bool Entity::getIsDefending() const {
    return isDefending;
}

void Entity::addTemporaryDefense(int defense) {
    temporaryDefense += defense;
}

int Entity::getTotalDefense() const {
    return defense + temporaryDefense;
}

// Virtual destructor
Entity::~Entity() {
    // Nothing to clean up for basic entity
}