#include <string>
#include <vector>
using namespace std;

class Player
{
    public:
        void UpdateHealth(string food);
        void CheckHealth();
        void CheckInventory(vector<string>& Inventory);
        void UpdateInventory(vector<string>& Inventory,string item);
};
