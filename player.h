#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <array>
#include<vector>
#include "Items.h"
using namespace std;

class Player
{
    private:
        // player attributes
        string name, hair, eyes;
        //inventory
        array<Item, 5> inventory;
        bool fullInventory;
        //health
        vector<int> healthUnits; // 1 = full, 0 = empty
        int maxHealth;

    public:
        // default constructor
        Player();

        // parameterized constructor for loading saved player
        Player(string name, string hair, string eyes, array<Item, 5> inv);

        // setters
        void setName(string name);
        void setHair(string hair);
        void setEyes(string eyes);
        void setInventory(array<Item, 5> inv);

        // getters
        string getName();
        string getHair();
        string getEyes();
        array<Item, 5> getInventory();
        bool fullInv();

        void checkInventory();
        bool addToInventory(Item item);
        bool removeFromInventory(const string& itemName);
        void clearInventory();

        // Methods
        void takeDamage(int amount);
        void heal(int amount);
        void display() const;
        bool isAlive() const;
        int getCurrentHealth() const;
        int getMaxHealth() const;

};
#endif