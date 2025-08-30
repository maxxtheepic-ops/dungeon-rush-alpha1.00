#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "../item.h"

// Forward declaration
class Player;

enum EquipmentSlot {
    SLOT_WEAPON,
    SLOT_ARMOR,
    SLOT_ACCESSORY
};

class Equipment : public Item {
protected:
    EquipmentSlot slot;
    int hpBonus;
    int attackBonus;
    int defenseBonus;
    int speedBonus;
    bool isEquipped;
    
public:
    // Constructor
    Equipment(int id, String name, EquipmentSlot equipSlot);
    
    // Item interface implementation
    bool use(Player* player) override;
    String getUseDescription() const override;
    
    // Equipment-specific properties
    EquipmentSlot getSlot() const;
    int getHPBonus() const;
    int getAttackBonus() const;
    int getDefenseBonus() const;
    int getSpeedBonus() const;
    
    bool getIsEquipped() const;
    void setEquipped(bool equipped);
    
    // Stat management
    void setStatBonuses(int hp, int atk, int def, int spd);
    String getStatsDescription() const;
    String getSlotName() const;
    
    // Equipment actions
    bool equip(Player* player);
    bool unequip(Player* player);
};

// ==============================================
// SPECIFIC EQUIPMENT ITEMS
// ==============================================

// WEAPONS
class RustyDagger : public Equipment {
public:
    RustyDagger();
};

class IronSword : public Equipment {
public:
    IronSword();
};

class SteelSword : public Equipment {
public:
    SteelSword();
};

// ARMOR
class LeatherArmor : public Equipment {
public:
    LeatherArmor();
};

class ChainMail : public Equipment {
public:
    ChainMail();
};

class PlateArmor : public Equipment {
public:
    PlateArmor();
};

// ACCESSORIES
class SpeedBoots : public Equipment {
public:
    SpeedBoots();
};

class HealthRing : public Equipment {
public:
    HealthRing();
};

class PowerGloves : public Equipment {
public:
    PowerGloves();
};

#endif