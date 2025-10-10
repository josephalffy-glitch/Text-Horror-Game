#include "Room.h"
#include <iostream>
using namespace std;

// default constructor
Room::Room() {
    name = "unknown";
    description = "unknown";
    items = {};
    visited = false;
}

// parameterized constructor
Room::Room(string name, string description, vector<string> items, bool visited) {
    this->name = name;
    this->description = description;
    this->items = items;
    this->visited = visited;
}

// getters
string Room::getName() {
    return name;
}

string Room::getDescription() {
    return description;
}

vector<string> Room::getItems() {
    return items;
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

void Room::setItems(vector<string> items) {
    this->items = items;
}

void Room::setVisited(bool visited) {
    this->visited = visited;
}