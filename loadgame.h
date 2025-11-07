#ifndef LOADGAME_H
#define LOADGAME_H

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include "player.h"

namespace fs = std::filesystem;

class LoadGame 
{
public:
    void loadSave();
};

#endif