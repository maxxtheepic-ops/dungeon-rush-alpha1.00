#ifndef ENTITY_H
#define ENTITY_H

#include <Arduino.h>

class Entity {
protected:
    // Core stats
    String name;
    int maxHP;
    int currentHP;
    int attack;
    int defense;
    int speed;
    
    // Combat state
    bool isDefending;
    int temporaryDefense;
    
public:
    // Constructors
    Entity();
    Entity(String entityName, int hp, int atk, int def, int spd);
    
    // Basic getters
    String getName() const;
    int getCurrentHP() const;
    int getMaxHP() const;
    int getAttack() const;
    int getDefense() const;
    int getSpeed() const;
    
    // Basic setters
    void setName(String newName);
    void setStats(int hp, int atk, int def, int spd);
    
    // Health management
    void takeDamage(int damage);
    void heal(int amount);
    bool isAlive() const;
    
    // Combat actions
    virtual int performAttack();
    virtual int performDefend();
    void resetDefense();
    
    // Defense state
    void setDefending(bool defending);
    bool getIsDefending() const;
    void addTemporaryDefense(int defense);
    int getTotalDefense() const;
    
    // Virtual destructor for inheritance
    virtual ~Entity();
};

#endif