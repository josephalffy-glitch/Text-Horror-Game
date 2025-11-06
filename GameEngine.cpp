#include <iostream>
#include <string>
#include <vector>
#include "player.h"
#include "Room.h"
#include "GameEngine.h"
#include "Items.h"

using namespace std;

void storageLocationMenu()
{
    //cout << "\n1. Inspect an item\n"; // Could add later
    cout << "\n1. Pick up an item\n";
    cout << "2. Drop an item\n";
    cout << "3. Leave\n";
    cout << "Enter your choice: ";
}

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

bool GameEngine::pickUpItem(const string& roomName, const string& locationName, const string itemName){
    //check if player's inventory is full
    if (player->fullInv()) {
        cout << "Inventory full. Cannot pick up " << itemName << ".\n";
        return false;
    }
    
    for (Room& room : *rooms) {
        if (room.getName() == roomName) {
            vector<Item> items = room.getItemsInLocation(locationName);
            for (const Item& item : items) {
                if (item.getName() == itemName) {
                    player->addToInventory(item);
                    room.removeItemFromLocation(locationName, item);
                    cout << "Picked up " << itemName << " from the " << locationName << " in the " << roomName << ".\n";
                    return true;
                }
            }
        }
    }
    cout << "Item " << itemName << " not found in " << locationName << " of " << roomName << ".\n";
    return false;
}

bool GameEngine::dropItem(const string& roomName, const string& locationName, const string itemName){
    // First check if player has the item and get a copy
    array<Item, 5> inventory = player->getInventory();
    Item itemToDrop;
    bool itemFound = false;
    
    // Find the item in player's inventory
    for (const Item& item : inventory) {
        if (item.getName() == itemName) {
            itemToDrop = item;
            itemFound = true;
            break;
        }
    }
    
    // If item not found, notify and return
    if (!itemFound) {
        cout << "You do not have " << itemName << " in your inventory.\n";
        return false;
    }
    
    // Find the room and add item to location
    for (Room& room : *rooms) {
        if (room.getName() == roomName) {
            room.addItemToLocation(locationName, itemToDrop);
            // Remove item from player's inventory using player method
            if (player->removeFromInventory(itemName)) {
                cout << "Placed " << itemName << " into the " << locationName << " in the " << roomName << ".\n";
            }
            return true;
        }
    }
    cout << "Room " << roomName << " not found.\n";
    return false;
}

// lookInStorageLocation opens up a storage location and lists items
// and then asks the player if they want to inspect, pick anything up, drop
// anything, or leave.
void GameEngine::lookInStorageLocation(const string& roomName, const string& locationName){
    for (Room& room : *rooms) {
        if (room.getName() == roomName) {
            vector<Item> items = room.getItemsInLocation(locationName);
            cout << "You look in the " << locationName << " and see the following items:\n";
            for (const Item& item : items) {
                cout << "- " << item.getName() << "\n";
            }
        }
    }

    storageLocationMenu();
    int choice;
    cin >> choice;
    bool inMenu = true;
    bool firstLoop = true;
    while (inMenu) {
        // On first loop, we have already displayed items
        if (!firstLoop) {
            for (Room& room : *rooms) {
                if (room.getName() == roomName) {
                    vector<Item> items = room.getItemsInLocation(locationName);
                    cout << "\nIn the " << locationName << " you now see the following items:\n";
                    for (const Item& item : items) {
                        cout << "- " << item.getName() << "\n";
                    }
                }
            }
            storageLocationMenu();
            cin >> choice;
        }
        
        switch (choice) {
            case 1: {
                // Handle picking up an item
                string itemName;
                cout << "Enter the name of the item to pick up or 'back' to return: ";
                cin >> itemName;
                while (itemName != "back") {
                    bool success = pickUpItem(roomName, locationName, itemName);
                    if (!success) {
                        cout << "Failed to pick up item: " << itemName << "\n";
                    }
                    cout << "Enter the name of another item to pick up or 'back' to return: ";
                    cin >> itemName;
                }

                break;
            }
            case 2: {
                // Handle dropping an item
                string itemName;
                cout << "Enter the name of the item to drop or 'back' to return: ";
                cin >> itemName;
                while (itemName != "back") {
                    bool success = dropItem(roomName, locationName, itemName);
                    if (!success) {
                        cout << "Failed to drop item: " << itemName << "\n";
                    }
                    cout << "Enter the name of another item to drop or 'back' to return: ";
                    cin >> itemName;
                }
                break;
            }
            case 3:
                // Handle leaving
                inMenu = false;
                break;
            default:
                cout << "Invalid choice.\n";
        }

        firstLoop = false;
    }
}

