





#ifndef INVENTORY_H
#define INVENTORY_H

#include "Types.h"
#include <vector>
#include <string>
#include <functional>

namespace AQE {






enum class SortType {
    NAME_ASC,        
    NAME_DESC,       
    VALUE_ASC,       
    VALUE_DESC,      
    RARITY_ASC,      
    RARITY_DESC,     
    TYPE             
};






class InventorySystem {
public:
    
    explicit InventorySystem(Player& player);
    
    
    
    
    
    
    void displayInventoryMenu();
    
    
    void displayItems() const;
    
    
    void displayItemDetails(int itemIndex) const;
    
    
    void displayEquipment() const;
    
    
    
    
    
    
    bool addItem(const Item& item);
    
    
    int addItems(const std::vector<Item>& items);
    
    
    bool removeItem(int itemIndex, int quantity = 1);
    
    
    bool removeItemById(int itemId, int quantity = 1);
    
    
    bool hasSpace() const;
    
    
    int getItemCount() const;
    
    
    bool hasItem(int itemId) const;
    
    
    int getItemQuantity(int itemId) const;
    
    
    
    
    
    
    void sortInventory(SortType sortType);
    
    
    void bubbleSortByName();
    
    
    void selectionSortByValue();
    
    
    void insertionSortByRarity();
    
    
    
    
    
    
    int linearSearchByName(const std::string& name) const;
    
    
    int binarySearchByName(const std::string& name) const;
    
    
    int searchById(int itemId) const;
    
    
    std::vector<int> findByType(ItemType type) const;
    
    
    std::vector<int> findByRarity(ItemRarity rarity) const;
    
    
    std::vector<int> searchPartial(const std::string& partialName) const;
    
    
    
    
    
    
    bool useItem(int itemIndex);
    
    
    bool equipItem(int itemIndex);
    
    
    bool unequipItem(int itemIndex);
    
    
    bool applyItemEffect(const Item& item);
    
    
    Item dropItem(int itemIndex);
    
    
    
    
    
    
    int calculateTotalValue() const;
    
    
    int getSellPrice(int itemIndex) const;

private:
    Player& playerRef;
    
    
    bool tryStackItem(const Item& item);
    
    
    std::function<bool(const Item&, const Item&)> getComparator(SortType sortType) const;
};






std::vector<Item> createStartingItems(CharacterClass charClass);


Item generateRandomItem(ItemRarity rarity, int targetLevel = 1);


Item generateItemFromTemplate(const Item& templateItem, int targetLevel = 1,
                              bool rollPerk = true);


bool canUseItem(const Item& item, bool inCombat);


int getRarityColor(ItemRarity rarity);


std::string formatItemName(const Item& item);

}  

#endif 
