#ifndef HEALTH_H
#define HEALTH_H

#include <iostream>
#include <vector>
using namespace std;

class Health 
{
private:
    vector<int> healthUnits; // 1 = full, 0 = empty
    int maxHealth;

public:
    // Constructor: initialize health bar
    Health(int maxHealth = 10);

    // Methods
    void takeDamage(int amount);
    void heal(int amount);
    void display() const;
    bool isAlive() const;
    int getCurrentHealth() const;
    int getMaxHealth() const;
};

#endif