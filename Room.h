#ifndef ROOM_H
#define ROOM_H

#include <vector>
#include <string>
#include "Items.h"

using namespace std;


enum class RoomEnum {
	ROOM_BEDROOM,
	ROOM_KITCHEN,
	ROOM_LIVING,
	NULL_ROOM
};


struct StorageLocation {
    string name;
    vector<Item> items;
};

class Room {
    private: 
        string name;
        string description;
        vector<StorageLocation> storageLocations;
        bool visited;
		RoomEnum roomEnum;

    public:
        // constructors
        Room();
        Room(string name, string description, vector<StorageLocation> storageLocations, bool visited,RoomEnum roomEnum);
        // getters
        string getName() const;
        string getDescription() const;
        RoomEnum getRoomEnum() const;
        vector<StorageLocation> getStorageLocations() const;
        vector<Item> getItemsInLocation(string locationName) const;
        bool isVisited() const;
        // setters
        void setName(string name);
        void setDescription(string description);
		void setRoomEnum(RoomEnum roomEnum);
        void addStorageLocation(string locationName);
        void addItemToLocation(string locationName, Item item);
        void removeItemFromLocation(string locationName, Item item);
        void setVisited(bool visited);

};

#endif // ROOM_H