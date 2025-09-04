#include "spell.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../combat/CombatTextBox.h"  // NEW: Include text box
#include <TFT_eSPI.h>  // Add this for TFT color constants

// Static member definition for Meditate
int Meditate::consecutiveUses = 0;

//============================================================================
// SPELL BASE CLASS IMPLEMENTATION
//============================================================================

Spell::Spell(int id, String spellName, ElementType elem, SpellEffect effect, int power, int cost) {
    spellID = id;
    name = spellName;
    element = elem;
    primaryEffect = effect;
    basePower = power;
    manaCost = cost;
    
    hasSecondaryEffect = false;
    secondaryEffect = EFFECT_DAMAGE;
    secondaryPower = 0;
    duration = 0;
    
    // Set default descriptions based on element and effect
    description = "A " + getElementName() + " spell that " + getEffectName() + ".";
}

void Spell::addSecondaryEffect(SpellEffect effect, int power, int dur) {
    hasSecondaryEffect = true;
    secondaryEffect = effect;
    secondaryPower = power;
    duration = dur;
}

bool Spell::cast(Player* caster, Enemy* target, const std::vector<Spell*>& otherSpells, CombatTextBox* textBox) {
    if (!caster || !target) return false;
    
    // Check mana cost
    if (caster->getCurrentMana() < manaCost) {
        Serial.println("Not enough mana to cast " + getName() + "!");
        return false;
    }
    
    // Reset Meditate consecutive uses if this isn't a Meditate spell
    if (spellID != 34) {  // 34 is Meditate's ID
        Meditate::resetConsecutiveUses();
    }
    
    // Spend mana
    caster->spendMana(manaCost);
    
    // Calculate synergy bonus
    int synergyBonus = calculateSynergyBonus(otherSpells);
    int totalPower = basePower + synergyBonus;
    
    // DEBUG: Add this debug output
    Serial.println("DEBUG base Spell::cast() - spellName: " + getName());
    Serial.println("DEBUG base Spell::cast() - synergyBonus: " + String(synergyBonus));
    Serial.println("DEBUG base Spell::cast() - textBox pointer: " + String(textBox != nullptr ? "NOT NULL" : "NULL"));
    
    // Show synergy bonus in text box if present (ONLY ONCE!)
    if (textBox && synergyBonus > 0) {
        Serial.println("DEBUG base Spell::cast() - CALLING textBox->showSynergyBonus() ONCE");
        textBox->showSynergyBonus(getName(), synergyBonus);
    } else {
        Serial.println("DEBUG base Spell::cast() - NOT calling showSynergyBonus - textBox: " + 
                      String(textBox != nullptr ? "exists" : "null") + 
                      ", synergyBonus: " + String(synergyBonus));
    }
    
    // Apply primary effect
    switch (primaryEffect) {
        case EFFECT_DAMAGE:
            target->takeDamage(totalPower);
            Serial.println(getName() + " deals " + String(totalPower) + " damage!");
            if (synergyBonus > 0) {
                Serial.println("Synergy bonus: +" + String(synergyBonus) + " damage!");
            }
            break;
            
        case EFFECT_HEAL:
            if (spellID == 34) {
                // Special handling for Meditate - it restores mana, not HP
                caster->restoreMana(totalPower);
                Serial.println(getName() + " restores " + String(totalPower) + " mana!");
                if (synergyBonus > 0) {
                    Serial.println("Deep meditation bonus: +" + String(synergyBonus) + " mana!");
                }
            } else {
                // Regular healing spells restore HP
                caster->heal(totalPower);
                Serial.println(getName() + " heals " + String(totalPower) + " HP!");
            }
            break;
            
        case EFFECT_SHIELD:
            caster->addSpellEffect(EFFECT_SHIELD, totalPower, 3);
            Serial.println(getName() + " grants " + String(totalPower) + " shield!");
            break;
            
        case EFFECT_BUFF:
            caster->addSpellEffect(EFFECT_BUFF, totalPower, duration);
            Serial.println(getName() + " provides a magical enhancement!");
            break;
            
        case EFFECT_DEBUFF:
            // For now, apply debuff to enemy directly (would need enemy spell effect system)
            Serial.println(getName() + " weakens the enemy!");
            break;
            
        case EFFECT_DAMAGE_OVER_TIME:
            // For now, apply immediate damage (would need enemy spell effect system)
            target->takeDamage(totalPower);
            Serial.println(getName() + " inflicts burning damage!");
            break;
    }
    
    // Apply secondary effect if present
    if (hasSecondaryEffect) {
        switch (secondaryEffect) {
            case EFFECT_HEAL:
                if (spellID == 34) {
                    caster->restoreMana(secondaryPower);
                    Serial.println("  Also restores " + String(secondaryPower) + " mana!");
                } else {
                    caster->heal(secondaryPower);
                    Serial.println("  Also heals " + String(secondaryPower) + " HP!");
                }
                break;
            case EFFECT_DAMAGE_OVER_TIME:
                target->takeDamage(secondaryPower);
                Serial.println("  Also burns for " + String(secondaryPower) + " damage!");
                break;
            case EFFECT_DEBUFF:
                Serial.println("  Also applies debuff!");
                break;
            case EFFECT_BUFF:
                caster->addSpellEffect(EFFECT_BUFF, secondaryPower, duration);
                Serial.println("  Also provides enhancement!");
                break;
        }
    }
    
    return true;
}

String Spell::getElementName() const {
    switch (element) {
        case ELEMENT_FIRE: return "Fire";
        case ELEMENT_ICE: return "Ice";
        case ELEMENT_LIGHTNING: return "Lightning";
        case ELEMENT_ARCANE: return "Arcane";
        case ELEMENT_EARTH: return "Earth";
        case ELEMENT_SHADOW: return "Shadow";
        default: return "Unknown";
    }
}

String Spell::getEffectName() const {
    switch (primaryEffect) {
        case EFFECT_DAMAGE: return "damages enemies";
        case EFFECT_HEAL: return "restores health";
        case EFFECT_SHIELD: return "provides protection";
        case EFFECT_BUFF: return "enhances abilities";
        case EFFECT_DEBUFF: return "weakens foes";
        case EFFECT_DAMAGE_OVER_TIME: return "inflicts lasting harm";
        default: return "has mysterious effects";
    }
}

uint16_t Spell::getElementColor() const {
    switch (element) {
        case ELEMENT_FIRE: return TFT_RED;
        case ELEMENT_ICE: return TFT_WHITE;
        case ELEMENT_LIGHTNING: return TFT_WHITE;
        case ELEMENT_ARCANE: return TFT_MAGENTA;
        case ELEMENT_EARTH: return TFT_GREEN;
        case ELEMENT_SHADOW: return TFT_WHITE;
        default: return TFT_WHITE;
    }
}

int Spell::calculateSynergyBonus(const std::vector<Spell*>& recentSpells) const {
    int bonus = 0;
    
    for (Spell* recentSpell : recentSpells) {
        if (!recentSpell) continue;
        
        // Same element bonus
        if (recentSpell->getElement() == element) {
            bonus += 5; // +5 damage for same element
        }
        
        // Specific synergies
        ElementType recent = recentSpell->getElement();
        
        switch (element) {
            case ELEMENT_FIRE:
                if (recent == ELEMENT_ICE) bonus += 8; // Steam explosion
                if (recent == ELEMENT_EARTH) bonus += 6; // Molten rock
                break;
            case ELEMENT_ICE:
                if (recent == ELEMENT_FIRE) bonus += 8; // Steam explosion
                if (recent == ELEMENT_LIGHTNING) bonus += 7; // Supercooled lightning
                break;
            case ELEMENT_LIGHTNING:
                if (recent == ELEMENT_ICE) bonus += 7; // Supercooled lightning
                if (recent == ELEMENT_ARCANE) bonus += 6; // Arcane storm
                break;
            case ELEMENT_ARCANE:
                if (recent == ELEMENT_LIGHTNING) bonus += 6; // Arcane storm
                if (recent == ELEMENT_SHADOW) bonus += 9; // Void magic
                break;
            case ELEMENT_EARTH:
                if (recent == ELEMENT_FIRE) bonus += 6; // Molten rock
                if (recent == ELEMENT_SHADOW) bonus += 5; // Cursed earth
                break;
            case ELEMENT_SHADOW:
                if (recent == ELEMENT_ARCANE) bonus += 9; // Void magic
                if (recent == ELEMENT_EARTH) bonus += 5; // Cursed earth
                break;
        }
    }
    
    return bonus;
}

//============================================================================
// MEDITATE SPELL IMPLEMENTATION
//============================================================================

bool Meditate::cast(Player* caster, Enemy* target, const std::vector<Spell*>& otherSpells, CombatTextBox* textBox) {
    if (!caster) return false;
    
    // No mana cost for Meditate
    
    // DEBUG: Show current state
    Serial.println("DEBUG Meditate::cast() - consecutiveUses BEFORE increment: " + String(consecutiveUses));
    Serial.println("DEBUG Meditate::cast() - textBox pointer: " + String(textBox != nullptr ? "NOT NULL" : "NULL"));
    
    // Increment consecutive uses
    consecutiveUses++;
    Serial.println("DEBUG Meditate::cast() - consecutiveUses AFTER increment: " + String(consecutiveUses));
    
    // Calculate mana restoration with self-synergy
    int baseManaRestore = basePower; // 5 mana
    int synergyBonus = calculateSynergyBonus(otherSpells);
    int totalManaRestore = baseManaRestore + synergyBonus;
    
    Serial.println("DEBUG Meditate::cast() - baseManaRestore: " + String(baseManaRestore));
    Serial.println("DEBUG Meditate::cast() - synergyBonus: " + String(synergyBonus));
    Serial.println("DEBUG Meditate::cast() - totalManaRestore: " + String(totalManaRestore));
    
    // Show synergy bonus in text box if present and there is a bonus
    if (textBox && synergyBonus > 0) {
        Serial.println("DEBUG Meditate::cast() - CALLING textBox->showSynergyBonus()");
        textBox->showSynergyBonus(getName(), synergyBonus);
    } else {
        Serial.println("DEBUG Meditate::cast() - NOT calling showSynergyBonus - textBox: " + 
                      String(textBox != nullptr ? "exists" : "null") + 
                      ", synergyBonus: " + String(synergyBonus));
    }
    
    // Restore mana
    caster->restoreMana(totalManaRestore);
    
    // Show appropriate message based on consecutive uses
    if (consecutiveUses == 1) {
        Serial.println("Meditate restores " + String(totalManaRestore) + " mana.");
    } else if (consecutiveUses == 2) {
        Serial.println("Deeper meditation restores " + String(totalManaRestore) + " mana.");
    } else {
        Serial.println("Perfect focus restores " + String(totalManaRestore) + " mana!");
    }
    
    if (synergyBonus > 0) {
        Serial.println("Consecutive meditation bonus: +" + String(synergyBonus) + " mana!");
    }
    
    return true;
}

int Meditate::calculateSynergyBonus(const std::vector<Spell*>& recentSpells) const {
    // Self-synergy: Each consecutive use adds +5 mana, capping at +10 (total 15)
    // consecutiveUses: 1 = +0, 2 = +5, 3+ = +10
    
    if (consecutiveUses <= 1) {
        return 0; // First use, no bonus
    } else if (consecutiveUses == 2) {
        return 5; // Second consecutive use, +5 bonus
    } else {
        return 10; // Third+ consecutive use, +10 bonus (capping at 15 total)
    }
}

//============================================================================
// SPELL LIBRARY IMPLEMENTATION
//============================================================================

SpellLibrary::SpellLibrary() {
    knownSpells.clear();
    equippedSpells.resize(MAX_EQUIPPED, nullptr);
    recentCasts.clear();
}

SpellLibrary::~SpellLibrary() {
    // Clean up known spells
    for (Spell* spell : knownSpells) {
        delete spell;
    }
    knownSpells.clear();
    
    // Don't delete equipped spells - they're the same pointers as known spells
    equippedSpells.clear();
    recentCasts.clear();
}

bool SpellLibrary::learnSpell(Spell* spell) {
    if (!spell) return false;
    
    // Check if already known
    for (Spell* known : knownSpells) {
        if (known->getID() == spell->getID()) {
            Serial.println("Spell already known: " + spell->getName());
            return false;
        }
    }
    
    // Learn the spell
    knownSpells.push_back(spell);
    Serial.println("Learned spell: " + spell->getName());
    return true;
}

bool SpellLibrary::forgetSpell(int spellID) {
    for (int i = 0; i < knownSpells.size(); i++) {
        if (knownSpells[i]->getID() == spellID) {
            // Remove from equipped slots first
            for (int j = 0; j < MAX_EQUIPPED; j++) {
                if (equippedSpells[j] && equippedSpells[j]->getID() == spellID) {
                    equippedSpells[j] = nullptr;
                }
            }
            
            // Delete and remove from known spells
            delete knownSpells[i];
            knownSpells.erase(knownSpells.begin() + i);
            return true;
        }
    }
    return false;
}

bool SpellLibrary::equipSpell(int spellID, int slot) {
    if (slot < 0 || slot >= MAX_EQUIPPED) return false;
    
    // Find the spell
    Spell* spellToEquip = nullptr;
    for (Spell* known : knownSpells) {
        if (known->getID() == spellID) {
            spellToEquip = known;
            break;
        }
    }
    
    if (!spellToEquip) return false;
    
    // Equip the spell
    equippedSpells[slot] = spellToEquip;
    Serial.println("Equipped " + spellToEquip->getName() + " to slot " + String(slot + 1));
    return true;
}

bool SpellLibrary::unequipSpell(int slot) {
    if (slot < 0 || slot >= MAX_EQUIPPED) return false;
    
    equippedSpells[slot] = nullptr;
    Serial.println("Unequipped spell from slot " + String(slot + 1));
    return true;
}

Spell* SpellLibrary::getEquippedSpell(int slot) const {
    if (slot < 0 || slot >= MAX_EQUIPPED) return nullptr;
    return equippedSpells[slot];
}

std::vector<Spell*> SpellLibrary::getEquippedSpells() const {
    std::vector<Spell*> equipped;
    for (Spell* spell : equippedSpells) {
        equipped.push_back(spell); // Include nullptrs to maintain slot positions
    }
    return equipped;
}

std::vector<Spell*> SpellLibrary::getKnownSpells() const {
    return knownSpells;
}

std::vector<Spell*> SpellLibrary::getRecentCasts() const {
    return recentCasts;
}

bool SpellLibrary::castSpell(int slot, Player* caster, Enemy* target, CombatTextBox* textBox) {
    if (slot < 0 || slot >= MAX_EQUIPPED) return false;
    
    Spell* spell = equippedSpells[slot];
    if (!spell) return false;
    
    // Cast the spell - Pass text box to spell casting (synergy display handled in Spell::cast())
    if (spell->cast(caster, target, recentCasts, textBox)) {
        recordCast(spell);
        return true;
    }
    
    return false;
}

void SpellLibrary::recordCast(Spell* spell) {
    if (!spell) return;
    
    // Add to recent casts
    recentCasts.push_back(spell);
    
    // Keep only the most recent casts
    while (recentCasts.size() > MAX_RECENT) {
        recentCasts.erase(recentCasts.begin());
    }
}

void SpellLibrary::clearRecentCasts() {
    recentCasts.clear();
}

int SpellLibrary::getKnownSpellCount() const {
    return knownSpells.size();
}

int SpellLibrary::getEquippedSpellCount() const {
    int count = 0;
    for (Spell* spell : equippedSpells) {
        if (spell) count++;
    }
    return count;
}

bool SpellLibrary::hasSpell(int spellID) const {
    for (Spell* spell : knownSpells) {
        if (spell->getID() == spellID) return true;
    }
    return false;
}

bool SpellLibrary::canLearnMore() const {
    return true; // Wizards can always learn more spells (replace if needed)
}

bool SpellLibrary::hasEquipSlot() const {
    return getEquippedSpellCount() < MAX_EQUIPPED;
}

void SpellLibrary::displayKnownSpells() const {
    Serial.println("=== SPELL GRIMOIRE ===");
    Serial.println("Known spells: " + String(knownSpells.size()));
    
    for (int i = 0; i < knownSpells.size(); i++) {
        Spell* spell = knownSpells[i];
        Serial.println(String(i + 1) + ". " + spell->getName() + " (" + 
                      spell->getElementName() + ") - Power: " + String(spell->getBasePower()));
        Serial.println("   " + spell->getDescription());
    }
}

void SpellLibrary::displayEquippedSpells() const {
    Serial.println("=== EQUIPPED SPELLS ===");
    
    for (int i = 0; i < MAX_EQUIPPED; i++) {
        Serial.print("Slot " + String(i + 1) + ": ");
        if (equippedSpells[i]) {
            Serial.println(equippedSpells[i]->getName() + " (" + 
                          equippedSpells[i]->getElementName() + ")");
        } else {
            Serial.println("Empty");
        }
    }
}

void SpellLibrary::displaySpellDetails(int spellID) const {
    for (Spell* spell : knownSpells) {
        if (spell->getID() == spellID) {
            Serial.println("=== " + spell->getName() + " ===");
            Serial.println("Element: " + spell->getElementName());
            Serial.println("Power: " + String(spell->getBasePower()));
            Serial.println("Mana Cost: " + String(spell->getManaCost()));
            Serial.println("Effect: " + spell->getEffectName());
            Serial.println("Description: " + spell->getDescription());
            return;
        }
    }
    Serial.println("Spell not found in grimoire.");
}

//============================================================================
// SPELL FACTORY IMPLEMENTATION
//============================================================================

Spell* SpellFactory::createSpell(int spellID) {
    switch (spellID) {
        // Fire spells
        case 1: return new Fireball();
        case 2: return new Ignite();
        case 3: return new Immolation();
        
        // Ice spells
        case 11: return new FrostBolt();
        case 12: return new IceBarrier();
        case 13: return new Blizzard();
        
        // Lightning spells
        case 21: return new LightningBolt();
        case 22: return new ChainLightning();
        case 23: return new Shock();
        
        // Arcane spells
        case 31: return new MagicMissile();
        case 32: return new ArcaneShield();
        case 33: return new PowerSurge();
        case 34: return new Meditate();  // Meditate spell
        
        // Earth spells
        case 41: return new StoneSpear();
        case 42: return new EarthWall();
        case 43: return new Earthquake();
        
        // Shadow spells
        case 51: return new ShadowBolt();
        case 52: return new Drain();
        case 53: return new DarkRitual();
        
        default:
            Serial.println("Unknown spell ID: " + String(spellID));
            return nullptr;
    }
}

Spell* SpellFactory::createRandomSpell(int minTier, int maxTier) {
    std::vector<int> availableSpells;
    
    if (minTier <= 1 && maxTier >= 1) {
        std::vector<int> tier1 = getTier1Spells();
        availableSpells.insert(availableSpells.end(), tier1.begin(), tier1.end());
    }
    
    if (minTier <= 2 && maxTier >= 2) {
        std::vector<int> tier2 = getTier2Spells();
        availableSpells.insert(availableSpells.end(), tier2.begin(), tier2.end());
    }
    
    if (minTier <= 3 && maxTier >= 3) {
        std::vector<int> tier3 = getTier3Spells();
        availableSpells.insert(availableSpells.end(), tier3.begin(), tier3.end());
    }
    
    if (availableSpells.empty()) return nullptr;
    
    int randomIndex = random(0, availableSpells.size());
    return createSpell(availableSpells[randomIndex]);
}

std::vector<Spell*> SpellFactory::createStarterSpells() {
    std::vector<Spell*> starter;
    starter.push_back(new MagicMissile()); // Reliable damage spell
    starter.push_back(new Meditate());     // Mana restoration spell
    return starter;
}

std::vector<Spell*> SpellFactory::createSpellsOfElement(ElementType element) {
    std::vector<Spell*> elementSpells;
    
    switch (element) {
        case ELEMENT_FIRE:
            elementSpells.push_back(createSpell(1));
            elementSpells.push_back(createSpell(2));
            elementSpells.push_back(createSpell(3));
            break;
        case ELEMENT_ICE:
            elementSpells.push_back(createSpell(11));
            elementSpells.push_back(createSpell(12));
            elementSpells.push_back(createSpell(13));
            break;
        case ELEMENT_LIGHTNING:
            elementSpells.push_back(createSpell(21));
            elementSpells.push_back(createSpell(22));
            elementSpells.push_back(createSpell(23));
            break;
        case ELEMENT_ARCANE:
            elementSpells.push_back(createSpell(31));
            elementSpells.push_back(createSpell(32));
            elementSpells.push_back(createSpell(33));
            elementSpells.push_back(createSpell(34)); // Include Meditate
            break;
        case ELEMENT_EARTH:
            elementSpells.push_back(createSpell(41));
            elementSpells.push_back(createSpell(42));
            elementSpells.push_back(createSpell(43));
            break;
        case ELEMENT_SHADOW:
            elementSpells.push_back(createSpell(51));
            elementSpells.push_back(createSpell(52));
            elementSpells.push_back(createSpell(53));
            break;
    }
    
    return elementSpells;
}

std::vector<int> SpellFactory::getTier1Spells() {
    return {1, 11, 21, 31, 34, 41, 51}; // Added 34 (Meditate) to tier 1
}

std::vector<int> SpellFactory::getTier2Spells() {
    return {2, 12, 22, 32, 42, 52}; // Intermediate spells
}

std::vector<int> SpellFactory::getTier3Spells() {
    return {3, 13, 23, 33, 43, 53}; // Advanced spells
}

std::vector<Spell*> SpellFactory::createAllSpells() {
    std::vector<Spell*> allSpells;
    
    // Create one of each spell
    for (int id : getTier1Spells()) {
        allSpells.push_back(createSpell(id));
    }
    for (int id : getTier2Spells()) {
        allSpells.push_back(createSpell(id));
    }
    for (int id : getTier3Spells()) {
        allSpells.push_back(createSpell(id));
    }
    
    return allSpells;
}