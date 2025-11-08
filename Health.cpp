#include "Health.h"

Health::Health(int maxHealth) 
{
    this->maxHealth = maxHealth;
    healthUnits.resize(maxHealth, 1); // start full health
}

void Health::takeDamage(int amount)
 {
    cout << "You took " << amount << " damage!\n";
    for (int i = 0; i < amount && getCurrentHealth() > 0; ++i)
     {
        for (int j = maxHealth - 1; j >= 0; --j) 
        {
            if (healthUnits[j] == 1) 
            {
                healthUnits[j] = 0;
                break;
            }
        }
    }
    display();
}

void Health::heal(int amount) 
{
    cout << "Healing " << amount << " points!\n";
    for (int i = 0; i < amount; ++i)
     {
        for (int j = 0; j < maxHealth; ++j)
         {
            if (healthUnits[j] == 0) 
            {
                healthUnits[j] = 1;
                break;
            }
        }
    }
    display();
}

void Health::display() const 
{
    cout << "Health: [ ";
    for (int unit : healthUnits) 
    {
        cout << (unit == 1 ? "♥ " : "· "); // ♥ = full, · = empty
    }
    cout << "]  (" << getCurrentHealth() << "/" << maxHealth << ")\n";
}

bool Health::isAlive() const 
{
    return getCurrentHealth() > 0;
}

int Health::getCurrentHealth() const 
{
    int total = 0;
    for (int unit : healthUnits)
        if (unit == 1)
            total++;
    return total;
}

int Health::getMaxHealth() const 
{
    return maxHealth;
}