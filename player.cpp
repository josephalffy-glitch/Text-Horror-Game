#include <iostream>
#include <array>
#include <string>
using namespace std;
#include "player.h"

// default constructor
Player::Player()
{
	name = "unknown";
	hair = "unknown";
	eyes = "unknown";
	inventory.fill("");
}
// parameterized constructor for loading saved player
Player::Player(string n, string h, string e, array<string, 5> inv)
{
	name = n;
	hair = h;
	eyes = e;
	inventory = inv;
}

// setters ......................................
void Player::setName(string n)
{
	name = n;
}
void Player::setHair(string h)
{
	hair = h;
}
void Player::setEyes(string e)
{
	eyes = e;
}
void Player::setInventory(array<string, 5> inv)
{
	inventory = inv;
}

// getters ......................................
string Player::getName()
{
	return name;
}
string Player::getHair()
{
	return hair;
}
string Player::getEyes()
{
	return eyes;
}
array<string, 5> Player::getInventory()
{
	return inventory;
}

// other member functions ........................
void Player::updateHealth(string food)
{

}
void Player::checkHealth()
{

}
void Player::checkInventory()
{
	cout << "Inventory:\n";

	for(int i = 0; i < inventory.size(); i++)
	{
		cout << i + 1 << ": " << inventory[i] << "\n";
	}
}
void Player::addToInventory(array<string, 5>& Inventory, string item)
{
	bool inventoryFull = true;

	//check if inventory is full
	for(int i = 0; i < Inventory.size(); i++)
	{
		if(Inventory[i] == "")
		{
			inventoryFull = false;
			break;
		}
	}

	//if full, notify user and return
	if(inventoryFull) {
		cout << "Inventory full. Cannot add " << item << ".\n";
		return;
	} else {
		//add item to first empty slot
		for(int i = 0; i < Inventory.size(); i++) {
			if(Inventory[i] == "") {
				Inventory[i] = item;
				cout << item << " added to inventory.\n";
				return;
			}
		}
	}
}
