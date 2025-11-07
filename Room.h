#ifndef ROOM_H
#define ROOM_H

#include <vector>
#include <string>
#include "Items.h"

using namespace std;

struct StorageLocation {
    string name;
    vector<Item> items;
};

class Room {
    private: 
        string name;
        string description;
        vector<StorageLocation> storageLocations;
        bool visited;

    public:
        // constructors
        Room();
        Room(string name, string description, vector<StorageLocation> storageLocations, bool visited);
        // getters
        string getName();
        string getDescription();
        vector<StorageLocation> getStorageLocations();
        vector<Item> getItemsInLocation(string locationName);
        bool isVisited();
        // setters
        void setName(string name);
        void setDescription(string description);
        void addStorageLocation(string locationName);
        void addItemToLocation(string locationName, Item item);
        void removeItemFromLocation(string locationName, Item item);
        void setVisited(bool visited);

};

#endif // ROOM_H