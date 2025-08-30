#ifndef INVENTORY_H
#define INVENTORY_H

#include "item.h"
#include <vector>
#include <Arduino.h>

// Forward declarations
class Player;
class Equipment;

struct InventorySlot {
    Item* item;
    int quantity;
    
    InventorySlot(Item* i, int qty = 1) : item(i), quantity(qty) {}
};

class Inventory {
private:
    std::vector<InventorySlot> items;
    int maxSlots;
    
    // Equipment slots (for future equipment system)
    Equipment* equippedWeapon;
    Equipment* equippedArmor;
    Equipment* equippedAccessory;
    
    // Helper methods
    int findItemIndex(int itemID) const;
    int findEmptySlot() const;
    
public:
    // Constructor
    Inventory(int slots = 15);
    
    // Item management
    bool addItem(Item* item, int quantity = 1);
    bool removeItem(int itemID, int quantity = 1);
    bool hasItem(int itemID, int quantity = 1) const;
    Item* getItemByID(int itemID) const;
    
    // Inventory usage
    bool useItem(int inventoryIndex, Player* player);
    bool useItemByID(int itemID, Player* player);
    
    // Equipment management
    bool equipOrUnequipItem(Equipment* equipment, Player* player);
    void displayEquipment() const;
    
    // Information
    int getItemCount() const;
    int getMaxSlots() const;
    bool isFull() const;
    int getQuantity(int itemID) const;
    
    // Access
    std::vector<InventorySlot> getItems() const;
    Item* getItem(int index) const;
    int getSlotQuantity(int index) const;
    
    // Equipment slots (for future use)
    Equipment* getEquippedWeapon() const;
    Equipment* getEquippedArmor() const;
    Equipment* getEquippedAccessory() const;
    
    // Display
    void displayInventory() const;
    void displayConsumables() const;
    String getInventoryStatus() const;
    
    // Utility
    void sortInventory(); // Sort by item type and rarity
    bool hasSpace(Item* item, int quantity = 1) const;
    
    // Cleanup
    ~Inventory();
};

#endif