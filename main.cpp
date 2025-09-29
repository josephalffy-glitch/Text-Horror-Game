
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
using namespace std;
namespace fs = std::filesystem;

#include "player.h"
#include "Day1.h"

void loadSave();
void mainMenu();

int main()
{
    int choice = 0;

    std::cout << "Welcome to the Adventure Game!" << endl;

    // Display main menu and get user choice
    while (choice != 3) {
        mainMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Starting a new game..." << endl;
                day1(); // Call the function to start Day 1
                break;
            case 2:
                loadSave();
                break;
            case 3:
                cout << "Quitting the game. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}

/*
    Player Adam;
	int choice;
	vector<string> myInventory;

	cout << "You found a mirror. . .\n> Add to Inventory? (1 = yes or 2 = no)\n";
	cin >> choice;

	if (choice == 1)
	Adam.UpdateInventory(myInventory, "mirror");


	cout << "open inventory?\n";
	cin >> choice;
	if (choice = 1)
	Adam.CheckInventory(myInventory);
	Adam.CheckInventory(myInventory);
*/

// main menu
void mainMenu() {
    cout << "Main Menu:\n";
    cout << "1. Start New Game\n";
    cout << "2. Load Saved Game\n";
    cout << "3. Quit\n";
    cout << "Enter your choice: ";
}

// loadSave() will check for available save files.
// If none are found, it will notify the user and return
// to the main menu.
// If save files are found, it will display a list of them
// and prompt the user to select one.
void loadSave() {
    cout << "loadSave() called" << endl;

    vector<string> saveFiles; // Array to hold save file names
    // check if there are any save files within SaveFiles directory
    for (const auto& entry : fs::directory_iterator("SaveFiles")) {
        saveFiles.push_back(entry.path().filename().string());
    }

    // If no save files are found, notify the user and return to main menu
    if (saveFiles.empty()) {
        cout << "No save files found." << endl;
        return; // Return to main menu
    } else { // If save files are found, display them and prompt user to select one
        cout << "Available save files:" << endl;

        for (int i = 0; i < saveFiles.size(); ++i) {
            cout << i + 1 << ". " << saveFiles[i] << endl;
        }

        cout << "Select a save file by entering its number: ";
        int choice;
        cin >> choice;

        if (choice < 1 || choice > saveFiles.size()) {
            cout << "Invalid choice. Returning to main menu." << endl;
            return; // Return to main menu
        }

        string selectedFile = saveFiles[choice - 1];
        cout << "You selected: " << selectedFile << endl;

        // Here you would add code to actually load the selected save file
    }


    
}
