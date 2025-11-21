#include <iostream>
#include <string>
#include <vector>
#include "player.h"
#include "Room.h"
#include "GameEngine.h"
#include "Items.h"
#include <fstream>
#include <sstream> // for stringstream
#include <string>
#include "extern/imgui/imgui.h"
#include "extern/imgui/backends/imgui_impl_glfw.h"
#include "extern/imgui/backends/imgui_impl_opengl3.h"
#include "glfw/include/GLFW/glfw3.h" 

using namespace std;

void storageLocationMenu()
{
    //cout << "\n1. Inspect an item\n"; // Could add later
    cout << "\n1. Pick up an item\n";
    cout << "2. Drop an item\n";
    cout << "3. Leave\n";
    cout << "Enter your choice: ";
}

GameEngine::GameEngine()
{
    // Initialize player and rooms
    player = new Player();
    rooms = new vector<Room>();
    currentRoom = nullptr;
    visitedRooms = new vector<Room>();

    // Initialize game items
    // (Bedroom items)
    Item* mirror = new Item("mirror", ItemType::MIRROR, 0);
    Item* pencil = new Item("pencil", ItemType::MISC, 0);
    Item* bandaids = new Item("bandage", ItemType::BANDAGE, 5);
    Item* boxcutter = new Item("boxcutter", ItemType::MISC, 0);
    Item* snackBar = new Item("snack bar", ItemType::FOOD, 3);
    // (Kitchen items)
    Item* cornstarch = new Item("cornstarch", ItemType::MISC, 0);
    Item* syrup = new Item("syrup", ItemType::FOOD, 1);
    Item* salt = new Item("salt", ItemType::MISC, 0);
    Item* cookie = new Item("cookie", ItemType::FOOD, 2);

    // Create rooms and add storage locations and items
    // (Bedroom)
    Room bedroom = Room(); // create bedroom
    bedroom.setName("Bedroom");
    bedroom.setDescription("Your cozy bedroom filled with familiar items.");
	bedroom.setRoomEnum(RoomEnum::ROOM_BEDROOM);
    bedroom.addStorageLocation("Drawer");
    bedroom.addItemToLocation("Drawer", *mirror);
    bedroom.addItemToLocation("Drawer", *pencil);
    bedroom.addItemToLocation("Drawer", *bandaids);
    bedroom.addItemToLocation("Drawer", *boxcutter);
    bedroom.addItemToLocation("Drawer", *snackBar);
    rooms->push_back(bedroom); // add bedroom to rooms vector
    // (Kitchen)
    Room kitchen = Room(); // create kitchen
    kitchen.setName("Kitchen");
    kitchen.setDescription("The heart of the home, bustling with activity.");
	kitchen.setRoomEnum(RoomEnum::ROOM_KITCHEN);
    kitchen.addStorageLocation("Cabinet");
    kitchen.addItemToLocation("Cabinet", *cornstarch);
    kitchen.addItemToLocation("Cabinet", *syrup);
    kitchen.addItemToLocation("Cabinet", *salt);
    kitchen.addItemToLocation("Cabinet", *cookie);
    rooms->push_back(kitchen); // add kitchen to rooms vector
	// (Living Room)
	Room livingRoom = Room(); // create living room
    livingRoom.setName("Living Room");
    livingRoom.setDescription("A comfortable living space with a couch and a TV.");
	livingRoom.setRoomEnum(RoomEnum::ROOM_LIVING);
    livingRoom.addStorageLocation("Shelf");
    // Add items to living room shelf if needed
    rooms->push_back(livingRoom); // add living room to rooms vector

	//visitRoom("Bedroom"); // start in bedroom
}

// Singleton instance
GameEngine& GameEngine::getInstance()
{
    static GameEngine instance;
    return instance;
}

// Game Loop
Player* GameEngine::getPlayer()
{
    return player;
}

void GameEngine::useItem(const string& itemName, std::string& message_log){
    auto push_log = [&](const std::string& s) 
    {
        message_log += s + "\n";
    };

    array<Item, 5> inventory = player->getInventory();
    for (const Item& item : inventory) {
        if (item.getName() == itemName) {
            switch (item.getType()) {
                case ItemType::FOOD:
                    player->heal(item.getHealAmount());
                    push_log("You ate " + itemName + " and healed " + to_string(item.getHealAmount()) + " health.");
                    player->removeFromInventory(itemName);
                    return;
                case ItemType::BANDAGE:
                    player->heal(item.getHealAmount());
                    push_log("You used " + itemName + " and healed " + to_string(item.getHealAmount()) + " health.");
                    player->removeFromInventory(itemName);
                    return;
                case ItemType::MIRROR:
                    push_log("\nYou hold the mirror up and see your " + player->getEyes() + " eyes staring back at you.");
                    return;
                default:
                    push_log(itemName + " cannot be used right now.");
                    return;
            }
        }
    }
    cout << "You do not have " << itemName << " in your inventory.\n";
}

bool GameEngine::pickUpItem(const string& roomName, const string& locationName, const string itemName){
    //check if player's inventory is full
    if (player->fullInv()) {
        cout << "Inventory full. Cannot pick up " << itemName << ".\n";
        return false;
    }
    
    for (Room& room : *rooms) {
        if (room.getName() == roomName) {
            vector<Item> items = room.getItemsInLocation(locationName);
            for (const Item& item : items) {
                if (item.getName() == itemName) {
                    player->addToInventory(item);
                    room.removeItemFromLocation(locationName, item);
                    cout << "Picked up " << itemName << " from the " << locationName << " in the " << roomName << ".\n";
                    return true;
                }
            }
        }
    }
    cout << "Item " << itemName << " not found in " << locationName << " of " << roomName << ".\n";
    return false;
}

bool GameEngine::dropItem(const string& roomName, const string& locationName, const string itemName){
    // First check if player has the item and get a copy
    array<Item, 5> inventory = player->getInventory();
    Item itemToDrop;
    bool itemFound = false;
    
    // Find the item in player's inventory
    for (const Item& item : inventory) {
        if (item.getName() == itemName) {
            itemToDrop = item;
            itemFound = true;
            break;
        }
    }
    
    // If item not found, notify and return
    if (!itemFound) {
        cout << "You do not have " << itemName << " in your inventory.\n";
        return false;
    }
    
    // Find the room and add item to location
    for (Room& room : *rooms) {
        if (room.getName() == roomName) {
            room.addItemToLocation(locationName, itemToDrop);
            // Remove item from player's inventory using player method
            if (player->removeFromInventory(itemName)) {
                cout << "Placed " << itemName << " into the " << locationName << " in the " << roomName << ".\n";
            }
            return true;
        }
    }
    cout << "Room " << roomName << " not found.\n";
    return false;
}

// lookInStorageLocation opens up a storage location and lists items
// and then asks the player if they want to inspect, pick anything up, drop
// anything, or leave.
void GameEngine::lookInStorageLocation(const string& roomName, const string& locationName, bool& showStorageWindow, std::string& message_log){
    auto push_log = [&](const std::string& s) 
    {
        message_log += s + "\n";
    };

    showStorageWindow = true;
    push_log("You look in the " + locationName + " and see the following items:");
}

void GameEngine::renderStorageLocationWindow(const string& roomName, const string& locationName, bool& showStorageWindow, std::string& message_log) {
    auto push_log = [&](const std::string& s) 
    {
        message_log += s + "\n";
    };
    
    if (!showStorageWindow) return;

    // Create window title
    string windowTitle = locationName + " (" + roomName + ")";
    
    if (ImGui::Begin(windowTitle.c_str(), &showStorageWindow, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Contents of %s:", locationName.c_str());
        ImGui::Separator();

        // Find the room and get items
        for (Room& room : *rooms) {
            if (room.getName() == roomName) {
                vector<Item> items = room.getItemsInLocation(locationName);
                
                if (items.empty()) {
                    ImGui::Text("(empty)");
                } else {
                    // Display each item with a "Take" button
                    for (size_t i = 0; i < items.size(); ++i) {
                        const Item& item = items[i];
                        
                        // Display item name
                        ImGui::Text("%s", item.getName().c_str());
                        
                        // Same line for the "Take" button
                        ImGui::SameLine();
                        
                        // Create unique button ID
                        string btnLabel = "Take##" + locationName + "_" + to_string(i);
                        
                        if (ImGui::Button(btnLabel.c_str())) {
                            // Check if inventory is full
                            if (player->fullInv()) {
                                push_log("Inventory is full! Cannot pick up " + item.getName() + ".");
                            } else {
                                // Call pickUpItem function
                                bool success = pickUpItem(roomName, locationName, item.getName());
                                if (success) {
                                    push_log("Picked up " + item.getName() + " from the " + locationName + ".");
                                } else {
                                    push_log("Failed to pick up " + item.getName() + ".");
                                }
                            }
                        }
                        
                        // Show item description if available (optional)
                        if (item.getType() != ItemType::NULLITEM) {
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "(%s)", 
                                item.getType() == ItemType::FOOD ? "Food" :
                                item.getType() == ItemType::BANDAGE ? "Bandage" : "Item");
                        }
                    }
                }
                break;
            }
        }
        
        ImGui::Separator();
        
        // Add a "Drop Item" section
        ImGui::Text("Drop an item here:");
        array<Item, 5> inventory = player->getInventory();
        bool hasItems = false;
        
        for (size_t i = 0; i < inventory.size(); ++i) {
            const Item& item = inventory[i];
            if (item.getType() != ItemType::NULLITEM) {
                hasItems = true;
                
                ImGui::Text("%s", item.getName().c_str());
                ImGui::SameLine();
                
                string dropBtnLabel = "Drop##inventory_" + to_string(i);
                if (ImGui::Button(dropBtnLabel.c_str())) {
                    bool success = dropItem(roomName, locationName, item.getName());
                    if (success) {
                        push_log("Dropped " + item.getName() + " into the " + locationName + ".");
                    } else {
                        push_log("Failed to drop " + item.getName() + ".");
                    }
                }
            }
        }
        
        if (!hasItems) {
            ImGui::Text("(no items to drop)");
        }
        
        ImGui::Separator();
        
        // Close button
        if (ImGui::Button("Close")) {
            showStorageWindow = false;
        }
    }
    ImGui::End();
}

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

void GameEngine::visitRoom(const string& roomName) {
	for (Room& room : *rooms) {
		if (room.getName() == roomName) {
			currentRoom = &room;
			if (!room.isVisited()) {
				room.setVisited(true);
				visitedRooms->push_back(room);
			}
			return;
		}
	}
}

Room* GameEngine::getCurrentRoom() {
	if (currentRoom != nullptr) {
		return currentRoom;
	}
	return nullptr; // return nullptr if currentRoom is null
}

// Simple save format (text):
// line1: name
// line2: hair
// line3: eyes
// line4: comma-separated inventory item names (empty for empty slots)
// line5: current health (integer)

void GameEngine::saveGame(const string& saveFile)
{
    string filename = saveFile;
    if (filename.empty()) filename = "save.txt";
    string path = string("SaveFiles/") + filename;

    // Ensure SaveFiles directory exists (best-effort)
#ifdef _WIN32
    system("if not exist SaveFiles mkdir SaveFiles >nul");
#else
    system("mkdir -p SaveFiles >/dev/null 2>&1");
#endif

    ofstream ofs(path);
    if (!ofs.is_open()) {
        cout << "Failed to open save file for writing: " << path << endl;
        return;
    }

    // Write basic player info
    ofs << player->getName() << "\n";
    ofs << player->getHair() << "\n";
    ofs << player->getEyes() << "\n";

    // Inventory as comma-separated names
    array<Item,5> inv = player->getInventory();
    for (size_t i = 0; i < inv.size(); ++i) {
        ofs << inv[i].getName();
        if (i + 1 < inv.size()) ofs << ",";
    }
    ofs << "\n";

    // Current health
    ofs << player->getCurrentHealth() << "\n";

    ofs.close();
    cout << "Saved game to " << path << endl;
}

void GameEngine::loadGame(const string& saveFile)
{
    string filename = saveFile;
    if (filename.empty()) filename = "save.txt";
    string path = string("SaveFiles/") + filename;

    ifstream ifs(path);
    if (!ifs.is_open()) {
        cout << "Failed to open save file: " << path << endl;
        return;
    }

    string line;
    // name
    if (!getline(ifs, line)) { cout << "Save file malformed (name)\n"; return; }
    player->setName(line);

    // hair
    if (!getline(ifs, line)) { cout << "Save file malformed (hair)\n"; return; }
    player->setHair(line);

    // eyes
    if (!getline(ifs, line)) { cout << "Save file malformed (eyes)\n"; return; }
    player->setEyes(line);

    // inventory
    if (!getline(ifs, line)) { cout << "Save file malformed (inventory)\n"; return; }
    array<Item,5> inv;
    // parse comma-separated names
    vector<string> parts;
    stringstream ss(line);
    string itemname;
    while (getline(ss, itemname, ',')) {
        parts.push_back(itemname);
    }
    // fill inventory items
    for (size_t i = 0; i < 5; ++i) {
        string nm = (i < parts.size()) ? parts[i] : string();
        if (nm.empty()) {
            inv[i] = Item();
        } else {
            // best-effort item type mapping
            ItemType t = ItemType::MISC;
            if (nm == "mirror") t = ItemType::MIRROR;
            else if (nm == "bandage" || nm == "bandaids") t = ItemType::BANDAGE;
            else if (nm == "cookie" || nm == "snack bar" || nm == "syrup") t = ItemType::FOOD;
            inv[i] = Item(nm, t, (t==ItemType::FOOD?2: (t==ItemType::BANDAGE?5:0))); 
        }
    }
    player->setInventory(inv);

    // health
    if (!getline(ifs, line)) { cout << "Save file malformed (health)\n"; return; }
    int savedHealth = 0;
    try { savedHealth = stoi(line); } catch(...) { savedHealth = player->getCurrentHealth(); }

    // Adjust player's health to match saved value
    int current = player->getCurrentHealth();
    if (savedHealth < current) {
        player->takeDamage(current - savedHealth);
    } else if (savedHealth > current) {
        player->heal(savedHealth - current);
    }

    ifs.close();
    cout << "Loaded game from " << path << endl;
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

