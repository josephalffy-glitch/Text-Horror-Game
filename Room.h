#ifndef ROOM_H
#define ROOM_H

#include <vector>
#include <string>

using namespace std;

class Room {
    private: 
        string name;
        string description;
        vector<string> items;
        bool visited;

    public:
        // constructors
        Room();
        Room(string name, string description, vector<string> items, bool visited);
        // getters
        string getName();
        string getDescription();
        vector<string> getItems();
        bool isVisited();
        // setters
        void setName(string name);
        void setDescription(string description);
        void setItems(vector<string> items);
        void setVisited(bool visited);

};

#endif // ROOM_H