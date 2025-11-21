#include "Room.h"
#include <iostream>
using namespace std;

// default constructor
Room::Room() {
    name = "";
    description = "";
    storageLocations = {};
    visited = false;
	roomEnum = RoomEnum::NULL_ROOM;
}

// parameterized constructor
Room::Room(string name, string description, vector<StorageLocation> storageLocations, bool visited ,RoomEnum roomEnum) {
    this->name = name;
    this->description = description;
    this->storageLocations = storageLocations;
    this->visited = visited;
	this->roomEnum = roomEnum;
}

// getters
string Room::getName() {
    return name;
}

string Room::getDescription() {
    return description;
}

RoomEnum Room::getRoomEnum() {
	return roomEnum;
}

vector<StorageLocation> Room::getStorageLocations() {
    return storageLocations;
}

vector<Item> Room::getItemsInLocation(string locationName) {
    for (const auto& location : storageLocations) {
        if (location.name == locationName) {
            return location.items;
        }
    }
    return vector<Item>();
}

bool Room::isVisited() {
    return visited;
}

// setters
void Room::setName(string name) {
    this->name = name;
}

void Room::setDescription(string description) {
    this->description = description;
}

void Room::setRoomEnum(RoomEnum roomEnum) {
	this->roomEnum = roomEnum;
}

void Room::addStorageLocation(string locationName) {
    StorageLocation newLocation;
    newLocation.name = locationName;
    newLocation.items = {};
    storageLocations.push_back(newLocation);
}

void Room::addItemToLocation(string locationName, Item item) {
    for (auto& location : storageLocations) {
        if (location.name == locationName) {
            location.items.push_back(item);
            return;
        }
    }
}

void Room::removeItemFromLocation(string locationName, Item item) {
    // Find the storage location
    for (auto& location : storageLocations) {
        // If the location matches
        if (location.name == locationName) {
            // Find and remove the item
            for (auto it = location.items.begin(); it != location.items.end(); ++it) {
                if (it->getName() == item.getName()) {
                    location.items.erase(it);
                    return;
                }
            }
        }
    }
}

void Room::setVisited(bool visited) {
    this->visited = visited;
}