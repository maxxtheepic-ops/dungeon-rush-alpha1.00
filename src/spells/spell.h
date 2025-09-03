#ifndef SPELL_H
#define SPELL_H

#include <Arduino.h>
#include <vector>
#include <TFT_eSPI.h>  // For color constants
#include "spell_types.h"

// Forward declarations
class Player;
class Enemy;

struct SpellSynergy {
    ElementType element1;
    ElementType element2;
    int bonusDamage;
    String description;
};

class Spell {
private:
    int spellID;
    String name;
    ElementType element;
    SpellEffect primaryEffect;
    
    // Damage/healing values
    int basePower;
    int manaCost;
    
    // Additional effects
    bool hasSecondaryEffect;
    SpellEffect secondaryEffect;
    int secondaryPower;
    int duration;
    
protected:
    String description;  // Changed to protected so subclasses can access it
    
public:
    // Constructor
    Spell(int id, String spellName, ElementType elem, SpellEffect effect, int power, int cost = 0);
    
    // Basic properties
    int getID() const { return spellID; }
    String getName() const { return name; }
    String getDescription() const { return description; }
    ElementType getElement() const { return element; }
    SpellEffect getPrimaryEffect() const { return primaryEffect; }
    int getBasePower() const { return basePower; }
    int getManaCost() const { return manaCost; }
    
    // Secondary effects
    void addSecondaryEffect(SpellEffect effect, int power, int dur = 0);
    bool hasSecondary() const { return hasSecondaryEffect; }
    SpellEffect getSecondaryEffect() const { return secondaryEffect; }
    int getSecondaryPower() const { return secondaryPower; }
    int getDuration() const { return duration; }
    
    // Usage
    virtual bool cast(Player* caster, Enemy* target, const std::vector<Spell*>& otherSpells = {});
    virtual String getElementName() const;
    virtual String getEffectName() const;
    virtual uint16_t getElementColor() const;
    
    // Synergy calculation
    int calculateSynergyBonus(const std::vector<Spell*>& recentSpells) const;
    
    // Virtual destructor
    virtual ~Spell() {}
};

//============================================================================
// SPELL LIBRARY - Manages player's known spells
//============================================================================

class SpellLibrary {
private:
    std::vector<Spell*> knownSpells;
    std::vector<Spell*> equippedSpells;  // Max 4 equipped
    std::vector<Spell*> recentCasts;     // For synergy tracking
    
    static const int MAX_EQUIPPED = 4;
    static const int MAX_RECENT = 3;     // Track last 3 casts for synergies
    
public:
    SpellLibrary();
    
    // Spell management
    bool learnSpell(Spell* spell);
    bool forgetSpell(int spellID);
    bool equipSpell(int spellID, int slot);
    bool unequipSpell(int slot);
    
    // Spell access
    Spell* getEquippedSpell(int slot) const;
    std::vector<Spell*> getEquippedSpells() const;
    std::vector<Spell*> getKnownSpells() const;
    std::vector<Spell*> getRecentCasts() const;
    
    // Combat integration
    bool castSpell(int slot, Player* caster, Enemy* target);
    void recordCast(Spell* spell);
    void clearRecentCasts(); // Called after combat
    
    // Information
    int getKnownSpellCount() const;
    int getEquippedSpellCount() const;
    bool hasSpell(int spellID) const;
    bool canLearnMore() const; // Always true - can replace spells
    bool hasEquipSlot() const;
    
    // Display
    void displayKnownSpells() const;
    void displayEquippedSpells() const;
    void displaySpellDetails(int spellID) const;
    
    ~SpellLibrary();
};

//============================================================================
// SPECIFIC SPELL IMPLEMENTATIONS
//============================================================================

// FIRE SPELLS
class Fireball : public Spell {
public:
    Fireball() : Spell(1, "Fireball", ELEMENT_FIRE, EFFECT_DAMAGE, 25, 6) {
        description = "A blazing orb of fire that burns enemies.";
    }
};

class Ignite : public Spell {
public:
    Ignite() : Spell(2, "Ignite", ELEMENT_FIRE, EFFECT_DAMAGE_OVER_TIME, 8, 5) {
        description = "Sets the enemy ablaze, dealing damage over time.";
        addSecondaryEffect(EFFECT_DAMAGE_OVER_TIME, 6, 3); // 6 damage for 3 turns
    }
};

class Immolation : public Spell {
public:
    Immolation() : Spell(3, "Immolation", ELEMENT_FIRE, EFFECT_DAMAGE, 35, 12) {
        description = "A devastating fire spell that consumes everything.";
    }
};

// ICE SPELLS  
class FrostBolt : public Spell {
public:
    FrostBolt() : Spell(11, "Frost Bolt", ELEMENT_ICE, EFFECT_DAMAGE, 20, 5) {
        description = "A shard of ice that pierces and slows enemies.";
        addSecondaryEffect(EFFECT_DEBUFF, 3, 2); // -3 speed for 2 turns
    }
};

class IceBarrier : public Spell {
public:
    IceBarrier() : Spell(12, "Ice Barrier", ELEMENT_ICE, EFFECT_SHIELD, 15, 7) {
        description = "Creates a protective barrier of magical ice.";
    }
};

class Blizzard : public Spell {
public:
    Blizzard() : Spell(13, "Blizzard", ELEMENT_ICE, EFFECT_DAMAGE, 30, 11) {
        description = "A freezing storm that devastates the battlefield.";
        addSecondaryEffect(EFFECT_DEBUFF, 5, 2); // -5 attack for 2 turns
    }
};

// LIGHTNING SPELLS
class LightningBolt : public Spell {
public:
    LightningBolt() : Spell(21, "Lightning Bolt", ELEMENT_LIGHTNING, EFFECT_DAMAGE, 28, 6) {
        description = "A crackling bolt of pure electrical energy.";
    }
};

class ChainLightning : public Spell {
public:
    ChainLightning() : Spell(22, "Chain Lightning", ELEMENT_LIGHTNING, EFFECT_DAMAGE, 22, 8) {
        description = "Lightning that jumps between targets with increasing power.";
        // Bonus damage based on recent lightning spells
    }
};

class Shock : public Spell {
public:
    Shock() : Spell(23, "Shock", ELEMENT_LIGHTNING, EFFECT_DEBUFF, 10, 9) {
        description = "Stuns the enemy, reducing their accuracy and speed.";
        addSecondaryEffect(EFFECT_DEBUFF, 8, 3); // Major debuff for 3 turns
    }
};

// ARCANE SPELLS
class MagicMissile : public Spell {
public:
    MagicMissile() : Spell(31, "Magic Missile", ELEMENT_ARCANE, EFFECT_DAMAGE, 18, 4) {
        description = "Reliable arcane projectiles that never miss.";
        // Always hits, good starter spell
    }
};

class ArcaneShield : public Spell {
public:
    ArcaneShield() : Spell(32, "Arcane Shield", ELEMENT_ARCANE, EFFECT_SHIELD, 20, 8) {
        description = "A shimmering barrier of pure magical energy.";
        addSecondaryEffect(EFFECT_BUFF, 5, 3); // +5 to all resistances
    }
};

class PowerSurge : public Spell {
public:
    PowerSurge() : Spell(33, "Power Surge", ELEMENT_ARCANE, EFFECT_BUFF, 12, 10) {
        description = "Channels raw magic to enhance all abilities.";
        addSecondaryEffect(EFFECT_BUFF, 8, 4); // +8 to all stats for 4 turns
    }
};

// EARTH SPELLS
class StoneSpear : public Spell {
public:
    StoneSpear() : Spell(41, "Stone Spear", ELEMENT_EARTH, EFFECT_DAMAGE, 24, 5) {
        description = "Conjures a sharp spear of hardened earth.";
    }
};

class EarthWall : public Spell {
public:
    EarthWall() : Spell(42, "Earth Wall", ELEMENT_EARTH, EFFECT_SHIELD, 25, 9) {
        description = "Raises a protective wall of solid stone.";
        addSecondaryEffect(EFFECT_HEAL, 10, 0); // Also heals 10 HP
    }
};

class Earthquake : public Spell {
public:
    Earthquake() : Spell(43, "Earthquake", ELEMENT_EARTH, EFFECT_DAMAGE, 32, 13) {
        description = "Shakes the very foundations of the battlefield.";
        addSecondaryEffect(EFFECT_DEBUFF, 6, 3); // -6 defense for 3 turns
    }
};

// SHADOW SPELLS
class ShadowBolt : public Spell {
public:
    ShadowBolt() : Spell(51, "Shadow Bolt", ELEMENT_SHADOW, EFFECT_DAMAGE, 22, 6) {
        description = "A bolt of pure darkness that drains life force.";
        addSecondaryEffect(EFFECT_HEAL, 8, 0); // Heals caster for 8 HP
    }
};

class Drain : public Spell {
public:
    Drain() : Spell(52, "Drain", ELEMENT_SHADOW, EFFECT_DAMAGE, 15, 7) {
        description = "Siphons health and energy from the enemy.";
        addSecondaryEffect(EFFECT_HEAL, 15, 0); // Heals equal to damage
    }
};

class DarkRitual : public Spell {
public:
    DarkRitual() : Spell(53, "Dark Ritual", ELEMENT_SHADOW, EFFECT_BUFF, 5, 15) {
        description = "A forbidden ritual that grants immense power.";
        addSecondaryEffect(EFFECT_BUFF, 15, 5); // +15 damage for 5 turns, but costs HP
    }
};

//============================================================================
// SPELL FACTORY - Creates spells by ID
//============================================================================

class SpellFactory {
public:
    static Spell* createSpell(int spellID);
    static Spell* createRandomSpell(int minTier = 1, int maxTier = 3);
    static std::vector<Spell*> createStarterSpells(); // Returns Magic Missile
    static std::vector<Spell*> createSpellsOfElement(ElementType element);
    static std::vector<Spell*> createAllSpells();
    
    // Spell tiers for progression
    static std::vector<int> getTier1Spells(); // Basic spells
    static std::vector<int> getTier2Spells(); // Intermediate spells  
    static std::vector<int> getTier3Spells(); // Advanced spells
};

#endif // SPELL_H