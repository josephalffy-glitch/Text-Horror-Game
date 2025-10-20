#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
// GameEngine serves as the main controller for the game flow
// It manages the game state, player interactions, and transitions between different game scenes.

#include <iostream>
#include <string>
#include <vector>
#include "player.h"
#include "Room.h"

using namespace std;

class GameEngine {
    private:
        Player* player; // Pointer to the player object for reference and manipulation
        vector<Room>* rooms; // Pointer to a vector of Room objects

        // Private constructor
        GameEngine();
        ~GameEngine();

    public:
        // Singleton instance
        static GameEngine& getInstance();

        // Game Initialization
        void startNewGame();
        void loadGame(const string& saveFile);
        void saveGame(const string& saveFile);

        // Game Loop
        Player* getPlayer();
        bool checkForItemInLocation(const string& roomName, const string& locationName, const string& item);
        void rmItemFromRoom(const string& roomName, const string& locationName, const string& item);
        void addItemToRoom(const string& roomName, const string& locationName, const string& item);

};


#endif // GAME_ENGINE_H
