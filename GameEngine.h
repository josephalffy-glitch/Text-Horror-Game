#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
#include <fstream>
#include <sstream>
// GameEngine serves as the main controller for the game flow
// It manages the game state, player interactions, and transitions between different game scenes.

#include <iostream>
#include <string>
#include <vector>
#include "player.h"
#include "Room.h"
#include "Items.h"
#include "extern/imgui/imgui.h"
#include "extern/imgui/backends/imgui_impl_glfw.h"
#include "extern/imgui/backends/imgui_impl_opengl3.h"
#include "glfw/include/GLFW/glfw3.h" 

using namespace std;

class GameEngine {
    private:
        Player* player; // Pointer to the player object for reference and manipulation
        vector<Room>* rooms; // Pointer to a vector of Room objects
		Room* currentRoom; // Pointer to the current room the player is in
		vector<Room>* visitedRooms; // Pointer to a vector of visited rooms
        bool justLoaded; // flag set when a load operation completed

        // Private constructor
        GameEngine();
        //~GameEngine();

    public:
        // Singleton instance
        static GameEngine& getInstance();

        // Game Initialization
        void startNewGame();
        void loadGame(const string& saveFile);
        void saveGame(const string& saveFile);

        //ASCII Generation
        string printArt(const string& image);

		// Getters
		Player* getPlayer();
		Room* getCurrentRoom();
		vector<Room>* getVisitedRooms();

        // Game Loop
        void useItem(const string& itemName, std::string& message_log);
        bool checkForItemInLocation(const string& roomName, const string& locationName, const string& item);
        bool pickUpItem(const string& roomName, const string& locationName, const string itemName);
        bool dropItem(const string& roomName, const string& locationName, const string itemName);
        void lookInStorageLocation(const string& roomName, const string& locationName, bool& showStorageWindow, std::string& message_log);
        void renderStorageLocationWindow(const string& roomName, const string& locationName, bool& showStorageWindow, std::string& message_log);
		void visitRoom(const string& roomName);
        // load flag helpers
        void markJustLoaded();
        bool wasJustLoaded() const;
        void clearJustLoaded();

};


#endif // GAME_ENGINE_H
