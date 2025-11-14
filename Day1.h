//thee updated Day1.h 
#ifndef DAY1_H
#define DAY1_H

#include <iostream>
#include <string>
#include <array>
using namespace std;
#include "player.h"
#include "Items.h"
#include "GameEngine.h"
#include "PowerMeterMinigame.h" // include the minigame header

//function to handle the power outage event
void powerOutageEvent()
{
    cout << "\n------------------------------------------------\n";
    cout << "a sudden darkness envelops the kitchen. the lights flicker and die.\n";
    cout << ">> mom: oh no, the power must be out! i hate when that happens.\n";
    cout << "you hear a strange humming sound from the basement door.\n";
    cout << ">> mom: honey, can you go check the auxiliary power unit down in the cellar?\n";
    cout << "it looks like you have to fix the power meter to continue!\n";
    cout << "------------------------------------------------\n\n";

    PowerMeterMinigame powerGame;
    // run the minigame
    bool success = powerGame.run();

    if (success) {
        cout << "\n>> mom: that's my kid! the lights are back on. thanks, sweetie!\n";
    } else {
        cout << "\n>> mom: well, the lights are back on, but that was a bit rough. maybe be more careful next time!\n";
    }
    cout << "\n------------------------------------------------\n";
}

void bedroomMenu() {
    cout << "\nYou are in your bedroom.";
    cout << "\nwhat would you like to do?\n";
    cout << "1. lie down on the bed and rest\n";
    cout << "2. look in the drawer\n";
    cout << "3. check out the trophy\n";
    cout << "4. look at your inventory\n";
    cout << "5. Go to the kitchen\n"; // an option to move to the next scene
    cout << "6. quit the adventure\n"; // quit is now number6
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
    cout << "4. Leave the kitchen\n"; // lets them leave the kitchen
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
    // get the game engine instance
    GameEngine& engine = GameEngine::getInstance();

    // get the player reference
    Player* playerPtr = engine.getPlayer();
    Player& player = *playerPtr;

    // greet player and get info
    string name, hair, eyes;
    cout << "welcome adventurer! let's set up your character.\n";
    cout << "what's your name? \n";
    cin >> name;

    cout << "what color is your hair? \n";
    cin >> hair;

    cout << "what color are your eyes? \n";
    cin >> eyes;

    // create player
    player.setName(name);
    player.setHair(hair);
    player.setEyes(eyes);

    int choice;
    // gotta set this to true so the game actually starts
    bool playing = true;

    while (playing) {
        // show the player their options
        bedroomMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "\nYou walk over to your bed and lie down. It's nice to rest for a bit...\n";
                break;
            case 2:
                //engine.lookInStorageLocation("Bedroom", "Drawer");
/*            
                mirror();
                int choice2;
                cin >> choice2;
                
                if (choice2 == 1)
                {
                    cout << "\nYou open the mirror and see a familar face.\nYour " << player.getEyes() << " eyes are staring back at you and your ";
                    cout << player.getHair() << " hair is framing your face.";
                    cout << " You decide to hold on to the mirror. It could come in handy.\n";
                    Item mirror = Item("mirror", ItemType::MISC, 0);
                    player.addToInventory(mirror);
                }
                if (choice2 == 2)
                {
                    cout << "Before you close the drawer you take the mirror with you.\n";
                    Item mirror = Item("mirror", ItemType::MISC, 0);
                    player.addToInventory(mirror);
                }
*/                    
                break;
            case 3:
                trophy(name);
                break;
            case 4:
                player.checkInventory();
                break;
            // for moving to the kitchen
            case 5:
                cout << ">> Mom: Sweetie! Breakfeast is ready!\n";
                cout << "She has always called you that. It's a little embarrassing, but mostly endearing\n";
                cout << ">> " << name << ": I'm coming!\n";
                playing = false; // this is how we get out of the bedroom loop
                break;
            // for quitting the game
            case 6:
                cout << "Thanks for playing, " << player.getName() << "! See you next time.\n";
                playing = false;
                return 0; // exit the whole program
            default:
                cout << "Hmm, that doesn't seem right. Try picking a number from the list.\n";
        }
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
                { // brackets to keep these variables from interfering
                    int choice2;
                    kitchenCab();
                    bool inCab = true;
                    while(inCab)
                    {
                        cin >> choice2;
                        // a switch is just a bit cleaner here
                        switch(choice2) {
                            case 1: {
                                cout << "\nNot sure what I would use cornstarch for right now...let's put it back.\n";
                                inCab = false; break;
                            }
                            case 2: {
                                cout << "\nPerfect!\n";
                                Item syrup = Item("syrup", ItemType::FOOD, 1);
                                player.addToInventory(syrup);
                                inCab = false; break;
                            }
                            case 3: {
                                cout << "\nNot sure if I want salty waffles...let's put it back.\n";
                                inCab = false; break;
                            }
                            case 4: {
                                cout << "\nI really shouldn't...I'm sure mom won't mind.\n";
                                Item cookie = Item("cookie", ItemType::FOOD, 2);
                                player.addToInventory(cookie);
                                inCab = false; break;
                            }
                            case 5: {
                                player.checkInventory();
                                kitchenCab(); // show menu again so they can pick something else
                                break;
                            }
                            default: {
                                cout << "\nInvalid choice\n";
                                inCab = false; break;
                            }
                        }
                    }
                }
                break;
            // for leaving the kitchen
            case 4:
                cout << "\nYou finish breakfast and get ready for the day.\n";
                inKitchen = false; // this ends the kitchen loop
                break;
        }
    }
    
    // call the minigame event here before day 1 ends
    powerOutageEvent();

    // moved this outside the loop so the game doesn't end after one action
    cout << "\nDay 1 has concluded. To be continued...\n";
    return 0;
}

#endif // DAY1_H
