#include <iostream>
#include <string>
#include <vector>
#include "player.h"
#include "Room.h"
#include "GameEngine.h"

using namespace std;

#include <fstream>
#include <sstream> // for stringstream
#include <string>
using namespace std;

string GameEngine::printArt(const string& image)
{
    ifstream file("ASCII.txt");
    if (!file.is_open()) return "ASCII art file not found.";

    string line;
    bool correctSection = false;
    stringstream art; // store all lines

    while (getline(file, line))
    {
        if (line == "#" + image)
        {
            correctSection = true;
            continue;
        }
        if (correctSection && line.rfind("#", 0) == 0) // next section
        {
            break;
        }
        if (correctSection)
        {
            art << line << "\n"; // append line to stringstream
        }
    }

    file.close();
    return art.str();
}
/*void loadSave() {
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
    }*/

// Needs implementation of GameEngine methods
