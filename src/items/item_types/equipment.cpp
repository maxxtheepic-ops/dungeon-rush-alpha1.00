#include "equipment.h"
#include "../../entities/player.h"
#include "../../utils/constants.h"

// Base Equipment constructor
Equipment::Equipment(int id, String name, EquipmentSlot equipSlot) 
    : Item(id, name, ITEM_EQUIPMENT) {
    slot = equipSlot;
    hpBonus = 0;
    attackBonus = 0;
    defenseBonus = 0;
    speedBonus = 0;
    isEquipped = false;
    setStackable(false); // Equipment doesn't stack
}

// Equipment use - equip/unequip
bool Equipment::use(Player* player) {
    if (!player) return false;
    
    if (isEquipped) {
        return unequip(player);
    } else {
        return equip(player);
    }
}

String Equipment::getUseDescription() const {
    if (isEquipped) {
        return "Use: Unequip " + getSlotName();
    } else {
        return "Use: Equip " + getSlotName() + " - " + getStatsDescription();
    }
}

// Equipment-specific getters
EquipmentSlot Equipment::getSlot() const {
    return slot;
}

int Equipment::getHPBonus() const {
    return hpBonus;
}

int Equipment::getAttackBonus() const {
    return attackBonus;
}

int Equipment::getDefenseBonus() const {
    return defenseBonus;
}

int Equipment::getSpeedBonus() const {
    return speedBonus;
}

bool Equipment::getIsEquipped() const {
    return isEquipped;
}

void Equipment::setEquipped(bool equipped) {
    isEquipped = equipped;
}

// Stat management
void Equipment::setStatBonuses(int hp, int atk, int def, int spd) {
    hpBonus = hp;
    attackBonus = atk;
    defenseBonus = def;
    speedBonus = spd;
}

String Equipment::getStatsDescription() const {
    String stats = "";
    bool hasStats = false;
    
    if (hpBonus > 0) {
        stats += "+" + String(hpBonus) + " HP";
        hasStats = true;
    }
    if (attackBonus > 0) {
        if (hasStats) stats += ", ";
        stats += "+" + String(attackBonus) + " ATK";
        hasStats = true;
    }
    if (defenseBonus > 0) {
        if (hasStats) stats += ", ";
        stats += "+" + String(defenseBonus) + " DEF";
        hasStats = true;
    }
    if (speedBonus > 0) {
        if (hasStats) stats += ", ";
        stats += "+" + String(speedBonus) + " SPD";
        hasStats = true;
    }
    
    return hasStats ? stats : "No stat bonuses";
}

String Equipment::getSlotName() const {
    switch(slot) {
        case SLOT_WEAPON:
            return "Weapon";
        case SLOT_ARMOR:
            return "Armor";
        case SLOT_ACCESSORY:
            return "Accessory";
        default:
            return "Unknown";
    }
}

// Equip item
bool Equipment::equip(Player* player) {
    if (!player || isEquipped) return false;
    
    // Apply stat bonuses using existing equipment system
    player->addEquipmentBonus(hpBonus, attackBonus, defenseBonus, speedBonus);
    setEquipped(true);
    
    Serial.println("Equipped " + getName() + "! (" + getStatsDescription() + ")");
    return true;
}

// Unequip item
bool Equipment::unequip(Player* player) {
    if (!player || !isEquipped) return false;
    
    // Remove stat bonuses using existing equipment system
    player->removeEquipmentBonus(hpBonus, attackBonus, defenseBonus, speedBonus);
    setEquipped(false);
    
    Serial.println("Unequipped " + getName() + ".");
    return true;
}

// ==============================================
// SPECIFIC EQUIPMENT IMPLEMENTATIONS
// ==============================================

// WEAPONS

// Rusty Dagger - starter weapon
RustyDagger::RustyDagger() : Equipment(101, "Rusty Dagger", SLOT_WEAPON) {
    setStatBonuses(0, 2, 0, 1); // +2 ATK, +1 SPD
    setGoldCost(25);
    setDescription("A worn dagger that's seen better days.");
    setRarity(RARITY_COMMON);
}

// Iron Sword - basic weapon
IronSword::IronSword() : Equipment(102, "Iron Sword", SLOT_WEAPON) {
    setStatBonuses(0, 5, 0, 0); // +5 ATK
    setGoldCost(100);
    setDescription("A sturdy iron blade favored by adventurers.");
    setRarity(RARITY_COMMON);
}

// Steel Sword - upgraded weapon
SteelSword::SteelSword() : Equipment(103, "Steel Sword", SLOT_WEAPON) {
    setStatBonuses(0, 8, 1, 0); // +8 ATK, +1 DEF
    setGoldCost(250);
    setDescription("A well-crafted steel blade with excellent balance.");
    setRarity(RARITY_UNCOMMON);
}

// ARMOR

// Leather Armor - starter armor
LeatherArmor::LeatherArmor() : Equipment(201, "Leather Armor", SLOT_ARMOR) {
    setStatBonuses(10, 0, 3, 0); // +10 HP, +3 DEF
    setGoldCost(60);
    setDescription("Basic leather protection for novice adventurers.");
    setRarity(RARITY_COMMON);
}

// Chain Mail - medium armor
ChainMail::ChainMail() : Equipment(202, "Chain Mail", SLOT_ARMOR) {
    setStatBonuses(20, 0, 6, -1); // +20 HP, +6 DEF, -1 SPD
    setGoldCost(150);
    setDescription("Interlocked metal rings provide solid protection.");
    setRarity(RARITY_COMMON);
}

// Plate Armor - heavy armor
PlateArmor::PlateArmor() : Equipment(203, "Plate Armor", SLOT_ARMOR) {
    setStatBonuses(35, 0, 10, -2); // +35 HP, +10 DEF, -2 SPD
    setGoldCost(400);
    setDescription("Heavy steel plates offer maximum protection.");
    setRarity(RARITY_RARE);
}

// ACCESSORIES

// Speed Boots - mobility accessory
SpeedBoots::SpeedBoots() : Equipment(301, "Speed Boots", SLOT_ACCESSORY) {
    setStatBonuses(0, 0, 0, 5); // +5 SPD
    setGoldCost(120);
    setDescription("Enchanted boots that make you fleet of foot.");
    setRarity(RARITY_UNCOMMON);
}

// Health Ring - vitality accessory
HealthRing::HealthRing() : Equipment(302, "Health Ring", SLOT_ACCESSORY) {
    setStatBonuses(25, 0, 2, 0); // +25 HP, +2 DEF
    setGoldCost(180);
    setDescription("A magical ring that enhances vitality.");
    setRarity(RARITY_UNCOMMON);
}

// Power Gloves - strength accessory
PowerGloves::PowerGloves() : Equipment(303, "Power Gloves", SLOT_ACCESSORY) {
    setStatBonuses(0, 4, 0, 0); // +4 ATK
    setGoldCost(140);
    setDescription("Gauntlets imbued with strength-enhancing magic.");
    setRarity(RARITY_UNCOMMON);
}