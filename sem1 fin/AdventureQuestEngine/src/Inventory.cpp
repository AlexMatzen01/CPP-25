//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Inventory management implementation

#include "../include/Inventory.h"
#include "../include/Utils.h"
#include "../include/Constants.h"
#include "../include/UIEnhanced.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace AQE {

namespace {

bool isEquippableItem(const Item& item) {
    return item.type == ItemType::WEAPON ||
           item.type == ItemType::ARMOR ||
           item.type == ItemType::ACCESSORY;
}

std::string getSlotLabel(ItemType type) {
    switch (type) {
        case ItemType::WEAPON:
            return "weapon";
        case ItemType::ARMOR:
            return "armor";
        case ItemType::ACCESSORY:
            return "accessory";
        default:
            return "item";
    }
}

const Item* findEquippedItemForType(const Player& player, ItemType type) {
    for (const Item& item : player.inventory) {
        if (item.type == type && item.isEquipped) {
            return &item;
        }
    }

    int equippedId = -1;
    switch (type) {
        case ItemType::WEAPON:
            equippedId = player.equippedWeaponId;
            break;
        case ItemType::ARMOR:
            equippedId = player.equippedArmorId;
            break;
        case ItemType::ACCESSORY:
            equippedId = player.equippedAccessoryId;
            break;
        default:
            break;
    }

    if (equippedId >= 0) {
        for (const Item& item : player.inventory) {
            if (item.id == equippedId) {
                return &item;
            }
        }
    }

    return nullptr;
}

int getComparisonScore(const Item& item) {
    return item.attackBonus * 2 + item.defenseBonus * 2 +
           item.critBonus + item.dodgeBonus + item.speedBonus +
           (item.itemLevel * 2) + (item.hasUniquePerk ? 15 : 0);
}

const Item* findBestUnequippedAlternative(const Player& player, ItemType type) {
    const Item* bestItem = nullptr;
    int bestScore = 0;
    bool foundAny = false;

    for (const Item& item : player.inventory) {
        if (item.type != type || item.isEquipped) {
            continue;
        }

        int score = getComparisonScore(item);
        if (!foundAny || score > bestScore ||
            (score == bestScore && item.value > bestItem->value)) {
            bestItem = &item;
            bestScore = score;
            foundAny = true;
        }
    }

    return bestItem;
}

std::string formatSignedDelta(int value, const std::string& suffix = "") {
    std::ostringstream ss;
    if (value > 0) {
        ss << "+";
    }
    ss << value << suffix;
    return ss.str();
}

std::string buildComparisonSummary(const Item& candidate, const Item& baseline) {
    std::vector<std::string> parts;

    auto addPart = [&](const std::string& label, int delta, const std::string& suffix = "") {
        if (delta != 0) {
            parts.push_back(label + " " + formatSignedDelta(delta, suffix));
        }
    };

    addPart("ATK", candidate.attackBonus - baseline.attackBonus);
    addPart("DEF", candidate.defenseBonus - baseline.defenseBonus);
    addPart("iLvl", candidate.itemLevel - baseline.itemLevel);
    addPart("CRIT", candidate.critBonus - baseline.critBonus, "%");
    addPart("DODGE", candidate.dodgeBonus - baseline.dodgeBonus, "%");
    addPart("SPD", candidate.speedBonus - baseline.speedBonus);
    addPart("VAL", candidate.value - baseline.value);

    if (parts.empty()) {
        return "No stat changes";
    }

    std::ostringstream ss;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            ss << ", ";
        }
        ss << parts[i];
    }
    return ss.str();
}

const Item* getComparisonBaseline(const Player& player, const Item& item) {
    if (!isEquippableItem(item)) {
        return nullptr;
    }

    if (item.isEquipped) {
        return findBestUnequippedAlternative(player, item.type);
    }

    return findEquippedItemForType(player, item.type);
}

}  // namespace

// ---
// CONSTRUCTOR
// ---

InventorySystem::InventorySystem(Player& player) : playerRef(player) {}

// ---
// INVENTORY DISPLAY
// ---

void InventorySystem::displayInventoryMenu() {
    bool exitMenu = false;
    
    while (!exitMenu) {
        clearScreen();
        std::cout << "\n";
        displaySeparator(60, '=');
        std::cout << centerString("INVENTORY", 60) << "\n";
        displaySeparator(60, '=');
        
        std::cout << "\nGold: " << playerRef.gold << "\n";
        std::cout << "Items: " << getItemCount();
        if (playerRef.maxInventorySize > 0) {
            std::cout << "/" << playerRef.maxInventorySize;
        }
        std::cout << "\n";
        
        std::vector<std::string> options = {
            "View All Items",
            "View Equipment",
            "Use Item",
            "Equip/Unequip Item",
            "Sort Inventory",
            "Search Items",
            "Drop Item",
            "Back to Game"
        };
        
        int choice = displayMenu("Inventory Options", options);
        
        switch (choice) {
            case 1:
                displayItems();
                pause();
                break;
            case 2:
                displayEquipment();
                pause();
                break;
            case 3: {
                displayItems();
                if (!playerRef.inventory.empty()) {
                    int idx = getValidatedInt("\nSelect item to use (0 to cancel): ", 
                                              0, getItemCount());
                    if (idx > 0) {
                        useItem(idx - 1);
                        pause();
                    }
                } else {
                    pause();
                }
                break;
            }
            case 4: {
                displayItems();
                if (!playerRef.inventory.empty()) {
                    int idx = getValidatedInt("\nSelect item to equip/unequip (0 to cancel): ",
                                              0, getItemCount());
                    if (idx > 0) {
                        Item& item = playerRef.inventory[idx - 1];
                        if (item.isEquipped) {
                            unequipItem(idx - 1);
                        } else {
                            equipItem(idx - 1);
                        }
                        pause();
                    }
                } else {
                    pause();
                }
                break;
            }
            case 5: {
                std::vector<std::string> sortOptions = {
                    "Sort by Name (A-Z)",
                    "Sort by Name (Z-A)",
                    "Sort by Value (Low-High)",
                    "Sort by Value (High-Low)",
                    "Sort by Rarity (Common-Legendary)",
                    "Sort by Rarity (Legendary-Common)",
                    "Sort by Type",
                    "Cancel"
                };
                int sortChoice = displayMenu("Sort Options", sortOptions);
                if (sortChoice < 8) {
                    sortInventory(static_cast<SortType>(sortChoice - 1));
                    std::cout << "\nInventory sorted!\n";
                    pause();
                }
                break;
            }
            case 6: {
                std::string query = getValidatedString("\nEnter item name to search: ", 1, 50);
                std::vector<int> results = searchPartial(query);
                
                if (results.empty()) {
                    std::cout << "No items found matching '" << query << "'.\n";
                } else {
                    std::cout << "\nFound " << results.size() << " item(s):\n";
                    for (int idx : results) {
                        displayItemDetails(idx);
                    }
                }
                pause();
                break;
            }
            case 7: {
                displayItems();
                if (!playerRef.inventory.empty()) {
                    int idx = getValidatedInt("\nSelect item to drop (0 to cancel): ",
                                              0, getItemCount());
                    if (idx > 0) {
                        if (getYesNo("Are you sure?")) {
                            Item dropped = dropItem(idx - 1);
                            std::cout << "Dropped " << dropped.name << ".\n";
                        }
                        pause();
                    }
                } else {
                    pause();
                }
                break;
            }
            case 8:
                exitMenu = true;
                break;
        }
    }
}

void InventorySystem::displayItems() const {
    std::cout << "\n";
    displaySeparator(80, '-');
    std::cout << std::left << std::setw(5) << "#"
              << std::setw(23) << "Name"
              << std::setw(12) << "Type"
              << std::setw(10) << "Rarity"
              << std::setw(8) << "iLvl"
              << std::setw(8) << "Value"
              << std::setw(5) << "Qty"
              << "\n";
    displaySeparator(80, '-');
    
    if (playerRef.inventory.empty()) {
        std::cout << "  Your inventory is empty.\n";
        displaySeparator(80, '-');
        return;
    }
    
    for (size_t i = 0; i < playerRef.inventory.size(); ++i) {
        const Item& item = playerRef.inventory[i];
        
        std::string name = item.name;
        if (item.isEquipped) name += " [E]";
        if (name.length() > 24) name = name.substr(0, 21) + "...";
        
        std::cout << std::left << std::setw(5) << (i + 1)
                  << std::setw(23) << name
                  << std::setw(12) << itemTypeToString(item.type)
                  << std::setw(10) << rarityToString(item.rarity)
                  << std::setw(8) << item.itemLevel
                  << std::setw(8) << item.value
                  << std::setw(5) << item.quantity
                  << "\n";

        if (isEquippableItem(item)) {
            const Item* baseline = getComparisonBaseline(playerRef, item);
            std::cout << "  " << (item.isEquipped ? "Best alternative" : "Compare vs equipped") << ": ";

            if (baseline != nullptr) {
                if (item.isEquipped) {
                    std::cout << baseline->name << " (" << buildComparisonSummary(*baseline, item) << ")";
                } else {
                    std::cout << baseline->name << " (" << buildComparisonSummary(item, *baseline) << ")";
                }
            } else {
                std::cout << "No " << getSlotLabel(item.type) << " comparison available";
            }

            std::cout << "\n";
        }
    }
    displaySeparator(80, '-');
    std::cout << "Total Value: " << calculateTotalValue() << " gold\n";
}

void InventorySystem::displayItemDetails(int itemIndex) const {
    if (itemIndex < 0 || itemIndex >= static_cast<int>(playerRef.inventory.size())) {
        std::cout << "Invalid item index.\n";
        return;
    }
    
    const Item& item = playerRef.inventory[itemIndex];
    
    std::cout << "\n";
    displaySeparator(50, '-');
    std::cout << "  " << item.name;
    if (item.isEquipped) std::cout << " [EQUIPPED]";
    std::cout << "\n";
    displaySeparator(50, '-');
    
    std::cout << "  Type:        " << itemTypeToString(item.type) << "\n";
    std::cout << "  Rarity:      " << rarityToString(item.rarity) << "\n";
    std::cout << "  Item Level:  " << item.itemLevel << " ("
              << itemLevelTierToString(item.itemLevel) << ")\n";
    std::cout << "  Value:       " << item.value << " gold\n";
    std::cout << "  Quantity:    " << item.quantity << "\n";
    if (item.hasUniquePerk && !item.uniquePerkName.empty()) {
        std::cout << "  Perk:        " << item.uniquePerkName << "\n";
    }
    std::cout << "  Description: " << wrapText(item.description, 40) << "\n";
    
    // Show stats if applicable
    if (item.type == ItemType::WEAPON || item.type == ItemType::ARMOR || 
        item.type == ItemType::ACCESSORY) {
        std::cout << "  Stats:\n";
        if (item.attackBonus != 0) std::cout << "    Attack: +" << item.attackBonus << "\n";
        if (item.defenseBonus != 0) std::cout << "    Defense: +" << item.defenseBonus << "\n";
        if (item.critBonus != 0) std::cout << "    Crit: +" << item.critBonus << "%\n";
        if (item.dodgeBonus != 0) std::cout << "    Dodge: +" << item.dodgeBonus << "%\n";
        if (item.speedBonus != 0) std::cout << "    Speed: +" << item.speedBonus << "\n";
    }
    
    if (item.type == ItemType::CONSUMABLE && item.healAmount > 0) {
        std::cout << "  Heals: " << item.healAmount << " HP\n";
    }

    if (isEquippableItem(item)) {
        const Item* baseline = getComparisonBaseline(playerRef, item);

        std::cout << "  Comparison:  ";
        if (baseline != nullptr) {
            if (item.isEquipped) {
                std::cout << "Best alternative -> " << baseline->name << "\n";
                UIManager::displayItemComparison(*baseline, item);
            } else {
                std::cout << "Vs equipped -> " << baseline->name << "\n";
                UIManager::displayItemComparison(item, *baseline);
            }
        } else {
            std::cout << "No " << getSlotLabel(item.type) << " comparison available\n";
        }
    }
    
    displaySeparator(50, '-');
}

void InventorySystem::displayEquipment() const {
    std::cout << "\n";
    displaySeparator(50, '=');
    std::cout << centerString("EQUIPMENT", 50) << "\n";
    displaySeparator(50, '=');
    
    // Find equipped items
    std::string weapon = "Empty";
    std::string armor = "Empty";
    std::string accessory = "Empty";
    
    int totalAtk = 0, totalDef = 0, totalCrit = 0, totalDodge = 0, totalSpd = 0;
    
    for (const Item& item : playerRef.inventory) {
        if (item.isEquipped) {
            switch (item.type) {
                case ItemType::WEAPON:
                    weapon = item.name + " [ATK+" + std::to_string(item.attackBonus) + "]";
                    break;
                case ItemType::ARMOR:
                    armor = item.name + " [DEF+" + std::to_string(item.defenseBonus) + "]";
                    break;
                case ItemType::ACCESSORY:
                    accessory = item.name;
                    break;
                default:
                    break;
            }
            totalAtk += item.attackBonus;
            totalDef += item.defenseBonus;
            totalCrit += item.critBonus;
            totalDodge += item.dodgeBonus;
            totalSpd += item.speedBonus;
        }
    }
    
    std::cout << "\n  Weapon:    " << weapon << "\n";
    std::cout << "  Armor:     " << armor << "\n";
    std::cout << "  Accessory: " << accessory << "\n";
    std::cout << "  Average Item Level: " << playerRef.averageItemLevel << "\n";
    
    std::cout << "\n  Total Equipment Bonuses:\n";
    std::cout << "    Attack:  +" << totalAtk << "\n";
    std::cout << "    Defense: +" << totalDef << "\n";
    std::cout << "    Crit:    +" << totalCrit << "%\n";
    std::cout << "    Dodge:   +" << totalDodge << "%\n";
    std::cout << "    Speed:   +" << totalSpd << "\n";

    std::cout << "\n  Upgrade Comparisons:\n";

    auto printComparison = [&](ItemType type, const std::string& label, const Item& equippedItem) {
        const Item* alternative = findBestUnequippedAlternative(playerRef, type);
        std::cout << "    " << label << ": ";

        if (alternative != nullptr) {
            std::cout << alternative->name << " (" << buildComparisonSummary(*alternative, equippedItem) << ")\n";
        } else {
            std::cout << "No better " << getSlotLabel(type) << " found\n";
        }
    };

    bool hasWeapon = false;
    bool hasArmor = false;
    bool hasAccessory = false;

    for (const Item& item : playerRef.inventory) {
        if (!item.isEquipped) {
            continue;
        }

        switch (item.type) {
            case ItemType::WEAPON:
                if (!hasWeapon) {
                    printComparison(ItemType::WEAPON, "Weapon", item);
                    hasWeapon = true;
                }
                break;
            case ItemType::ARMOR:
                if (!hasArmor) {
                    printComparison(ItemType::ARMOR, "Armor", item);
                    hasArmor = true;
                }
                break;
            case ItemType::ACCESSORY:
                if (!hasAccessory) {
                    printComparison(ItemType::ACCESSORY, "Accessory", item);
                    hasAccessory = true;
                }
                break;
            default:
                break;
        }
    }

    if (!hasWeapon) {
        std::cout << "    Weapon: No weapon equipped\n";
    }
    if (!hasArmor) {
        std::cout << "    Armor: No armor equipped\n";
    }
    if (!hasAccessory) {
        std::cout << "    Accessory: No accessory equipped\n";
    }
    
    displaySeparator(50, '=');
}

// ---
// ITEM MANAGEMENT
// ---

bool InventorySystem::addItem(const Item& item) {
    // Check inventory space
    if (!hasSpace()) {
        std::cout << "Inventory is full! Cannot add " << item.name << ".\n";
        return false;
    }
    
    // Try to stack with existing item
    if (item.isStackable && tryStackItem(item)) {
        return true;
    }
    
    // Add as new item
    playerRef.inventory.push_back(item);
    return true;
}

int InventorySystem::addItems(const std::vector<Item>& items) {
    int added = 0;
    for (const Item& item : items) {
        if (addItem(item)) {
            added++;
        }
    }
    return added;
}

bool InventorySystem::removeItem(int itemIndex, int quantity) {
    if (itemIndex < 0 || itemIndex >= static_cast<int>(playerRef.inventory.size())) {
        return false;
    }
    
    Item& item = playerRef.inventory[itemIndex];
    
    if (item.isEquipped) {
        unequipItem(itemIndex);
    }
    
    if (item.quantity <= quantity) {
        playerRef.inventory.erase(playerRef.inventory.begin() + itemIndex);
    } else {
        item.quantity -= quantity;
    }
    
    return true;
}

bool InventorySystem::removeItemById(int itemId, int quantity) {
    int idx = searchById(itemId);
    if (idx >= 0) {
        return removeItem(idx, quantity);
    }
    return false;
}

bool InventorySystem::hasSpace() const {
    if (playerRef.maxInventorySize <= 0) return true;  // Unlimited
    return static_cast<int>(playerRef.inventory.size()) < playerRef.maxInventorySize;
}

int InventorySystem::getItemCount() const {
    return static_cast<int>(playerRef.inventory.size());
}

bool InventorySystem::hasItem(int itemId) const {
    return searchById(itemId) >= 0;
}

int InventorySystem::getItemQuantity(int itemId) const {
    int idx = searchById(itemId);
    if (idx >= 0) {
        return playerRef.inventory[idx].quantity;
    }
    return 0;
}

bool InventorySystem::tryStackItem(const Item& item) {
    for (Item& existing : playerRef.inventory) {
        if (existing.id == item.id && existing.isStackable) {
            int newQty = existing.quantity + item.quantity;
            if (newQty > STACK_LIMIT) {
                // Partial stack - stack is now full
                existing.quantity = STACK_LIMIT;
                // Would need to add new stack for overflow
                return true;
            }
            existing.quantity = newQty;
            return true;
        }
    }
    return false;
}

// ---
// SORTING ALGORITHMS
// ---

void InventorySystem::sortInventory(SortType sortType) {
    switch (sortType) {
        case SortType::NAME_ASC:
        case SortType::NAME_DESC:
            bubbleSortByName();
            if (sortType == SortType::NAME_DESC) {
                std::reverse(playerRef.inventory.begin(), playerRef.inventory.end());
            }
            break;
        case SortType::VALUE_ASC:
        case SortType::VALUE_DESC:
            selectionSortByValue();
            if (sortType == SortType::VALUE_DESC) {
                std::reverse(playerRef.inventory.begin(), playerRef.inventory.end());
            }
            break;
        case SortType::RARITY_ASC:
        case SortType::RARITY_DESC:
            insertionSortByRarity();
            if (sortType == SortType::RARITY_DESC) {
                std::reverse(playerRef.inventory.begin(), playerRef.inventory.end());
            }
            break;
        case SortType::TYPE:
            std::sort(playerRef.inventory.begin(), playerRef.inventory.end(),
                [](const Item& a, const Item& b) {
                    return static_cast<int>(a.type) < static_cast<int>(b.type);
                });
            break;
    }
}

void InventorySystem::bubbleSortByName() {
    // Classic bubble sort - O(n^2)
    int n = static_cast<int>(playerRef.inventory.size());
    
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        
        for (int j = 0; j < n - i - 1; ++j) {
            // Compare names (case-insensitive)
            std::string nameA = toLower(playerRef.inventory[j].name);
            std::string nameB = toLower(playerRef.inventory[j + 1].name);
            
            if (nameA > nameB) {
                std::swap(playerRef.inventory[j], playerRef.inventory[j + 1]);
                swapped = true;
            }
        }
        
        // Optimization: if no swaps, already sorted
        if (!swapped) break;
    }
}

void InventorySystem::selectionSortByValue() {
    // Classic selection sort - O(n^2)
    int n = static_cast<int>(playerRef.inventory.size());
    
    for (int i = 0; i < n - 1; ++i) {
        int minIdx = i;
        
        for (int j = i + 1; j < n; ++j) {
            if (playerRef.inventory[j].value < playerRef.inventory[minIdx].value) {
                minIdx = j;
            }
        }
        
        if (minIdx != i) {
            std::swap(playerRef.inventory[i], playerRef.inventory[minIdx]);
        }
    }
}

void InventorySystem::insertionSortByRarity() {
    // Classic insertion sort - O(n^2)
    int n = static_cast<int>(playerRef.inventory.size());
    
    for (int i = 1; i < n; ++i) {
        Item key = playerRef.inventory[i];
        int j = i - 1;
        
        // Move items with higher rarity up
        while (j >= 0 && static_cast<int>(playerRef.inventory[j].rarity) > 
                         static_cast<int>(key.rarity)) {
            playerRef.inventory[j + 1] = playerRef.inventory[j];
            j--;
        }
        
        playerRef.inventory[j + 1] = key;
    }
}

// ---
// SEARCHING ALGORITHMS
// ---

int InventorySystem::linearSearchByName(const std::string& name) const {
    // Simple linear search - O(n)
    std::string searchName = toLower(name);
    
    for (size_t i = 0; i < playerRef.inventory.size(); ++i) {
        if (toLower(playerRef.inventory[i].name) == searchName) {
            return static_cast<int>(i);
        }
    }
    
    return -1;  // Not found
}

int InventorySystem::binarySearchByName(const std::string& name) const {
    // Binary search - O(log n) - requires sorted inventory
    std::string searchName = toLower(name);
    
    int left = 0;
    int right = static_cast<int>(playerRef.inventory.size()) - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        std::string midName = toLower(playerRef.inventory[mid].name);
        
        if (midName == searchName) {
            return mid;  // Found
        } else if (midName < searchName) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return -1;  // Not found
}

int InventorySystem::searchById(int itemId) const {
    for (size_t i = 0; i < playerRef.inventory.size(); ++i) {
        if (playerRef.inventory[i].id == itemId) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

std::vector<int> InventorySystem::findByType(ItemType type) const {
    std::vector<int> results;
    
    for (size_t i = 0; i < playerRef.inventory.size(); ++i) {
        if (playerRef.inventory[i].type == type) {
            results.push_back(static_cast<int>(i));
        }
    }
    
    return results;
}

std::vector<int> InventorySystem::findByRarity(ItemRarity rarity) const {
    std::vector<int> results;
    
    for (size_t i = 0; i < playerRef.inventory.size(); ++i) {
        if (playerRef.inventory[i].rarity == rarity) {
            results.push_back(static_cast<int>(i));
        }
    }
    
    return results;
}

std::vector<int> InventorySystem::searchPartial(const std::string& partialName) const {
    std::vector<int> results;
    std::string searchTerm = toLower(partialName);
    
    for (size_t i = 0; i < playerRef.inventory.size(); ++i) {
        std::string itemName = toLower(playerRef.inventory[i].name);
        if (itemName.find(searchTerm) != std::string::npos) {
            results.push_back(static_cast<int>(i));
        }
    }
    
    return results;
}

// ---
// ITEM USAGE
// ---

bool InventorySystem::useItem(int itemIndex) {
    if (itemIndex < 0 || itemIndex >= static_cast<int>(playerRef.inventory.size())) {
        std::cout << "Invalid item.\n";
        return false;
    }
    
    Item& item = playerRef.inventory[itemIndex];
    
    if (!item.isUsable) {
        std::cout << item.name << " cannot be used.\n";
        return false;
    }
    
    bool used = applyItemEffect(item);
    
    if (used && item.type == ItemType::CONSUMABLE) {
        removeItem(itemIndex, 1);
    }
    
    return used;
}

bool InventorySystem::equipItem(int itemIndex) {
    if (itemIndex < 0 || itemIndex >= static_cast<int>(playerRef.inventory.size())) {
        return false;
    }
    
    Item& item = playerRef.inventory[itemIndex];
    
    // Check if equippable
    if (item.type != ItemType::WEAPON && item.type != ItemType::ARMOR &&
        item.type != ItemType::ACCESSORY) {
        std::cout << item.name << " cannot be equipped.\n";
        return false;
    }
    
    if (item.isEquipped) {
        std::cout << item.name << " is already equipped.\n";
        return false;
    }
    
    // Unequip existing item in same slot
    for (Item& existing : playerRef.inventory) {
        if (existing.isEquipped && existing.type == item.type) {
            existing.isEquipped = false;
            std::cout << "Unequipped " << existing.name << ".\n";
            break;
        }
    }
    
    // Equip new item
    item.isEquipped = true;
    std::cout << "Equipped " << item.name << "!\n";
    
    // Update player equipment IDs
    switch (item.type) {
        case ItemType::WEAPON:
            playerRef.equippedWeaponId = item.id;
            break;
        case ItemType::ARMOR:
            playerRef.equippedArmorId = item.id;
            break;
        case ItemType::ACCESSORY:
            playerRef.equippedAccessoryId = item.id;
            break;
        default:
            break;
    }

    refreshAverageItemLevel(playerRef);
    
    return true;
}

bool InventorySystem::unequipItem(int itemIndex) {
    if (itemIndex < 0 || itemIndex >= static_cast<int>(playerRef.inventory.size())) {
        return false;
    }
    
    Item& item = playerRef.inventory[itemIndex];
    
    if (!item.isEquipped) {
        std::cout << item.name << " is not equipped.\n";
        return false;
    }
    
    item.isEquipped = false;
    
    // Update player equipment IDs
    switch (item.type) {
        case ItemType::WEAPON:
            playerRef.equippedWeaponId = -1;
            break;
        case ItemType::ARMOR:
            playerRef.equippedArmorId = -1;
            break;
        case ItemType::ACCESSORY:
            playerRef.equippedAccessoryId = -1;
            break;
        default:
            break;
    }

    refreshAverageItemLevel(playerRef);
    
    std::cout << "Unequipped " << item.name << ".\n";
    return true;
}

bool InventorySystem::applyItemEffect(const Item& item) {
    bool hasEffect = false;
    
    // Healing
    if (item.healAmount > 0) {
        int healed = item.healAmount;
        int before = playerRef.stats.currentHP;
        playerRef.stats.currentHP += healed;
        if (playerRef.stats.currentHP > playerRef.stats.maxHP) {
            playerRef.stats.currentHP = playerRef.stats.maxHP;
        }
        int actual = playerRef.stats.currentHP - before;
        std::cout << "Used " << item.name << " - restored " << actual << " HP!\n";
        hasEffect = true;
    }
    
    // Temporary buff
    if (item.duration > 0 && (item.attackBonus != 0 || item.defenseBonus != 0 ||
                              item.critBonus != 0 || item.dodgeBonus != 0 ||
                              item.speedBonus != 0)) {
        StatusEffect buff;
        buff.name = item.name + " Buff";
        buff.attackMod = item.attackBonus;
        buff.defenseMod = item.defenseBonus;
        buff.critMod = item.critBonus;
        buff.dodgeMod = item.dodgeBonus;
        buff.speedMod = item.speedBonus;
        buff.turnsRemaining = item.duration;
        buff.isDebuff = false;
        
        playerRef.activeEffects.push_back(buff);
        std::cout << "Gained temporary buff from " << item.name << "!\n";
        hasEffect = true;
    }
    
    return hasEffect;
}

Item InventorySystem::dropItem(int itemIndex) {
    Item dropped;
    
    if (itemIndex < 0 || itemIndex >= static_cast<int>(playerRef.inventory.size())) {
        return dropped;
    }
    
    dropped = playerRef.inventory[itemIndex];
    
    if (dropped.isEquipped) {
        unequipItem(itemIndex);
    }
    
    playerRef.inventory.erase(playerRef.inventory.begin() + itemIndex);
    
    return dropped;
}

// ---
// ECONOMY
// ---

int InventorySystem::calculateTotalValue() const {
    int total = 0;
    for (const Item& item : playerRef.inventory) {
        total += item.value * item.quantity;
    }
    return total;
}

int InventorySystem::getSellPrice(int itemIndex) const {
    if (itemIndex < 0 || itemIndex >= static_cast<int>(playerRef.inventory.size())) {
        return 0;
    }
    return calculateSellValue(playerRef.inventory[itemIndex]);
}

std::function<bool(const Item&, const Item&)> InventorySystem::getComparator(SortType sortType) const {
    switch (sortType) {
        case SortType::NAME_ASC:
            return [](const Item& a, const Item& b) { 
                return toLower(a.name) < toLower(b.name); 
            };
        case SortType::NAME_DESC:
            return [](const Item& a, const Item& b) { 
                return toLower(a.name) > toLower(b.name); 
            };
        case SortType::VALUE_ASC:
            return [](const Item& a, const Item& b) { return a.value < b.value; };
        case SortType::VALUE_DESC:
            return [](const Item& a, const Item& b) { return a.value > b.value; };
        case SortType::RARITY_ASC:
            return [](const Item& a, const Item& b) { 
                return static_cast<int>(a.rarity) < static_cast<int>(b.rarity); 
            };
        case SortType::RARITY_DESC:
            return [](const Item& a, const Item& b) { 
                return static_cast<int>(a.rarity) > static_cast<int>(b.rarity); 
            };
        case SortType::TYPE:
            return [](const Item& a, const Item& b) { 
                return static_cast<int>(a.type) < static_cast<int>(b.type); 
            };
        default:
            return [](const Item&, const Item&) { return false; };
    }
}

// ---
// STANDALONE FUNCTIONS
// ---

std::vector<Item> createStartingItems(CharacterClass charClass) {
    std::vector<Item> items;
    
    // Common starting items
    Item healthPotion;
    healthPotion.id = 1;
    healthPotion.name = "Minor Health Potion";
    healthPotion.description = "Restores 25 HP.";
    healthPotion.type = ItemType::CONSUMABLE;
    healthPotion.rarity = ItemRarity::COMMON;
    healthPotion.healAmount = 25;
    healthPotion.quantity = 3;
    healthPotion.value = 10;
    healthPotion.isUsable = true;
    healthPotion.isStackable = true;
    items.push_back(healthPotion);
    
    // Class-specific starting weapon
    Item weapon;
    weapon.id = 100;
    weapon.type = ItemType::WEAPON;
    weapon.rarity = ItemRarity::COMMON;
    weapon.isUsable = false;
    weapon.isStackable = false;
    weapon.quantity = 1;
    
    switch (charClass) {
        case CharacterClass::WARRIOR:
            weapon.name = "Iron Sword";
            weapon.description = "A sturdy iron sword.";
            weapon.attackBonus = 5;
            weapon.value = 25;
            break;
        case CharacterClass::MAGE:
            weapon.name = "Apprentice Staff";
            weapon.description = "A basic magical staff.";
            weapon.attackBonus = 3;
            weapon.critBonus = 5;
            weapon.value = 30;
            break;
        case CharacterClass::ROGUE:
            weapon.name = "Steel Dagger";
            weapon.description = "A quick and sharp dagger.";
            weapon.attackBonus = 3;
            weapon.critBonus = 10;
            weapon.speedBonus = 2;
            weapon.value = 20;
            break;
        case CharacterClass::PALADIN:
            weapon.name = "Holy Mace";
            weapon.description = "A blessed mace.";
            weapon.attackBonus = 4;
            weapon.defenseBonus = 2;
            weapon.value = 35;
            break;
        default:
            weapon.name = "Traveler's Blade";
            weapon.description = "A dependable blade for uncertain roads.";
            weapon.attackBonus = 3;
            weapon.value = 20;
            break;
    }
    
    items.push_back(weapon);
    
    return items;
}

Item generateRandomItem(ItemRarity rarity, int targetLevel) {
    Item item;
    item.id = randomInt(1000, 9999);
    item.rarity = rarity;
    item.quantity = 1;
    
    // Random type
    int typeRoll = randomInt(1, 100);
    if (typeRoll <= 40) {
        item.type = ItemType::CONSUMABLE;
    } else if (typeRoll <= 60) {
        item.type = ItemType::WEAPON;
    } else if (typeRoll <= 80) {
        item.type = ItemType::ARMOR;
    } else {
        item.type = ItemType::ACCESSORY;
    }
    
    // Generate stats based on rarity
    int statMult = static_cast<int>(rarity) + 1;
    
    switch (item.type) {
        case ItemType::CONSUMABLE:
            item.name = "Health Potion";
            item.healAmount = 20 * statMult;
            item.value = 10 * statMult;
            item.isUsable = true;
            item.isStackable = true;
            break;
        case ItemType::WEAPON:
            item.name = "Sword";
            item.attackBonus = 3 * statMult;
            item.critBonus = statMult;
            item.value = 25 * statMult;
            item.isUsable = false;
            item.isStackable = false;
            break;
        case ItemType::ARMOR:
            item.name = "Armor";
            item.defenseBonus = 3 * statMult;
            item.value = 25 * statMult;
            item.isUsable = false;
            item.isStackable = false;
            break;
        case ItemType::ACCESSORY:
            item.name = "Ring";
            item.critBonus = 2 * statMult;
            item.dodgeBonus = statMult;
            item.value = 30 * statMult;
            item.isUsable = false;
            item.isStackable = false;
            break;
        default:
            break;
    }
    
    // Add rarity prefix
    switch (rarity) {
        case ItemRarity::UNCOMMON:
            item.name = "Quality " + item.name;
            break;
        case ItemRarity::RARE:
            item.name = "Superior " + item.name;
            break;
        case ItemRarity::EPIC:
            item.name = "Epic " + item.name;
            break;
        case ItemRarity::LEGENDARY:
            item.name = "Legendary " + item.name;
            break;
        default:
            break;
    }

    const int contextLevel = std::max(1, targetLevel);
    const int rolledItemLevel = rollItemLevel(contextLevel, rarity, false);
    applyItemLevelScaling(item, rolledItemLevel, true);
    
    return item;
}

Item generateItemFromTemplate(const Item& templateItem, int targetLevel, bool rollPerk) {
    Item item = templateItem;
    
    // Add some randomization
    item.id = randomInt(10000, 99999);
    item.value += randomInt(-5, 10);
    if (item.value < 1) item.value = 1;

    const int contextLevel = std::max(1, targetLevel);
    const int rolledItemLevel = rollItemLevel(contextLevel, item.rarity, false);
    applyItemLevelScaling(item, rolledItemLevel, rollPerk);
    
    // Small stat variance
    if (item.attackBonus > 0) item.attackBonus += randomInt(-1, 2);
    if (item.defenseBonus > 0) item.defenseBonus += randomInt(-1, 2);
    if (item.healAmount > 0) item.healAmount += randomInt(0, 4);

    if (item.value < 1) item.value = 1;
    
    return item;
}

bool canUseItem(const Item& item, bool inCombat) {
    if (!item.isUsable) return false;
    
    switch (item.type) {
        case ItemType::CONSUMABLE:
            return true;  // Always usable
        case ItemType::KEY:
        case ItemType::QUEST:
            return !inCombat;  // Only out of combat
        default:
            return false;
    }
}

int getRarityColor(ItemRarity rarity) {
    switch (rarity) {
        case ItemRarity::COMMON:    return 37;  // White
        case ItemRarity::UNCOMMON:  return 32;  // Green
        case ItemRarity::RARE:      return 34;  // Blue
        case ItemRarity::EPIC:      return 35;  // Purple
        case ItemRarity::LEGENDARY: return 33;  // Yellow/Gold
        default:                    return 37;
    }
}

std::string formatItemName(const Item& item) {
    std::stringstream ss;
    ss << "\033[" << getRarityColor(item.rarity) << "m"
       << item.name << "\033[0m";
    return ss.str();
}

}  // namespace AQE
