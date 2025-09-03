#include "player.h"
#include "../utils/constants.h"
#include "../spells/spell.h"  // Include spell header in the cpp file

// Default constructor - creates a starting wizard
Player::Player() : Entity("Wizard", WIZARD_START_HP, WIZARD_START_ATK, WIZARD_START_DEF, WIZARD_START_SPD) {
    baseHP = WIZARD_START_HP;
    baseAttack = WIZARD_START_ATK;
    baseDefense = WIZARD_START_DEF;
    baseSpeed = WIZARD_START_SPD;
    baseMana = WIZARD_START_MANA;
    
    currentMana = baseMana;
    maxMana = baseMana;
    
    equipmentHP = 0;
    equipmentAttack = 0;
    equipmentDefense = 0;
    equipmentSpeed = 0;
    equipmentMana = 0;
    
    // Initialize spell library with starter spell
    spellLibrary = new SpellLibrary();
    auto starterSpells = SpellFactory::createStarterSpells();
    for (Spell* spell : starterSpells) {
        spellLibrary->learnSpell(spell);
        spellLibrary->equipSpell(spell->getID(), 0); // Equip to slot 1
    }
    
    // Initialize scroll inventory
    scrollInventory.clear();
    
    healthPotions = STARTING_POTIONS;
    manaPotions = 2; // Start with some mana potions
    gold = STARTING_GOLD;
    
    activeEffects.clear();
}

// Constructor with name
Player::Player(String playerName) : Entity(playerName, WIZARD_START_HP, WIZARD_START_ATK, WIZARD_START_DEF, WIZARD_START_SPD) {
    baseHP = WIZARD_START_HP;
    baseAttack = WIZARD_START_ATK;
    baseDefense = WIZARD_START_DEF;
    baseSpeed = WIZARD_START_SPD;
    baseMana = WIZARD_START_MANA;
    
    currentMana = baseMana;
    maxMana = baseMana;
    
    equipmentHP = 0;
    equipmentAttack = 0;
    equipmentDefense = 0;
    equipmentSpeed = 0;
    equipmentMana = 0;
    
    // Initialize spell library with starter spell
    spellLibrary = new SpellLibrary();
    auto starterSpells = SpellFactory::createStarterSpells();
    for (Spell* spell : starterSpells) {
        spellLibrary->learnSpell(spell);
        spellLibrary->equipSpell(spell->getID(), 0); // Equip to slot 1
    }
    
    // Initialize scroll inventory
    scrollInventory.clear();
    
    healthPotions = STARTING_POTIONS;
    manaPotions = 2;
    gold = STARTING_GOLD;
    
    activeEffects.clear();
}

// Constructor with custom stats
Player::Player(String playerName, int hp, int atk, int def, int spd, int mana) : Entity(playerName, hp, atk, def, spd) {
    baseHP = hp;
    baseAttack = atk;
    baseDefense = def;
    baseSpeed = spd;
    baseMana = mana;
    
    currentMana = mana;
    maxMana = mana;
    
    equipmentHP = 0;
    equipmentAttack = 0;
    equipmentDefense = 0;
    equipmentSpeed = 0;
    equipmentMana = 0;
    
    spellLibrary = new SpellLibrary();
    scrollInventory.clear();
    
    healthPotions = STARTING_POTIONS;
    manaPotions = 2;
    gold = STARTING_GOLD;
    
    activeEffects.clear();
}

Player::~Player() {
    delete spellLibrary;
    
    // Clean up scroll inventory
    clearAllScrolls();
}

//============================================================================
// SCROLL INVENTORY SYSTEM IMPLEMENTATION
//============================================================================

bool Player::addScroll(Spell* scroll) {
    if (!scroll) return false;
    
    if (!hasScrollSpace()) {
        Serial.println("Scroll inventory is full! (Max: " + String(MAX_SCROLLS) + ")");
        return false;
    }
    
    scrollInventory.push_back(scroll);
    Serial.println("Added scroll to inventory: " + scroll->getName() + " (" + String(scrollInventory.size()) + "/" + String(MAX_SCROLLS) + ")");
    return true;
}

bool Player::removeScroll(int scrollIndex) {
    if (scrollIndex < 0 || scrollIndex >= scrollInventory.size()) {
        return false;
    }
    
    Spell* scroll = scrollInventory[scrollIndex];
    String scrollName = scroll->getName();
    
    delete scroll;
    scrollInventory.erase(scrollInventory.begin() + scrollIndex);
    
    Serial.println("Removed scroll from inventory: " + scrollName);
    return true;
}

bool Player::learnSpellFromScroll(int scrollIndex) {
    if (scrollIndex < 0 || scrollIndex >= scrollInventory.size()) {
        Serial.println("Invalid scroll index: " + String(scrollIndex));
        return false;
    }
    
    Spell* scroll = scrollInventory[scrollIndex];
    
    // Check if player already knows this spell
    if (spellLibrary->hasSpell(scroll->getID())) {
        Serial.println("Already know spell: " + scroll->getName());
        return false;
    }
    
    // Create a new spell instance for the spell library
    Spell* spellCopy = SpellFactory::createSpell(scroll->getID());
    if (!spellCopy) {
        Serial.println("Failed to create spell copy for: " + scroll->getName());
        return false;
    }
    
    // Learn the spell
    if (spellLibrary->learnSpell(spellCopy)) {
        String spellName = scroll->getName();
        
        // Remove the scroll from inventory (consume it)
        delete scroll;
        scrollInventory.erase(scrollInventory.begin() + scrollIndex);
        
        Serial.println("Learned spell from scroll: " + spellName);
        return true;
    } else {
        // Failed to learn - clean up the copy
        delete spellCopy;
        Serial.println("Failed to learn spell from scroll: " + scroll->getName());
        return false;
    }
}

std::vector<Spell*> Player::getScrollInventory() const {
    return scrollInventory;
}

int Player::getScrollCount() const {
    return scrollInventory.size();
}

bool Player::hasScrolls() const {
    return !scrollInventory.empty();
}

bool Player::hasScrollSpace() const {
    return scrollInventory.size() < MAX_SCROLLS;
}

void Player::clearAllScrolls() {
    for (Spell* scroll : scrollInventory) {
        delete scroll;
    }
    scrollInventory.clear();
    Serial.println("Cleared all scrolls from inventory");
}

void Player::displayScrollInventory() const {
    Serial.println("=== SCROLL INVENTORY ===");
    Serial.println("Scrolls: " + String(scrollInventory.size()) + "/" + String(MAX_SCROLLS));
    
    if (scrollInventory.empty()) {
        Serial.println("No scrolls in inventory.");
        Serial.println("Find them in treasure chests or defeat bosses!");
        return;
    }
    
    for (int i = 0; i < scrollInventory.size(); i++) {
        Spell* scroll = scrollInventory[i];
        Serial.println(String(i + 1) + ". " + scroll->getName() + 
                      " (" + scroll->getElementName() + 
                      ") - Power: " + String(scroll->getBasePower()));
    }
}

//============================================================================
// EXISTING PLAYER METHODS (unchanged)
//============================================================================

// Equipment system with mana support
void Player::addEquipmentBonus(int hpBonus, int atkBonus, int defBonus, int spdBonus, int manaBonus) {
    equipmentHP += hpBonus;
    equipmentAttack += atkBonus;
    equipmentDefense += defBonus;
    equipmentSpeed += spdBonus;
    equipmentMana += manaBonus;
    
    updateStatsFromEquipment();
}

void Player::removeEquipmentBonus(int hpBonus, int atkBonus, int defBonus, int spdBonus, int manaBonus) {
    equipmentHP -= hpBonus;
    equipmentAttack -= atkBonus;
    equipmentDefense -= defBonus;
    equipmentSpeed -= spdBonus;
    equipmentMana -= manaBonus;
    
    // Don't let bonuses go negative
    if (equipmentHP < 0) equipmentHP = 0;
    if (equipmentAttack < 0) equipmentAttack = 0;
    if (equipmentDefense < 0) equipmentDefense = 0;
    if (equipmentSpeed < 0) equipmentSpeed = 0;
    if (equipmentMana < 0) equipmentMana = 0;
    
    updateStatsFromEquipment();
}

void Player::updateStatsFromEquipment() {
    // Calculate total stats
    int totalHP = baseHP + equipmentHP;
    int totalAttack = baseAttack + equipmentAttack;
    int totalDefense = baseDefense + equipmentDefense;
    int totalSpeed = baseSpeed + equipmentSpeed;
    int totalMana = baseMana + equipmentMana;
    
    // Store current percentages
    float hpPercentage = (float)currentHP / (float)maxHP;
    float manaPercentage = (float)currentMana / (float)maxMana;
    
    // Update stats
    maxHP = totalHP;
    attack = totalAttack;
    defense = totalDefense;
    speed = totalSpeed;
    maxMana = totalMana;
    
    // Restore percentages
    currentHP = (int)(maxHP * hpPercentage);
    currentMana = (int)(maxMana * manaPercentage);
    
    if (currentHP < 1 && isAlive()) currentHP = 1;
    if (currentMana < 0) currentMana = 0;
    if (currentMana > maxMana) currentMana = maxMana;
}

// Mana management
int Player::getCurrentMana() const {
    return currentMana;
}

int Player::getMaxMana() const {
    return maxMana;
}

void Player::spendMana(int amount) {
    currentMana -= amount;
    if (currentMana < 0) currentMana = 0;
}

void Player::restoreMana(int amount) {
    currentMana += amount;
    if (currentMana > maxMana) currentMana = maxMana;
}

void Player::restoreAllMana() {
    currentMana = maxMana;
}

bool Player::hasEnoughMana(int amount) const {
    return currentMana >= amount;
}

// Spell system integration
SpellLibrary* Player::getSpellLibrary() const {
    return spellLibrary;
}

bool Player::learnSpell(Spell* spell) {
    return spellLibrary->learnSpell(spell);
}

bool Player::castSpell(int spellSlot, Enemy* target) {
    return spellLibrary->castSpell(spellSlot, this, target);
}

std::vector<Spell*> Player::getEquippedSpells() const {
    return spellLibrary->getEquippedSpells();
}

bool Player::hasSpellEquipped() const {
    return spellLibrary->getEquippedSpellCount() > 0;
}

// Spell effect management
void Player::addSpellEffect(SpellEffect effect, int value, int duration, String sourceName) {
    activeEffects.push_back(ActiveSpellEffect(effect, value, duration, sourceName));
    Serial.println("Applied spell effect: " + sourceName + " (" + String(duration) + " turns)");
}

void Player::updateSpellEffects() {
    // Process and update all active effects
    for (int i = activeEffects.size() - 1; i >= 0; i--) {
        ActiveSpellEffect& effect = activeEffects[i];
        
        // Apply effect
        switch (effect.effect) {
            case EFFECT_DAMAGE_OVER_TIME:
                takeDamage(effect.value);
                Serial.println("DoT: " + String(effect.value) + " damage from " + effect.sourceName);
                break;
            case EFFECT_HEAL:
                heal(effect.value);
                Serial.println("HoT: " + String(effect.value) + " healing from " + effect.sourceName);
                break;
            // BUFF/DEBUFF/SHIELD effects are passive and applied in getEffective* methods
        }
        
        // Decrease duration
        effect.remainingDuration--;
        
        // Remove expired effects
        if (effect.remainingDuration <= 0) {
            Serial.println("Spell effect expired: " + effect.sourceName);
            activeEffects.erase(activeEffects.begin() + i);
        }
    }
}

void Player::clearSpellEffects() {
    activeEffects.clear();
    Serial.println("All spell effects cleared");
}

bool Player::hasActiveEffect(SpellEffect effect) const {
    for (const ActiveSpellEffect& active : activeEffects) {
        if (active.effect == effect) return true;
    }
    return false;
}

int Player::getActiveEffectValue(SpellEffect effect) const {
    int totalValue = 0;
    for (const ActiveSpellEffect& active : activeEffects) {
        if (active.effect == effect) {
            totalValue += active.value;
        }
    }
    return totalValue;
}

std::vector<ActiveSpellEffect> Player::getActiveEffects() const {
    return activeEffects;
}

// Combat stats with spell effects
int Player::getEffectiveAttack() const {
    int effectiveAtk = attack + getActiveEffectValue(EFFECT_BUFF);
    // Debuffs reduce attack
    effectiveAtk -= getActiveEffectValue(EFFECT_DEBUFF) / 2;
    return max(1, effectiveAtk);
}

int Player::getEffectiveDefense() const {
    int effectiveDef = defense + getActiveEffectValue(EFFECT_BUFF);
    int shieldValue = getShieldValue();
    return effectiveDef + shieldValue;
}

int Player::getEffectiveSpeed() const {
    int effectiveSpd = speed + getActiveEffectValue(EFFECT_BUFF);
    // Debuffs reduce speed
    effectiveSpd -= getActiveEffectValue(EFFECT_DEBUFF) / 3;
    return max(1, effectiveSpd);
}

int Player::getShieldValue() const {
    return getActiveEffectValue(EFFECT_SHIELD);
}

// Potion management
void Player::addHealthPotions(int amount) {
    healthPotions += amount;
}

void Player::addManaPotions(int amount) {
    manaPotions += amount;
}

bool Player::useHealthPotion() {
    if (healthPotions > 0) {
        healthPotions--;
        heal(POTION_HEAL_AMOUNT);
        Serial.println("Used health potion! Restored " + String(POTION_HEAL_AMOUNT) + " HP");
        return true;
    }
    return false;
}

bool Player::useManaPotion() {
    if (manaPotions > 0) {
        manaPotions--;
        restoreMana(MANA_POTION_RESTORE);
        Serial.println("Used mana potion! Restored " + String(MANA_POTION_RESTORE) + " mana");
        return true;
    }
    return false;
}

int Player::getHealthPotions() const {
    return healthPotions;
}

int Player::getManaPotions() const {
    return manaPotions;
}

// Currency system
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
    return false;
}

bool Player::hasEnoughGold(int amount) const {
    return gold >= amount;
}

// Wizard combat actions
int Player::performAttack() {
    // Wizards have weak physical attacks
    resetDefense();
    return getEffectiveAttack() / 2; // Half damage for melee
}

int Player::performDefend() {
    // Magical defense - creates a temporary shield
    setDefending(true);
    int defenseValue = getEffectiveDefense() / 2;
    addTemporaryDefense(defenseValue);
    
    // Also add a small magical shield effect
    addSpellEffect(EFFECT_SHIELD, defenseValue, 2, "Magical Defense");
    
    return defenseValue;
}

bool Player::performUseItem() {
    // For now, prioritize health potions
    if (getCurrentHP() < getMaxHP() && healthPotions > 0) {
        return useHealthPotion();
    } else if (getCurrentMana() < getMaxMana() && manaPotions > 0) {
        return useManaPotion();
    }
    
    Serial.println("No usable items available!");
    return false;
}

bool Player::performCastSpell(int slot, Enemy* target) {
    return castSpell(slot, target);
}

// Turn management
void Player::startTurn() {
    // Called at the start of each combat turn
    resetDefense(); // Clear defend state from last turn
    
    // Natural mana regeneration (1 point per turn)
    restoreMana(MANA_REGEN_PER_TURN);
}

void Player::endTurn() {
    // Called at the end of each combat turn
    updateSpellEffects(); // Process ongoing spell effects
}

// Reset methods
void Player::resetToBaseStats() {
    // Reset equipment bonuses
    equipmentHP = 0;
    equipmentAttack = 0;
    equipmentDefense = 0;
    equipmentSpeed = 0;
    equipmentMana = 0;
    
    // Clear spell effects
    clearSpellEffects();
    
    // Clear all scrolls
    clearAllScrolls();
    
    // Reset spell library to starter spell
    delete spellLibrary;
    spellLibrary = new SpellLibrary();
    auto starterSpells = SpellFactory::createStarterSpells();
    for (Spell* spell : starterSpells) {
        spellLibrary->learnSpell(spell);
        spellLibrary->equipSpell(spell->getID(), 0);
    }
    
    updateStatsFromEquipment();
    
    Serial.println("Player stats reset to base wizard values (including scrolls cleared)");
}

void Player::resetMana() {
    currentMana = maxMana;
}

// Base stat getters
int Player::getBaseHP() const { return baseHP; }
int Player::getBaseAttack() const { return baseAttack; }
int Player::getBaseDefense() const { return baseDefense; }
int Player::getBaseSpeed() const { return baseSpeed; }
int Player::getBaseMana() const { return baseMana; }

// Equipment bonus getters
int Player::getEquipmentHP() const { return equipmentHP; }
int Player::getEquipmentAttack() const { return equipmentAttack; }
int Player::getEquipmentDefense() const { return equipmentDefense; }
int Player::getEquipmentSpeed() const { return equipmentSpeed; }
int Player::getEquipmentMana() const { return equipmentMana; }

// Display helpers
void Player::displaySpellStatus() const {
    Serial.println("=== WIZARD STATUS ===");
    Serial.println("HP: " + String(currentHP) + "/" + String(maxHP));
    Serial.println("Mana: " + String(currentMana) + "/" + String(maxMana));
    Serial.println("Gold: " + String(gold));
    Serial.println("Health Potions: " + String(healthPotions));
    Serial.println("Mana Potions: " + String(manaPotions));
    Serial.println("Scrolls: " + String(scrollInventory.size()) + "/" + String(MAX_SCROLLS));
    
    Serial.println("\n=== EQUIPPED SPELLS ===");
    spellLibrary->displayEquippedSpells();
    
    if (!activeEffects.empty()) {
        Serial.println("\n=== ACTIVE EFFECTS ===");
        displayActiveEffects();
    }
    
    if (!scrollInventory.empty()) {
        Serial.println();
        displayScrollInventory();
    }
}

void Player::displayActiveEffects() const {
    for (const ActiveSpellEffect& effect : activeEffects) {
        String effectName = "";
        switch (effect.effect) {
            case EFFECT_SHIELD: effectName = "Shield"; break;
            case EFFECT_BUFF: effectName = "Buff"; break;
            case EFFECT_DEBUFF: effectName = "Debuff"; break;
            case EFFECT_DAMAGE_OVER_TIME: effectName = "DoT"; break;
            case EFFECT_HEAL: effectName = "HoT"; break;
            default: effectName = "Unknown"; break;
        }
        
        Serial.println(effect.sourceName + " (" + effectName + "): " + 
                      String(effect.value) + " for " + String(effect.remainingDuration) + " turns");
    }
}