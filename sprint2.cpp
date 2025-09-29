#include <iostream>
#include <string>
using namespace std;

class Player {
private:
    string name;
    string hair;
    string eyes;
    string inventory;
public:
    Player() {
        name = "unknown";
        hair = "unknown";
        eyes = "unknown";
        inventory = "nothing";
    }

    Player(string n, string h, string e, string inv) {
        name = n;
        hair = h;
        eyes = e;
        inventory = inv;
    }
    // setters
    void setName(string n) { name = n; }
    void setHair(string h) { hair = h; }
    void setEyes(string e) { eyes = e; }
    void setInventory(string inv) { inventory = inv; }
    // getters
    string getName() { return name; }
    string getHair() { return hair; }
    string getEyes() { return eyes; }
    string getInventory() { return inventory; }

    // look in mirror
    void mirror() {
        cout << "\nYou walk over to the mirror and take a look.\n";
        cout << "Looking back at you is " << name << ".\n";
        cout << "Your hair is " << hair << " and your eyes are " << eyes << ".\n";
    }
    // look at trophy
    void trophy() {
        cout << "\nYou see a shiny trophy on the shelf.\n";
        cout << "It has your name engraved: " << name << ".\n";
        cout << "It seems to celebrate your adventures so far!\n";
    }
};
// friendly menu
void menu() {
    cout << "\nwhat would you like to do next?\n";
    cout << "1. lie down on the bed and rest\n";
    cout << "2. look in the mirror\n";
    cout << "3. check out the trophy\n";
    cout << "4. look at your inventory\n";
    cout << "5. quit the adventure\n";
    cout << "enter your choice: ";
}
int main() {
    string name, hair, eyes, inv;
    // greet player and get info
    cout << "welcome adventurer! let's set up your character.\n";
    cout << "what's your name? ";
    getline(cin, name);

    cout << "what color is your hair? ";
    getline(cin, hair);

    cout << "what color are your eyes? ";
    getline(cin, eyes);

    cout << "what item do you start with in your inventory? ";
    getline(cin, inv);

    // create player
    Player player(name, hair, eyes, inv);

    int choice;
    bool playing = true;

    while (playing) {
        menu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "\nYou walk over to your bed and lie down. It's nice to rest for a bit...\n";
                break;
            case 2:
                player.mirror();
                break;
            case 3:
                player.trophy();
                break;
            case 4:
                cout << "\nYou check your inventory. You currently have: " << player.getInventory() << "\n";
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