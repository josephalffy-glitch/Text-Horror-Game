#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include "player.h"

void Player::UpdateHealth(string food)
{

}
void Player::CheckHealth()
{

}
void Player::CheckInventory(vector<string>& Inventory)
{

	for(int i = 0; i < Inventory.size(); i++)
	{
		cout << i + 1 << ": " << Inventory[i] << "\n";
	}
}
void Player::UpdateInventory(vector<string>& Inventory, string item)
{
	Inventory.push_back(item);
	cout << "Item Stored.\n";
}
