#ifndef LOADGAME_H
#define LOADGAME_H

#include <vector>
#include <string>

// List save files found in the SaveFiles/ directory (filenames only)
std::vector<std::string> listSaveFiles();

// Render a simple ImGui window to select and load a save file.
// - `show` toggles the window visibility.
// - `engine` is used to call `engine.loadGame(filename)` when a file is chosen.
// - `message_log` will be appended with a small status message when a file is loaded.
void renderLoadGameWindow(bool &show, class GameEngine &engine, std::string &message_log);

// Render an ImGui window to save the current game into one of three slots.
// Only the three predefined slots are used; choosing a slot overwrites its file.
void renderSaveGameWindow(bool &show, class GameEngine &engine, std::string &message_log);

#endif // LOADGAME_H
