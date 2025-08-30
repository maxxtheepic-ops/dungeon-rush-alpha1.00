#include "item.h"

// Constructor
Item::Item(int id, String itemName, ItemType itemType) {
    itemID = id;
    name = itemName;
    type = itemType;
    rarity = RARITY_COMMON;
    goldCost = 10;
    sellValue = 5; // Default to 50% of cost
    stackable = false;
    description = "A mysterious item.";
}

// Basic property getters
int Item::getID() const {
    return itemID;
}

String Item::getName() const {
    return name;
}

String Item::getDescription() const {
    return description;
}

ItemType Item::getType() const {
    return type;
}

ItemRarity Item::getRarity() const {
    return rarity;
}

// Economic system
int Item::getGoldCost() const {
    return goldCost;
}

int Item::getSellValue() const {
    return sellValue;
}

void Item::setGoldCost(int cost) {
    if (cost >= 0) {
        goldCost = cost;
        sellValue = cost / 2; // Auto-set sell value to 50% of cost
    }
}

void Item::setSellValue(int value) {
    if (value >= 0) {
        sellValue = value;
    }
}

// Inventory properties
bool Item::isStackable() const {
    return stackable;
}

void Item::setStackable(bool canStack) {
    stackable = canStack;
}

// Rarity system
void Item::setRarity(ItemRarity newRarity) {
    rarity = newRarity;
    
    // Adjust gold cost based on rarity
    float multiplier = 1.0f;
    switch(rarity) {
        case RARITY_COMMON:
            multiplier = 1.0f;
            break;
        case RARITY_UNCOMMON:
            multiplier = 1.5f;
            break;
        case RARITY_RARE:
            multiplier = 2.5f;
            break;
        case RARITY_EPIC:
            multiplier = 4.0f;
            break;
    }
    
    goldCost = (int)(goldCost * multiplier);
    sellValue = goldCost / 2;
}

String Item::getRarityName() const {
    switch(rarity) {
        case RARITY_COMMON:
            return "Common";
        case RARITY_UNCOMMON:
            return "Uncommon";
        case RARITY_RARE:
            return "Rare";
        case RARITY_EPIC:
            return "Epic";
        default:
            return "Unknown";
    }
}

// Description system
void Item::setDescription(String desc) {
    description = desc;
}

// Display helpers
String Item::getDisplayName() const {
    // For now, just return name with rarity
    // Later this could include color codes for display
    return "[" + getRarityName() + "] " + name;
}

String Item::getFullDescription() const {
    String fullDesc = getDisplayName() + "\n";
    fullDesc += description + "\n";
    fullDesc += "Value: " + String(goldCost) + " gold";
    
    if (sellValue > 0) {
        fullDesc += " (Sells for " + String(sellValue) + " gold)";
    }
    
    fullDesc += "\n" + getUseDescription();
    
    return fullDesc;
}

// Virtual destructor
Item::~Item() {
    // Base class cleanup (nothing to do for now)
}