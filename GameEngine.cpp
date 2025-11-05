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

// Needs implementation of GameEngine methods
