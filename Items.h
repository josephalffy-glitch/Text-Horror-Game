#ifndef ITEMS_H
#define ITEMS_H
#include <string>

using namespace std;

enum class ItemType {
    FOOD,
    BANDAGE,
    KEY,
    MISC,
    MIRROR,
	BOXCUTTER,
    NULLITEM
};

class Item {
    private:
        string itemName;
        ItemType itemType;
        int healAmount;

    public:
        Item(string name = "", ItemType type = ItemType::NULLITEM, int heal = 0)
            : itemName(name), itemType(type), healAmount(heal) {}

        string getName() const {
            return itemName;
        }

        ItemType getType() const {
            return itemType;
        }

        int getHealAmount() const {
            return healAmount;
        }

};

#endif // ITEMS_H