#include "player.h"
#include "../utils/constants.h"

// Default constructor - creates a basic adventurer
Player::Player() : Entity("Adventurer", PLAYER_START_HP, PLAYER_START_ATK, PLAYER_START_DEF, PLAYER_START_SPD) {
    baseHP = PLAYER_START_HP;
    baseAttack = PLAYER_START_ATK;
    baseDefense = PLAYER_START_DEF;
    baseSpeed = PLAYER_START_SPD;
    
    equipmentHP = 0;
    equipmentAttack = 0;
    equipmentDefense = 0;
    equipmentSpeed = 0;
    
    healthPotions = STARTING_POTIONS;
    gold = STARTING_GOLD;
    gold = 50; // Start with some gold for first shop visit
}

// Constructor with name
Player::Player(String playerName) : Entity(playerName, PLAYER_START_HP, PLAYER_START_ATK, PLAYER_START_DEF, PLAYER_START_SPD) {
    baseHP = PLAYER_START_HP;
    baseAttack = PLAYER_START_ATK;
    baseDefense = PLAYER_START_DEF;
    baseSpeed = PLAYER_START_SPD;
    
    equipmentHP = 0;
    equipmentAttack = 0;
    equipmentDefense = 0;
    equipmentSpeed = 0;
    
    healthPotions = STARTING_POTIONS;
    gold = STARTING_GOLD;
}

// Constructor with custom stats
Player::Player(String playerName, int hp, int atk, int def, int spd) : Entity(playerName, hp, atk, def, spd) {
    baseHP = hp;
    baseAttack = atk;
    baseDefense = def;
    baseSpeed = spd;
    
    equipmentHP = 0;
    equipmentAttack = 0;
    equipmentDefense = 0;
    equipmentSpeed = 0;
    
    healthPotions = STARTING_POTIONS;
}

// Equipment system
void Player::addEquipmentBonus(int hpBonus, int atkBonus, int defBonus, int spdBonus) {
    equipmentHP += hpBonus;
    equipmentAttack += atkBonus;
    equipmentDefense += defBonus;
    equipmentSpeed += spdBonus;
    
    updateStatsFromEquipment();
}

void Player::removeEquipmentBonus(int hpBonus, int atkBonus, int defBonus, int spdBonus) {
    equipmentHP -= hpBonus;
    equipmentAttack -= atkBonus;
    equipmentDefense -= defBonus;
    equipmentSpeed -= spdBonus;
    
    // Don't let equipment bonuses go negative
    if (equipmentHP < 0) equipmentHP = 0;
    if (equipmentAttack < 0) equipmentAttack = 0;
    if (equipmentDefense < 0) equipmentDefense = 0;
    if (equipmentSpeed < 0) equipmentSpeed = 0;
    
    updateStatsFromEquipment();
}

void Player::updateStatsFromEquipment() {
    // Calculate total stats
    int totalHP = baseHP + equipmentHP;
    int totalAttack = baseAttack + equipmentAttack;
    int totalDefense = baseDefense + equipmentDefense;
    int totalSpeed = baseSpeed + equipmentSpeed;
    
    // Store current HP percentage to maintain it after stat change
    float hpPercentage = (float)currentHP / (float)maxHP;
    
    // Update stats
    maxHP = totalHP;
    attack = totalAttack;
    defense = totalDefense;
    speed = totalSpeed;
    
    // Restore HP percentage (so player doesn't lose health when equipping items)
    currentHP = (int)(maxHP * hpPercentage);
    if (currentHP < 1 && isAlive()) {
        currentHP = 1;  // Don't accidentally kill player with equipment changes
    }
}

// Get base stats
int Player::getBaseHP() const {
    return baseHP;
}

int Player::getBaseAttack() const {
    return baseAttack;
}

int Player::getBaseDefense() const {
    return baseDefense;
}

int Player::getBaseSpeed() const {
    return baseSpeed;
}

// Get equipment bonuses
int Player::getEquipmentHP() const {
    return equipmentHP;
}

int Player::getEquipmentAttack() const {
    return equipmentAttack;
}

int Player::getEquipmentDefense() const {
    return equipmentDefense;
}

int Player::getEquipmentSpeed() const {
    return equipmentSpeed;
}

// Item management
void Player::addHealthPotions(int amount) {
    healthPotions += amount;
}

bool Player::useHealthPotion() {
    if (healthPotions > 0) {
        healthPotions--;
        heal(POTION_HEAL_AMOUNT);  // Use constant instead of hardcoded 30
        return true;
    }
    return false;  // No potions left
}

int Player::getHealthPotions() const {
    return healthPotions;
}

// Currency system implementation
int Player::getGold() const {
    return gold;
}

void Player::addGold(int amount) {
    if (amount > 0) {
        gold += amount;
    }
}

bool Player::spendGold(int amount) {
    if (hasEnoughGold(amount)) {
        gold -= amount;
        return true;
    }
    return false; // Not enough gold
}

bool Player::hasEnoughGold(int amount) const {
    return gold >= amount;
}

// Combat actions
int Player::performAttack() {
    // Call parent attack (handles defense reset)
    return Entity::performAttack();
}

int Player::performDefend() {
    // Call parent defend
    return Entity::performDefend();
}

bool Player::performUseItem() {
    // For now, only health potions
    return useHealthPotion();
}

// Action selection (placeholder - will be implemented with input system later)
PlayerAction Player::chooseAction() {
    // For now, return attack (this will be replaced with input handling)
    return ACTION_ATTACK;
}

// Reset to base stats (useful for testing)
void Player::resetToBaseStats() {
    equipmentHP = 0;
    equipmentAttack = 0;
    equipmentDefense = 0;
    equipmentSpeed = 0;
    updateStatsFromEquipment();
}

// Destructor
Player::~Player() {
    // Nothing special to clean up
}