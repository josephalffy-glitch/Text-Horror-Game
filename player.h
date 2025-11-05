#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <array>
using namespace std;

class Player
{
    private:
        // player attributes
        string name, hair, eyes;
        array<string, 5> inventory;

    public:
        // default constructor
        Player();

        // parameterized constructor for loading saved player
        Player(string name, string hair, string eyes, array<string, 5> inv);

        // setters
        void setName(string name);
        void setHair(string hair);
        void setEyes(string eyes);
        void setInventory(array<string, 5> inv);

        // getters
        string getName();
        string getHair();
        string getEyes();
        array<string, 5> getInventory();

        // other member functions
        void updateHealth(string food);
        void checkHealth();
        void checkInventory();
        void addToInventory(string item);
        void clearInventory();
};
#endif