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
Player::Player(string name, string hair, string eyes, array<string, 5> inventory)
{
	this->name = name;
	this->hair = hair;
	this->eyes = eyes;
	this->inventory = inventory;
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
void Player::setInventory(array<string, 5> inventory)
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
	cout << "\nInventory:\n";

	for(int i = 0; i < inventory.size(); i++)
	{
		cout << i + 1 << ": " << inventory[i] << "\n";
	}
}
void Player::addToInventory(string item)
{
	bool inventoryFull = true;

	//check if inventory is full
	for(int i = 0; i < inventory.size(); i++)
	{
		if(inventory[i] == "")
		{	
			inventoryFull = false;
			break;
		}
	}

	//if full, notify user and return
	if(inventoryFull) {
		cout << "\nInventory full. Cannot add " << item << ".\n";
		return;
	} else {
		//add item to first empty slot
		for(int i = 0; i < inventory.size(); i++) 
		{
			if(inventory[i] == "") 
			{
				inventory[i] = item;
				cout << "\n"<< item << " added to inventory.\n";
				return;
			}
		}
	}
}
