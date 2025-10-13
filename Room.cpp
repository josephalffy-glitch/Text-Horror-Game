#include "Room.h"
#include <iostream>
using namespace std;

// default constructor
Room::Room() {
    name = "unknown";
    description = "unknown";
    storageLocations = {};
    visited = false;
}

// parameterized constructor
Room::Room(string name, string description, vector<StorageLocation> storageLocations, bool visited) {
    this->name = name;
    this->description = description;
    this->storageLocations = storageLocations;
    this->visited = visited;
}

// getters
string Room::getName() {
    return name;
}

string Room::getDescription() {
    return description;
}

vector<StorageLocation> Room::getStorageLocations() {
    return storageLocations;
}

vector<string> Room::getItemsInLocation(string locationName) {
    for (const auto& location : storageLocations) {
        if (location.name == locationName) {
            return location.items;
        }
    }
    return vector<string>();
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

void Room::addStorageLocation(string locationName) {
    StorageLocation newLocation;
    newLocation.name = locationName;
    newLocation.items = {};
    storageLocations.push_back(newLocation);
}

void Room::addItemToLocation(string locationName, string item) {
    for (auto& location : storageLocations) {
        if (location.name == locationName) {
            location.items.push_back(item);
            return;
        }
    }
}

void Room::removeItemFromLocation(string locationName, string item) {
    for (auto& location : storageLocations) {
        if (location.name == locationName) {
            for (auto it = location.items.begin(); it != location.items.end(); ++it) {
                if (*it == item) {
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