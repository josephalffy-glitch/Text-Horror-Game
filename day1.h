#ifndef DAY1_H
#define DAY1_H
#include <iostream>
#include <string>
#include <array>
using namespace std;
#include "player.h"
void bedroomMenu() {
    cout << "\nYou are in your bedroom.";
    cout << "\nwhat would you like to do?\n";
    cout << "1. lie down on the bed and rest\n";
    cout << "2. look in the drawer\n";
    cout << "3. check out the trophy\n";
    cout << "4. look at your inventory\n";
    cout << "5. quit the adventure\n";
    cout << "enter your choice: ";
}

// new menu function
void showBedroomChoices(bool hasLookedInDrawer, bool hasCheckedTrophy) {
    cout << "\nYou are in your bedroom.";
    cout << "\nwhat would you like to do?\n";
    cout << "1. lie down on the bed and rest\n";

    // show if not done
    if (!hasLookedInDrawer) {
        cout << "2. look in the drawer\n";
    }

    // show if not done
    if (!hasCheckedTrophy) {
        cout << "3. check out the trophy\n";
    }

    cout << "4. look at your inventory\n";
    cout << "5. quit the adventure\n";
    cout << "enter your choice: ";
}

// look in mirror
void mirror() {
    cout << "\nYou walk over to the drawer on your desk and take a look.\n";
    cout << "Inside you see a few miscellaneous items: \n >>>> pencils, a boxcutter, bandaids, a snack bar\n";
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
void kitchenMenu()
{
    cout << "\n1. eat your food\n";
    cout << "2. ask your mom about her hand\n";
    cout << "3. get more syrup for your waffles\n";
    cout << "Enter your choice: ";
}
void kitchenCab()
{
    cout << "\n>> Mom: Don't tell me you want even more syrup! What am I going to do with you?\n";
    cout << "You make your way over to one of the kitchen cabinets where you see a plethora of things.\n";
    cout << ">>>> cornstarch, syrup, salt, box of cookies\n";
    cout << "\n1. Grab the cornstarch\n";
    cout << "2. Grab the syrup\n";
    cout << "3. Grab the salt\n";
    cout << "4. Grab a cookie\n";
    cout << "5. look at your inventory\n";
    cout << "enter your choice: ";
}
int day1()
{
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

    // memory flags
    bool hasLookedInDrawer = false;
    bool hasCheckedTrophy = false;

    int choice;
    bool playing = false; //true;
    while (playing) {
        showBedroomChoices(hasLookedInDrawer, hasCheckedTrophy);
        cin >> choice;
        cin.ignore();
        switch (choice) {
            case 1:
                cout << "\nYou walk over to your bed and lie down. It's nice to rest for a bit...\n";
                break;
            case 2:
                if (!hasLookedInDrawer) {
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
                    // update flag
                    hasLookedInDrawer = true;
                } else {
                    cout << "hmmm, pick something from the list.\n";
                }
                break;
            case 3:
                if (!hasCheckedTrophy) {
                    trophy(name);
                    // update flag
                    hasCheckedTrophy = true;
                } else {
                    cout << "hmmm, pick something from the list.\n";
                }
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
        cout << ">> Mom: Sweetie! Breakfeast is ready!\n";
        cout << "She has always called you that. It's a little embarrassin, but mostly endearing\n";
        cout << ">> " << name << ": I'm coming!";
    }
    bool inKitchen = true;
    cout << "\nYou run to the kitchen, ready for a fresh meal.\n";
    cout << ">> Mom: Rise and shine! I made your favorite, blueberry waffles!\n";
    cout << "Your mom turns around with a plate of waffles drenched in syrup.\n";
    cout << "As your mom sets the plate on the dining table you take a notice of her right hand.\nIt's bandaged up.\n";
    while(inKitchen)
    {
        kitchenMenu();
        cin >> choice;
        switch (choice)
        {
            case 1:
                cout << "\nYou take a bite of your food...absolute bliss.\n";
                break;
            case 2:
                cout << "\n>> " << name << ": What happened to your hand?\n";
                cout << ">> Mom: Oh! I was just cleaning the basement and touched some broken glass. No big deal sweetie.\n";
                break;
            case 3:
                int choice2;
                kitchenCab();
                bool inCab = true;
                while(inCab)
                {
                    cin >> choice2;
                    if (choice2 == 1)
                    {
                        cout << "\nNot sure what I would use cornstarch for right now...let's put it back.\n";
                        break;
                    }
                    if (choice2 == 2)
                    {
                        cout << "\nPerfect!\n";
                        player.addToInventory("syrup");
                        break;
                    }
                    if (choice2 == 3)
                    {
                        cout << "\nNot sure if I want salty waffles...let's put it back.\n";
                        break;
                    }
                    if (choice2 == 4)
                    {
                        cout << "\nI really shouldn't...I'm sure mom won't mind.\n";
                        player.addToInventory("cookie");
                        break;
                    }
                    if (choice2 == 5)
                    {
                        player.checkInventory();
                        break;
                    }
                    else
                    {
                        cout << "\nInvalid choice";
                        break;
                    }
                    }
                }

        }
        return 0;
    }
#endif // DAY1_H