#ifndef SPELL_TYPES_H
#define SPELL_TYPES_H

// Shared enums and types for spell system
// This file avoids circular dependencies between player.h and spell.h

enum ElementType {
    ELEMENT_FIRE,
    ELEMENT_ICE, 
    ELEMENT_LIGHTNING,
    ELEMENT_ARCANE,
    ELEMENT_EARTH,
    ELEMENT_SHADOW
};

enum SpellEffect {
    EFFECT_DAMAGE,           // Direct damage
    EFFECT_DAMAGE_OVER_TIME, // DOT effects
    EFFECT_HEAL,             // Self healing
    EFFECT_SHIELD,           // Temporary defense
    EFFECT_BUFF,             // Stat boosts
    EFFECT_DEBUFF            // Enemy stat reduction
};

#endif // SPELL_TYPES_H