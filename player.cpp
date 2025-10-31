#include <iostream>
#include <array>
#include <string>
using namespace std;
#include "player.h"
#include "Items.h"

// default constructor
Player::Player()
{
	name = "unknown";
	hair = "unknown";
	eyes = "unknown";
	Item nullItem = Item();
	inventory.fill(nullItem); // Initialize inventory with empty Items
}

// parameterized constructor for loading saved player
Player::Player(string name, string hair, string eyes, array<Item, 5> inv)
{
	this->name = name;
	this->hair = hair;
	this->eyes = eyes;
	this->inventory = inv;
}

// setters ......................................
void Player::setName(string name)
{
	this->name = name;
}
void Player::setHair(string hair)
{
	this->hair = hair;
}
void Player::setEyes(string eyes)
{
	this->eyes = eyes;
}
void Player::setInventory(array<Item, 5> inventory)
{
	this->inventory = inventory;
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
array<Item, 5> Player::getInventory()
{
	return inventory;
}

// other member functions ........................
void Player::updateHealth(Item item)
{

}
void Player::checkHealth()
{

}
void Player::checkInventory()
{
	cout << "\nInventory:\n";

	for(int i = 0; i < inventory.size(); i++)
	{
		cout << i + 1 << ": " << inventory[i].getName() << "\n";
	}
}
void Player::addToInventory(Item item)
{
	bool inventoryFull = true;

	//check if inventory is full
	for(int i = 0; i < inventory.size(); i++)
	{
		if(inventory[i].getType() == ItemType::NULLITEM)
		{	
			inventoryFull = false;
			break;
		}
	}

	//if full, notify user and return
	if(inventoryFull) {
		cout << "\nInventory full. Cannot add " << item.getName() << ".\n";
		return;
	} else {
		//add item to first empty slot
		for(int i = 0; i < inventory.size(); i++) 
		{
			if(inventory[i].getType() == ItemType::NULLITEM) 
			{
				inventory[i] = item;
				cout << "\n"<< item.getName() << " added to inventory.\n";
				return;
			}
		}
	}
}
