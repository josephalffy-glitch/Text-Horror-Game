#include <iostream>
#include <string>
#include <vector>
#include "player.h"
#include "Room.h"
#include "GameEngine.h"

using namespace std;

void GameEngine::printArt(const string& image)
{
    ifstream file("ASCII.txt"); //open file

    string line;
    bool correctSection;

    while (getline(file, line))
    {
        if (line == "#" + image)
        {
            correctSection = true;
            continue;
        }
        if (correctSection && line.rfind("#", 0) == 0)
        {
            break;
        }
        if(correctSection)
        {
            cout << line << endl;
        }
    }

    file.close(); // close file
}
// Needs implementation of GameEngine methods
