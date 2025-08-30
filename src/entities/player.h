#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include <Arduino.h>

enum PlayerAction {
    ACTION_ATTACK = 0,
    ACTION_DEFEND = 1,
    ACTION_USE_ITEM = 2
};

class Player : public Entity {
private:
    // Base stats (without equipment)
    int baseHP;
    int baseAttack;
    int baseDefense;
    int baseSpeed;
    
    // Equipment bonuses
    int equipmentHP;
    int equipmentAttack;
    int equipmentDefense;
    int equipmentSpeed;
    
    // Inventory (simple for now)
    int healthPotions;
    
    // Currency system
    int gold;
    
public:
    // Constructor
    Player();
    Player(String playerName);
    Player(String playerName, int hp, int atk, int def, int spd);
    
    // Equipment system
    void addEquipmentBonus(int hpBonus, int atkBonus, int defBonus, int spdBonus);
    void removeEquipmentBonus(int hpBonus, int atkBonus, int defBonus, int spdBonus);
    void updateStatsFromEquipment();
    
    // Get base stats (without equipment)
    int getBaseHP() const;
    int getBaseAttack() const;
    int getBaseDefense() const;
    int getBaseSpeed() const;
    
    // Get equipment bonuses
    int getEquipmentHP() const;
    int getEquipmentAttack() const;
    int getEquipmentDefense() const;
    int getEquipmentSpeed() const;
    
    // Items
    void addHealthPotions(int amount);
    bool useHealthPotion();
    int getHealthPotions() const;
    
    // Currency system
    int getGold() const;
    void addGold(int amount);
    bool spendGold(int amount);
    bool hasEnoughGold(int amount) const;
    
    // Player-specific combat actions
    int performAttack() override;
    int performDefend() override;
    bool performUseItem();
    
    // Action selection (returns PlayerAction enum)
    PlayerAction chooseAction();  // Will be implemented later with input
    
    // Reset to base stats (useful for testing/debugging)
    void resetToBaseStats();
    
    virtual ~Player();
};

#endif