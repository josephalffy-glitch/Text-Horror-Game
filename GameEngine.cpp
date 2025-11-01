#include <iostream>
#include <string>
#include <vector>
#include "player.h"
#include "Room.h"
#include "GameEngine.h"
#include "Items.h"

using namespace std;

GameEngine::GameEngine()
{
    // Initialize player and rooms
    player = new Player();
    rooms = new vector<Room>();

    // Initialize game items
    // (Bedroom items)
    Item* mirror = new Item("mirror", ItemType::MISC, 0);
    Item* pencil = new Item("pencil", ItemType::MISC, 0);
    Item* bandaids = new Item("bandage", ItemType::BANDAGE, 5);
    Item* boxcutter = new Item("boxcutter", ItemType::MISC, 0);
    Item* snackBar = new Item("snack bar", ItemType::FOOD, 3);
    // (Kitchen items)
    Item* cornstarch = new Item("cornstarch", ItemType::MISC, 0);
    Item* syrup = new Item("syrup", ItemType::FOOD, 1);
    Item* salt = new Item("salt", ItemType::MISC, 0);
    Item* cookie = new Item("cookie", ItemType::FOOD, 2);

    // Create rooms and add storage locations and items
    // (Bedroom)
    Room bedroom = Room(); // create bedroom
    bedroom.setName("Bedroom");
    bedroom.setDescription("Your cozy bedroom filled with familiar items.");
    bedroom.addStorageLocation("Drawer");
    bedroom.addItemToLocation("Drawer", *mirror);
    bedroom.addItemToLocation("Drawer", *pencil);
    bedroom.addItemToLocation("Drawer", *bandaids);
    bedroom.addItemToLocation("Drawer", *boxcutter);
    bedroom.addItemToLocation("Drawer", *snackBar);
    rooms->push_back(bedroom); // add bedroom to rooms vector
    // (Kitchen)
    Room kitchen = Room(); // create kitchen
    kitchen.setName("Kitchen");
    kitchen.setDescription("The heart of the home, bustling with activity.");
    kitchen.addStorageLocation("Cabinet");
    kitchen.addItemToLocation("Cabinet", *cornstarch);
    kitchen.addItemToLocation("Cabinet", *syrup);
    kitchen.addItemToLocation("Cabinet", *salt);
    kitchen.addItemToLocation("Cabinet", *cookie);
    rooms->push_back(kitchen); // add kitchen to rooms vector
}

// Singleton instance
GameEngine& GameEngine::getInstance()
{
    static GameEngine instance;
    return instance;
}

// Game Loop
Player* GameEngine::getPlayer()
{
    return player;
}

void GameEngine::printArt(const string& image)
{
    ifstream file("ASCII.txt"); //open file

    string line;
    bool correctSection;

    while (getline(file, line))
    {
        if (line == "#" + image)
        {
            correctSection = true;
            continue;
        }
        if (correctSection && line.rfind("#", 0) == 0)
        {
            break;
        }
        if(correctSection)
        {
            cout << line << endl;
        }
    }

    file.close(); // close file
}
