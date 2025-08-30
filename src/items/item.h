#ifndef ITEM_H
#define ITEM_H

#include <Arduino.h>

// Forward declaration to avoid circular includes
class Player;

enum ItemType {
    ITEM_CONSUMABLE,
    ITEM_EQUIPMENT
};

enum ItemRarity {
    RARITY_COMMON,     // Basic items
    RARITY_UNCOMMON,   // Slightly better
    RARITY_RARE,       // Good items
    RARITY_EPIC        // Best items
};

class Item {
protected:
    int itemID;
    String name;
    String description;
    ItemType type;
    ItemRarity rarity;
    int goldCost;
    int sellValue;
    bool stackable;
    
public:
    // Constructor
    Item(int id, String itemName, ItemType itemType);
    
    // Basic properties
    int getID() const;
    String getName() const;
    String getDescription() const;
    ItemType getType() const;
    ItemRarity getRarity() const;
    
    // Economic system
    int getGoldCost() const;
    int getSellValue() const;
    void setGoldCost(int cost);
    void setSellValue(int value);
    
    // Inventory properties
    bool isStackable() const;
    void setStackable(bool canStack);
    
    // Rarity system
    void setRarity(ItemRarity newRarity);
    String getRarityName() const;
    
    // Description system
    void setDescription(String desc);
    
    // Usage (pure virtual - must be implemented by subclasses)
    virtual bool use(Player* player) = 0;
    virtual String getUseDescription() const = 0;
    
    // Display helpers
    String getDisplayName() const;
    String getFullDescription() const;
    
    // Virtual destructor for proper inheritance
    virtual ~Item();
};

#endif