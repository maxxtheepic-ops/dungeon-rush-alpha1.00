#include "inventory.h"
#include "item_types/consumable.h"
#include "item_types/equipment.h"
#include "../entities/player.h"

// Constructor
Inventory::Inventory(int slots) {
    maxSlots = slots;
    equippedWeapon = nullptr;
    equippedArmor = nullptr;
    equippedAccessory = nullptr;
    items.clear();
}

// Add item to inventory
bool Inventory::addItem(Item* item, int quantity) {
    if (!item || quantity <= 0) return false;
    
    // Check if we have space
    if (!hasSpace(item, quantity)) {
        Serial.println("Inventory is full!");
        return false;
    }
    
    // If item is stackable, try to add to existing stack
    if (item->isStackable()) {
        int existingIndex = findItemIndex(item->getID());
        if (existingIndex != -1) {
            items[existingIndex].quantity += quantity;
            Serial.println("Added " + String(quantity) + "x " + item->getName() + " to inventory.");
            return true;
        }
    }
    
    // Add as new slot
    items.push_back(InventorySlot(item, quantity));
    Serial.println("Added " + String(quantity) + "x " + item->getName() + " to inventory.");
    return true;
}

// Remove item from inventory
bool Inventory::removeItem(int itemID, int quantity) {
    int index = findItemIndex(itemID);
    if (index == -1) return false;
    
    if (items[index].quantity >= quantity) {
        items[index].quantity -= quantity;
        
        // Remove slot if quantity reaches 0
        if (items[index].quantity == 0) {
            // Don't delete the item here - it might be referenced elsewhere (like equipped items)
            // Just remove from inventory
            items.erase(items.begin() + index);
        }
        return true;
    }
    
    return false; // Not enough quantity
}

// Check if inventory has item
bool Inventory::hasItem(int itemID, int quantity) const {
    int index = findItemIndex(itemID);
    if (index == -1) return false;
    
    return items[index].quantity >= quantity;
}

// Get item by ID
Item* Inventory::getItemByID(int itemID) const {
    int index = findItemIndex(itemID);
    if (index != -1) {
        return items[index].item;
    }
    return nullptr;
}

// Use item from inventory
bool Inventory::useItem(int inventoryIndex, Player* player) {
    if (inventoryIndex < 0 || inventoryIndex >= items.size()) {
        Serial.println("Invalid item selection!");
        return false;
    }
    
    Item* item = items[inventoryIndex].item;
    if (!item) return false;
    
    // Special handling for equipment
    if (item->getType() == ITEM_EQUIPMENT) {
        Equipment* equipment = static_cast<Equipment*>(item);
        return equipOrUnequipItem(equipment, player);
    }
    
    // Handle consumables normally
    if (item->use(player)) {
        // Successfully used, remove one from inventory
        removeItem(item->getID(), 1);
        return true;
    }
    
    return false; // Item couldn't be used
}

// Use item by ID
bool Inventory::useItemByID(int itemID, Player* player) {
    int index = findItemIndex(itemID);
    if (index != -1) {
        return useItem(index, player);
    }
    
    Serial.println("Item not found in inventory!");
    return false;
}

// Helper method to find item index
int Inventory::findItemIndex(int itemID) const {
    for (int i = 0; i < items.size(); i++) {
        if (items[i].item && items[i].item->getID() == itemID) {
            return i;
        }
    }
    return -1; // Not found
}

// Find empty slot
int Inventory::findEmptySlot() const {
    return items.size(); // Next available index
}

// Information methods
int Inventory::getItemCount() const {
    return items.size();
}

int Inventory::getMaxSlots() const {
    return maxSlots;
}

bool Inventory::isFull() const {
    return getItemCount() >= maxSlots;
}

int Inventory::getQuantity(int itemID) const {
    int index = findItemIndex(itemID);
    if (index != -1) {
        return items[index].quantity;
    }
    return 0;
}

// Equipment slot management
bool Inventory::equipOrUnequipItem(Equipment* equipment, Player* player) {
    if (!equipment || !player) return false;
    
    EquipmentSlot slot = equipment->getSlot();
    Equipment** currentSlot = nullptr;
    
    // Get the appropriate equipment slot
    switch(slot) {
        case SLOT_WEAPON:
            currentSlot = &equippedWeapon;
            break;
        case SLOT_ARMOR:
            currentSlot = &equippedArmor;
            break;
        case SLOT_ACCESSORY:
            currentSlot = &equippedAccessory;
            break;
        default:
            return false;
    }
    
    // If this item is already equipped, unequip it
    if (equipment->getIsEquipped()) {
        equipment->unequip(player);
        *currentSlot = nullptr;
        return true;
    }
    
    // If there's already something equipped in this slot, unequip it first
    if (*currentSlot != nullptr) {
        (*currentSlot)->unequip(player);
        Serial.println("Unequipped " + (*currentSlot)->getName() + " to make room.");
        *currentSlot = nullptr; // Clear the slot
    }
    
    // Equip the new item
    if (equipment->equip(player)) {
        *currentSlot = equipment;
        return true;
    }
    
    return false;
}

// Access methods
std::vector<InventorySlot> Inventory::getItems() const {
    return items;
}

Item* Inventory::getItem(int index) const {
    if (index >= 0 && index < items.size()) {
        return items[index].item;
    }
    return nullptr;
}

int Inventory::getSlotQuantity(int index) const {
    if (index >= 0 && index < items.size()) {
        return items[index].quantity;
    }
    return 0;
}

// Equipment getters
Equipment* Inventory::getEquippedWeapon() const {
    return equippedWeapon;
}

Equipment* Inventory::getEquippedArmor() const {
    return equippedArmor;
}

Equipment* Inventory::getEquippedAccessory() const {
    return equippedAccessory;
}

// Display inventory
void Inventory::displayInventory() const {
    Serial.println("=== INVENTORY ===");
    Serial.println("Slots used: " + String(getItemCount()) + "/" + String(maxSlots));
    
    if (items.empty()) {
        Serial.println("Inventory is empty.");
        return;
    }
    
    for (int i = 0; i < items.size(); i++) {
        Item* item = items[i].item;
        int qty = items[i].quantity;
        
        if (qty > 1) {
            Serial.println(String(i + 1) + ". " + item->getDisplayName() + " (x" + String(qty) + ")");
        } else {
            Serial.println(String(i + 1) + ". " + item->getDisplayName());
        }
        Serial.println("   " + item->getUseDescription());
        Serial.println("   Value: " + String(item->getGoldCost()) + " gold");
    }
}

// Display only consumables
void Inventory::displayConsumables() const {
    Serial.println("=== CONSUMABLES ===");
    
    bool foundConsumables = false;
    for (int i = 0; i < items.size(); i++) {
        Item* item = items[i].item;
        if (item->getType() == ITEM_CONSUMABLE) {
            int qty = items[i].quantity;
            foundConsumables = true;
            
            if (qty > 1) {
                Serial.println(String(i + 1) + ". " + item->getName() + " (x" + String(qty) + ")");
            } else {
                Serial.println(String(i + 1) + ". " + item->getName());
            }
            Serial.println("   " + item->getUseDescription());
        }
    }
    
    if (!foundConsumables) {
        Serial.println("No consumable items.");
    }
}

// Display equipped items
void Inventory::displayEquipment() const {
    Serial.println("=== EQUIPPED ITEMS ===");
    
    if (equippedWeapon) {
        Serial.println("Weapon: " + equippedWeapon->getDisplayName());
        Serial.println("  " + equippedWeapon->getStatsDescription());
    } else {
        Serial.println("Weapon: None");
    }
    
    if (equippedArmor) {
        Serial.println("Armor: " + equippedArmor->getDisplayName());
        Serial.println("  " + equippedArmor->getStatsDescription());
    } else {
        Serial.println("Armor: None");
    }
    
    if (equippedAccessory) {
        Serial.println("Accessory: " + equippedAccessory->getDisplayName());
        Serial.println("  " + equippedAccessory->getStatsDescription());
    } else {
        Serial.println("Accessory: None");
    }
}

// Get inventory status string
String Inventory::getInventoryStatus() const {
    return "Inventory: " + String(getItemCount()) + "/" + String(maxSlots) + " slots";
}

// Check if inventory has space for item
bool Inventory::hasSpace(Item* item, int quantity) const {
    if (!item) return false;
    
    // If stackable, check if we already have it
    if (item->isStackable()) {
        int existingIndex = findItemIndex(item->getID());
        if (existingIndex != -1) {
            return true; // Can stack with existing
        }
    }
    
    // Check if we have empty slots
    return getItemCount() < maxSlots;
}

// Sort inventory by type and rarity
void Inventory::sortInventory() {
    // Simple bubble sort for small inventories
    for (int i = 0; i < items.size() - 1; i++) {
        for (int j = 0; j < items.size() - i - 1; j++) {
            Item* item1 = items[j].item;
            Item* item2 = items[j + 1].item;
            
            // Sort by type first, then by rarity
            if (item1->getType() > item2->getType() || 
                (item1->getType() == item2->getType() && item1->getRarity() < item2->getRarity())) {
                
                // Swap slots
                InventorySlot temp = items[j];
                items[j] = items[j + 1];
                items[j + 1] = temp;
            }
        }
    }
}

// Destructor
Inventory::~Inventory() {
    // Clean up all items in inventory
    for (auto& slot : items) {
        if (slot.item) {
            delete slot.item;
            slot.item = nullptr;
        }
    }
    items.clear();
    
    // Note: We don't delete equipped items here because they're the same objects
    // as the ones in the items vector (just pointed to by equipped slots)
    // Setting them to nullptr prevents dangling pointers
    equippedWeapon = nullptr;
    equippedArmor = nullptr;
    equippedAccessory = nullptr;
}