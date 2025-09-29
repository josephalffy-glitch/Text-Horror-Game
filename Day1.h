#ifndef DAY1_H
#define DAY1_H

#include <iostream>
#include <string>
#include <array>
using namespace std;
#include "player.h"

void bedroomMenu() {
    cout << "\nwhat would you like to do next?\n";
    cout << "1. lie down on the bed and rest\n";
    cout << "2. look in the mirror\n";
    cout << "3. check out the trophy\n";
    cout << "4. look at your inventory\n";
    cout << "5. quit the adventure\n";
    cout << "enter your choice: ";
}

// look in mirror
void mirror(string name, string hair, string eyes) {
    cout << "\nYou walk over to the mirror and take a look.\n";
    cout << "Looking back at you is " << name << ".\n";
    cout << "Your hair is " << hair << " and your eyes are " << eyes << ".\n";
}

// look at trophy
void trophy(string name) {
    cout << "\nYou see a shiny trophy on the shelf.\n";
    cout << "It has your name engraved: " << name << ".\n";
    cout << "It seems to celebrate your adventures so far!\n";
}

int day1() {
    string name, hair, eyes, item;
    // greet player and get info
    cout << "welcome adventurer! let's set up your character.\n";
    cout << "what's your name? \n";
    cin >> name;

    cout << "what color is your hair? \n";
    cin >> hair;

    cout << "what color are your eyes? \n";
    cin >> eyes;

    cout << "what item do you start with in your inventory? \n";
    cin >> item;

    array<string, 5> startInv = {item, "", "", "", ""};

    // create player
    Player player = Player(name, hair, eyes, startInv);

    int choice;
    bool playing = true;

    while (playing) {
        bedroomMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "\nYou walk over to your bed and lie down. It's nice to rest for a bit...\n";
                break;
            case 2:
                mirror(name, hair, eyes);
                break;
            case 3:
                trophy(name);
                break;
            case 4:
                player.checkInventory();
                break;
            case 5:
                cout << "Thanks for playing, " << player.getName() << "! See you next time.\n";
                playing = false;
                break;
            default:
                cout << "Hmm, that doesn't seem right. Try picking a number from the list.\n";
        }
    }
    return 0;
}

#endif // DAY1_H