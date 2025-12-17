/*******************************************************************************
 * C++ Adventure Quest Engine
 * Inventory.h - Inventory Management Header
 * 
 * This file contains declarations for the inventory system including
 * item management, sorting, searching, equipment, and item effects.
 * 
 * Author: Adventure Quest Engine Team
 * Version: 1.0
 ******************************************************************************/

#ifndef INVENTORY_H
#define INVENTORY_H

#include "Types.h"
#include <vector>
#include <string>
#include <functional>

namespace AQE {

//==============================================================================
// SORT TYPES
//==============================================================================

/**
 * @brief Sorting criteria for inventory
 */
enum class SortType {
    NAME_ASC,        // Alphabetical A-Z
    NAME_DESC,       // Alphabetical Z-A
    VALUE_ASC,       // Price low to high
    VALUE_DESC,      // Price high to low
    RARITY_ASC,      // Common to Legendary
    RARITY_DESC,     // Legendary to Common
    TYPE             // Group by item type
};

//==============================================================================
// INVENTORY SYSTEM CLASS
//==============================================================================

/**
 * @brief Manages player inventory operations
 * 
 * Provides functionality for adding, removing, sorting, searching,
 * equipping items, and applying item effects.
 */
class InventorySystem {
public:
    /**
     * @brief Constructor
     * @param player Reference to player
     */
    explicit InventorySystem(Player& player);
    
    //==========================================================================
    // INVENTORY DISPLAY
    //==========================================================================
    
    /**
     * @brief Display full inventory menu
     */
    void displayInventoryMenu();
    
    /**
     * @brief Display item list with details
     */
    void displayItems() const;
    
    /**
     * @brief Display detailed item information
     * @param itemIndex Index of item to display
     */
    void displayItemDetails(int itemIndex) const;
    
    /**
     * @brief Display equipped items
     */
    void displayEquipment() const;
    
    //==========================================================================
    // ITEM MANAGEMENT
    //==========================================================================
    
    /**
     * @brief Add item to inventory
     * @param item Item to add
     * @return True if added successfully
     */
    bool addItem(const Item& item);
    
    /**
     * @brief Add multiple items to inventory
     * @param items Vector of items to add
     * @return Number of items added
     */
    int addItems(const std::vector<Item>& items);
    
    /**
     * @brief Remove item from inventory
     * @param itemIndex Index of item to remove
     * @param quantity Amount to remove (default 1)
     * @return True if removed successfully
     */
    bool removeItem(int itemIndex, int quantity = 1);
    
    /**
     * @brief Remove item by ID
     * @param itemId ID of item to remove
     * @param quantity Amount to remove
     * @return True if removed
     */
    bool removeItemById(int itemId, int quantity = 1);
    
    /**
     * @brief Check if inventory has space
     * @return True if space available
     */
    bool hasSpace() const;
    
    /**
     * @brief Get current inventory size
     * @return Number of items
     */
    int getItemCount() const;
    
    /**
     * @brief Check if player has item
     * @param itemId Item ID to check
     * @return True if item exists
     */
    bool hasItem(int itemId) const;
    
    /**
     * @brief Get item quantity
     * @param itemId Item ID to check
     * @return Quantity of item
     */
    int getItemQuantity(int itemId) const;
    
    //==========================================================================
    // SORTING (Multiple algorithms implemented)
    //==========================================================================
    
    /**
     * @brief Sort inventory by specified criteria
     * @param sortType Sorting criteria
     */
    void sortInventory(SortType sortType);
    
    /**
     * @brief Sort inventory using bubble sort (alphabetical)
     */
    void bubbleSortByName();
    
    /**
     * @brief Sort inventory using selection sort (by value)
     */
    void selectionSortByValue();
    
    /**
     * @brief Sort inventory using insertion sort (by rarity)
     */
    void insertionSortByRarity();
    
    //==========================================================================
    // SEARCHING
    //==========================================================================
    
    /**
     * @brief Linear search for item by name
     * @param name Item name to search
     * @return Index of item (-1 if not found)
     */
    int linearSearchByName(const std::string& name) const;
    
    /**
     * @brief Binary search for item by name (requires sorted inventory)
     * @param name Item name to search
     * @return Index of item (-1 if not found)
     */
    int binarySearchByName(const std::string& name) const;
    
    /**
     * @brief Search for item by ID
     * @param itemId Item ID to search
     * @return Index of item (-1 if not found)
     */
    int searchById(int itemId) const;
    
    /**
     * @brief Find all items of a specific type
     * @param type Item type to find
     * @return Vector of indices
     */
    std::vector<int> findByType(ItemType type) const;
    
    /**
     * @brief Find all items of a specific rarity
     * @param rarity Rarity to find
     * @return Vector of indices
     */
    std::vector<int> findByRarity(ItemRarity rarity) const;
    
    /**
     * @brief Search items by partial name match
     * @param partialName Part of name to search
     * @return Vector of matching indices
     */
    std::vector<int> searchPartial(const std::string& partialName) const;
    
    //==========================================================================
    // ITEM USAGE
    //==========================================================================
    
    /**
     * @brief Use an item from inventory
     * @param itemIndex Index of item to use
     * @return True if used successfully
     */
    bool useItem(int itemIndex);
    
    /**
     * @brief Equip an item
     * @param itemIndex Index of item to equip
     * @return True if equipped
     */
    bool equipItem(int itemIndex);
    
    /**
     * @brief Unequip an item
     * @param itemIndex Index of item to unequip
     * @return True if unequipped
     */
    bool unequipItem(int itemIndex);
    
    /**
     * @brief Apply consumable item effect
     * @param item The item to apply
     * @return True if effect applied
     */
    bool applyItemEffect(const Item& item);
    
    /**
     * @brief Drop item (remove without using)
     * @param itemIndex Index of item to drop
     * @return Dropped item
     */
    Item dropItem(int itemIndex);
    
    //==========================================================================
    // ECONOMY
    //==========================================================================
    
    /**
     * @brief Calculate total inventory value
     * @return Total gold value
     */
    int calculateTotalValue() const;
    
    /**
     * @brief Get sell price for item
     * @param itemIndex Index of item
     * @return Gold value when sold
     */
    int getSellPrice(int itemIndex) const;

private:
    Player& playerRef;
    
    /**
     * @brief Stack identical items
     * @param item Item to stack
     * @return True if stacked with existing item
     */
    bool tryStackItem(const Item& item);
    
    /**
     * @brief Get comparison function for sorting
     * @param sortType Sort criteria
     * @return Comparison function
     */
    std::function<bool(const Item&, const Item&)> getComparator(SortType sortType) const;
};

//==============================================================================
// STANDALONE INVENTORY FUNCTIONS
//==============================================================================

/**
 * @brief Create a list of default starting items
 * @param charClass Character class
 * @return Vector of starting items
 */
std::vector<Item> createStartingItems(CharacterClass charClass);

/**
 * @brief Generate random item based on rarity
 * @param rarity Desired rarity
 * @return Generated item
 */
Item generateRandomItem(ItemRarity rarity);

/**
 * @brief Generate item from template with randomization
 * @param templateItem Base item template
 * @return Randomized item
 */
Item generateItemFromTemplate(const Item& templateItem);

/**
 * @brief Check if item can be used in current context
 * @param item Item to check
 * @param inCombat Are we in combat?
 * @return True if usable
 */
bool canUseItem(const Item& item, bool inCombat);

/**
 * @brief Get rarity color code for display
 * @param rarity Item rarity
 * @return ANSI color code
 */
int getRarityColor(ItemRarity rarity);

/**
 * @brief Format item name with rarity color
 * @param item Item to format
 * @return Formatted string
 */
std::string formatItemName(const Item& item);

}  // namespace AQE

#endif // INVENTORY_H
