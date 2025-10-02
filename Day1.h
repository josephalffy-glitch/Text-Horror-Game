#ifndef DAY1_H
#define DAY1_H

#include <iostream>
#include <string>
#include <array>
using namespace std;
#include "player.h"

void bedroomMenu() {
    cout << "\nYou are in you bedroom.";
    cout << "\nwhat would you like to do?\n";
    cout << "1. lie down on the bed and rest\n";
    cout << "2. look in the drawer\n";
    cout << "3. check out the trophy\n";
    cout << "4. look at your inventory\n";
    cout << "5. quit the adventure\n";
    cout << "enter your choice: ";
}

// look in mirror
void mirror() {
    cout << "\nYou walk over to the drawer on your desk and take a look.\n";
    cout << "Inside you see a few miscellaneous items: \n >pencils, a boxcutter, bandaids, a snack bar\n";
    cout << "As you're about  to close the drawer something catches your eye.\nIt's a small pocket mirror that you have had for years.\n";
    cout << "\nWould you like to take look?\n";
    cout << "1. yes\n";
    cout << "2. no\n";
    cout << "enter your choice: ";
}

// look at trophy
void trophy(string name) {
    cout << "\nYou see a shiny trophy on the shelf.\n";
    cout << "It has your name engraved: " << name << ".\n";
    cout << "It seems to celebrate your adventures so far!\n";
}

int day1() {
    string name, hair, eyes;
    // greet player and get info
    cout << "welcome adventurer! let's set up your character.\n";
    cout << "what's your name? \n";
    cin >> name;

    cout << "what color is your hair? \n";
    cin >> hair;

    cout << "what color are your eyes? \n";
    cin >> eyes;

    // create player
    array<string, 5> startInv;
    Player player = Player(name, hair, eyes, startInv);
    player.setInventory(startInv);

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
                mirror();
                int choice2;
                cin >> choice2;
                if (choice2 == 1)
                {
                    cout << "\nYou open the mirror and see a familar face.\nYour " << player.getEyes() << " eyes are staring back at you and your ";
                    cout << player.getHair() << " hair is framing your face.";
                    cout << " You decide to hold on to the mirror. It could come in handy.\n";
                    player.addToInventory("mirror");
                }
                if (choice2 == 2)
                {
                    cout << "Before you close the drawer you take the mirror with you.\n";
                    player.addToInventory("mirror");
                }
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