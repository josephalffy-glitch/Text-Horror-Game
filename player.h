#include <string>
using namespace std;

class Player
{
    public:
        void UpdateHealth(string food);
        void CheckHealth();
        void CheckInventory();
        void UpdateInventory(string item);
};