//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Crafting system implementation (partially implemented - many features scrapped)

#include "../include/Crafting.h"
#include "../include/Utils.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cstdlib>

namespace AQE {

namespace {

void applyAccessoryStats(Item& item) {
    if (item.type != ItemType::ACCESSORY) {
        return;
    }

    item.isUsable = false;
    item.isStackable = false;

    const std::string lowerName = toLower(item.name);

    if (lowerName.find("swiftness") != std::string::npos) {
        item.speedBonus = 6;
        item.dodgeBonus = 6;
        return;
    }

    if (lowerName.find("fortune") != std::string::npos) {
        item.critBonus = 18;
        item.attackBonus = 2;
        return;
    }

    if (lowerName.find("might") != std::string::npos) {
        item.attackBonus = 8;
        item.critBonus = 3;
        return;
    }

    if (lowerName.find("protection") != std::string::npos) {
        item.defenseBonus = 6;
        item.dodgeBonus = 2;
        return;
    }

    if (lowerName.find("vitality") != std::string::npos) {
        item.defenseBonus = 5;
        item.dodgeBonus = 3;
        return;
    }

    if (lowerName.find("shadows") != std::string::npos) {
        item.dodgeBonus = 14;
        item.speedBonus = 4;
        item.defenseBonus = 2;
        return;
    }

    if (lowerName.find("dragon") != std::string::npos) {
        item.attackBonus = 12;
        item.defenseBonus = 8;
        item.critBonus = 12;
        item.speedBonus = -1;
        return;
    }

    if (lowerName.find("iron skin") != std::string::npos) {
        item.defenseBonus = 7;
        item.speedBonus = -2;
        return;
    }

    if (lowerName.find("assassin") != std::string::npos) {
        item.attackBonus = 4;
        item.critBonus = 12;
        item.speedBonus = 5;
        return;
    }

    if (lowerName.find("wisdom") != std::string::npos) {
        item.attackBonus = 5;
        item.defenseBonus = 5;
        item.critBonus = 5;
        return;
    }

    if (lowerName.find("pendant") != std::string::npos ||
        lowerName.find("necklace") != std::string::npos) {
        item.defenseBonus = 4;
        item.dodgeBonus = 4;
        item.critBonus = 2;
        return;
    }

    if (lowerName.find("amulet") != std::string::npos ||
        lowerName.find("charm") != std::string::npos ||
        lowerName.find("talisman") != std::string::npos) {
        item.defenseBonus = 5;
        item.dodgeBonus = 3;
        item.critBonus = 2;
        return;
    }

    if (lowerName.find("cloak") != std::string::npos) {
        item.dodgeBonus = 10;
        item.speedBonus = 5;
        item.defenseBonus = 2;
        return;
    }

    item.defenseBonus = 3;
    item.critBonus = 2;
    item.dodgeBonus = 2;
}

void configureVendorTemplateItem(Item& item, int dungeonLevel) {
    if (item.itemLevel <= 0) {
        int rarityOffset = 0;
        switch (item.rarity) {
            case ItemRarity::COMMON: rarityOffset = 0; break;
            case ItemRarity::UNCOMMON: rarityOffset = 2; break;
            case ItemRarity::RARE: rarityOffset = 4; break;
            case ItemRarity::EPIC: rarityOffset = 6; break;
            case ItemRarity::LEGENDARY: rarityOffset = 8; break;
            default: break;
        }
        item.itemLevel = std::max(1, dungeonLevel + rarityOffset);
    }

    rebuildItemStatsFromLevel(item);
}

}  // namespace

// ---
// CraftingSystem Implementation
// ---

CraftingSystem::CraftingSystem(Player& player) : playerRef(player) {
    initializeMaterials();
    initializeRecipes();
}

void CraftingSystem::addMaterial(int materialId, int quantity) {
    auto it = materials.find(materialId);
    if (it != materials.end()) {
        it->second.quantity += quantity;
        std::cout << "Collected " << quantity << " x " << it->second.name << ".\n";
    } else {
        CraftingMaterial mat;
        mat.id = materialId;
        mat.name = "Unknown Material";
        mat.quantity = quantity;
        materials[materialId] = mat;
        std::cout << "Collected " << quantity << " crafting materials.\n";
    }
}

int CraftingSystem::getMaterialCount(int materialId) const {
    auto it = materials.find(materialId);
    if (it != materials.end()) {
        return it->second.quantity;
    }
    return 0;
}

void CraftingSystem::displayMaterials() const {
    std::cout << "\n========================================\n";
    std::cout << "         CRAFTING MATERIALS\n";
    std::cout << "========================================\n\n";
    
    if (materials.empty()) {
        std::cout << "You have no crafting materials.\n";
        return;
    }
    
    for (const auto& pair : materials) {
        const CraftingMaterial& mat = pair.second;
        if (mat.quantity > 0) {
            std::cout << "[" << mat.id << "] " << mat.name;
            std::cout << " x" << mat.quantity << "\n";
        }
    }
}

bool CraftingSystem::hasMaterials(const std::map<int, int>& required) const {
    for (const auto& req : required) {
        if (getMaterialCount(req.first) < req.second) {
            return false;
        }
    }
    return true;
}

void CraftingSystem::consumeMaterials(const std::map<int, int>& required) {
    for (const auto& req : required) {
        if (materials.find(req.first) != materials.end()) {
            materials[req.first].quantity -= req.second;
        }
    }
}

void CraftingSystem::displayRecipes() const {
    std::cout << "\n========================================\n";
    std::cout << "           ALL RECIPES\n";
    std::cout << "========================================\n\n";
    
    for (const auto& recipe : recipes) {
        std::cout << "[" << recipe.id << "] " << recipe.name;
        if (!recipe.isDiscovered) {
            std::cout << " (UNDISCOVERED)";
        }
        std::cout << "\n  " << recipe.description;
        std::cout << " | Level " << recipe.requiredLevel;
        std::cout << " | Gold: " << recipe.requiredGold << "\n";
    }
}

void CraftingSystem::displayDiscoveredRecipes() const {
    std::cout << "\n========================================\n";
    std::cout << "        DISCOVERED RECIPES\n";
    std::cout << "========================================\n\n";
    
    bool hasRecipes = false;
    for (const auto& recipe : recipes) {
        if (recipe.isDiscovered) {
            hasRecipes = true;
            std::cout << "[" << recipe.id << "] " << recipe.name << "\n";
            std::cout << "  " << recipe.description << "\n";
            std::cout << "  Station: " << recipe.craftingStation;
            std::cout << " | Level: " << recipe.requiredLevel;
            std::cout << " | Gold: " << recipe.requiredGold << "\n";
            std::cout << "  Required Materials: ";
            bool first = true;
            for (const auto& req : recipe.requiredMaterials) {
                if (!first) {
                    std::cout << ", ";
                }
                const auto matIt = materials.find(req.first);
                if (matIt != materials.end()) {
                    std::cout << matIt->second.name << " x" << req.second;
                } else {
                    std::cout << "Material " << req.first << " x" << req.second;
                }
                first = false;
            }
            if (recipe.requiredMaterials.empty()) {
                std::cout << "None";
            }
            std::cout << "\n";
            std::cout << "  Success Chance: " << static_cast<int>(calculateSuccessChance(recipe) * 100.0f) << "%\n";
            std::cout << "  Can craft: " << (canCraft(recipe.id) ? "YES" : "NO") << "\n\n";
        }
    }
    
    if (!hasRecipes) {
        std::cout << "No recipes discovered yet.\n";
    }
}

bool CraftingSystem::craftItem(int recipeId) {
    Recipe* recipe = findRecipe(recipeId);
    if (recipe == nullptr) {
        std::cout << "Recipe not found!\n";
        return false;
    }
    
    if (!recipe->isDiscovered) {
        std::cout << "You haven't discovered this recipe yet!\n";
        return false;
    }
    
    if (!canCraft(recipeId)) {
        std::cout << "Cannot craft this item - missing requirements!\n";
        return false;
    }
    
    // Calculate success chance
    float successChance = calculateSuccessChance(*recipe);
    float roll = static_cast<float>(rand() % 100) / 100.0f;
    
    // Consume materials regardless
    consumeMaterials(recipe->requiredMaterials);
    playerRef.gold -= recipe->requiredGold;
    
    if (roll > successChance) {
        std::cout << "Crafting failed! Materials lost.\n";
        return false;
    }
    
    // Create the item
    Item craftedItem;
    // In a real system, we'd fetch the template for resultItemId
    // For now, we'll synthesize it from the recipe
    craftedItem.id = recipe->resultItemId;
    craftedItem.name = recipe->name;
    craftedItem.description = recipe->description;
    craftedItem.quantity = recipe->resultQuantity;
    craftedItem.value = recipe->requiredGold * 3;
    craftedItem.durability = 100;
    craftedItem.maxDurability = 100;
    
    // Set stats based on recipe family (keeps accessory effects meaningful)
    if (recipeId >= 1000 && recipeId < 1100) { // Weapons
        craftedItem.type = ItemType::WEAPON;
        craftedItem.isUsable = false;
        craftedItem.isStackable = false;
        craftedItem.attackBonus = 5 + (recipeId % 10) * 5;
    } else if (recipeId >= 1100 && recipeId < 1200) { // Armor
        craftedItem.type = ItemType::ARMOR;
        craftedItem.isUsable = false;
        craftedItem.isStackable = false;
        craftedItem.defenseBonus = 3 + (recipeId % 10) * 4;
    } else if (recipeId >= 1200 && recipeId < 1300) { // Accessories
        craftedItem.type = ItemType::ACCESSORY;
        applyAccessoryStats(craftedItem);
    } else { // Consumables
        craftedItem.type = ItemType::CONSUMABLE;
        craftedItem.isUsable = true;
        craftedItem.isStackable = true;
        craftedItem.healAmount = 20 + (recipeId % 10) * 10;
    }

    if (recipe->requiredLevel >= 16) {
        craftedItem.rarity = ItemRarity::LEGENDARY;
    } else if (recipe->requiredLevel >= 12) {
        craftedItem.rarity = ItemRarity::EPIC;
    } else if (recipe->requiredLevel >= 8) {
        craftedItem.rarity = ItemRarity::RARE;
    } else if (recipe->requiredLevel >= 4) {
        craftedItem.rarity = ItemRarity::UNCOMMON;
    } else {
        craftedItem.rarity = ItemRarity::COMMON;
    }

    bool stacked = false;
    if (craftedItem.isStackable) {
        for (Item& invItem : playerRef.inventory) {
            if (invItem.id == craftedItem.id) {
                invItem.quantity += craftedItem.quantity;
                stacked = true;
                break;
            }
        }
    }

    if (!stacked) {
        playerRef.inventory.push_back(craftedItem);
    }

    playerRef.experience += recipe->experienceGained;
    
    std::cout << "Successfully crafted: " << recipe->name;
    if (craftedItem.quantity > 1) {
        std::cout << " x" << craftedItem.quantity;
    }
    std::cout << "!\n";
    std::cout << "Gained " << recipe->experienceGained << " XP.\n";
    
    return true;
}

bool CraftingSystem::canCraft(int recipeId) const {
    const Recipe* recipe = nullptr;
    for (const auto& r : recipes) {
        if (r.id == recipeId) {
            recipe = &r;
            break;
        }
    }
    
    if (recipe == nullptr || !recipe->isDiscovered) {
        return false;
    }
    
    // Check level requirement
    if (playerRef.level < recipe->requiredLevel) {
        return false;
    }
    
    // Check gold
    if (playerRef.gold < recipe->requiredGold) {
        return false;
    }
    
    // Check materials
    if (!hasMaterials(recipe->requiredMaterials)) {
        return false;
    }
    
    return true;
}

void CraftingSystem::discoverRecipe(int recipeId) {
    Recipe* recipe = findRecipe(recipeId);
    if (recipe != nullptr && !recipe->isDiscovered) {
        recipe->isDiscovered = true;
        std::cout << "New recipe discovered: " << recipe->name << "!\n";
    }
}

void CraftingSystem::learnRecipeFromBook(int recipeBookId) {
    // Recipe books have IDs starting from 2000
    int recipeId = recipeBookId - 2000;
    discoverRecipe(recipeId);
}

bool CraftingSystem::enchantItem(int itemId, EnchantmentType enchantType) {
    // Find item in inventory
    Item* item = nullptr;
    for (auto& invItem : playerRef.inventory) {
        if (invItem.id == itemId) {
            item = &invItem;
            break;
        }
    }
    
    if (item == nullptr) {
        std::cout << "Item not found!\n";
        return false;
    }

    if (item->isEnchanted) {
        std::cout << "Item is already enchanted!\n";
        return false;
    }
    
    int cost = getEnchantCost(enchantType, 1);
    if (playerRef.gold < cost) {
        std::cout << "Not enough gold! Need " << cost << "\n";
        return false;
    }

    // Check for required essence (Material ID 6)
    if (getMaterialCount(6) < 2) {
        std::cout << "Need 2 Magical Essence to enchant!\n";
        return false;
    }
    
    playerRef.gold -= cost;
    consumeMaterials({{6, 2}});
    
    item->isEnchanted = true;
    
    switch (enchantType) {
        case EnchantmentType::FIRE_DAMAGE:
            item->enchantmentName = "Fiery";
            item->attackBonus += 10;
            break;
        case EnchantmentType::ICE_DAMAGE:
            item->enchantmentName = "Frozen";
            item->attackBonus += 5;
            item->speedBonus -= 2; // Slow effect
            break;
        case EnchantmentType::LIFESTEAL:
            item->enchantmentName = "Vampiric";
            item->healAmount += 5;
            break;
        case EnchantmentType::CRITICAL_STRIKE:
            item->enchantmentName = "Precise";
            item->critBonus += 10;
            break;
        case EnchantmentType::SHARPNESS:
            item->enchantmentName = "Sharp";
            item->attackBonus += 15;
            break;
        case EnchantmentType::PROTECTION:
            item->enchantmentName = "Warded";
            item->defenseBonus += 10;
            break;
        default:
            item->enchantmentName = "Enchanted";
            item->value += 50;
    }
    
    item->name = item->enchantmentName + " " + item->name;
    item->value += cost * 2;
    
    std::cout << "Successfully enchanted: " << item->name << "!\n";
    return true;
}

bool CraftingSystem::upgradeItem(int itemId) {
    for (auto& item : playerRef.inventory) {
        if (item.id == itemId) {
            if (item.upgradeLevel >= 10) {
                std::cout << "Item is already at maximum upgrade level (+10)!\n";
                return false;
            }

            int upgradeCost = (item.upgradeLevel + 1) * 100;
            if (playerRef.gold < upgradeCost) {
                std::cout << "Not enough gold! Need " << upgradeCost << "\n";
                return false;
            }

            // Require Metal (ID 1) for weapons/armor
            if (getMaterialCount(1) < (item.upgradeLevel + 1) * 2) {
                std::cout << "Not enough Iron Ore!\n";
                return false;
            }
            
            playerRef.gold -= upgradeCost;
            std::map<int, int> materials_needed;
            materials_needed[1] = (item.upgradeLevel + 1) * 2;
            consumeMaterials(materials_needed);
            
            item.upgradeLevel++;
            if (item.type == ItemType::WEAPON) item.attackBonus += 5;
            if (item.type == ItemType::ARMOR) item.defenseBonus += 3;
            
            // Update name
            size_t plusPos = item.name.find(" +");
            if (plusPos != std::string::npos) {
                item.name = item.name.substr(0, plusPos);
            }
            item.name += " +" + std::to_string(item.upgradeLevel);
            item.value += upgradeCost;
            
            std::cout << "Item upgraded to: " << item.name << "!\n";
            return true;
        }
    }
    
    std::cout << "Item not found!\n";
    return false;
}

bool CraftingSystem::repairItem(int itemId) {
    for (auto& item : playerRef.inventory) {
        if (item.id == itemId) {
            if (item.durability >= item.maxDurability) {
                std::cout << "Item is already in perfect condition.\n";
                return false;
            }

            int repairCost = (item.maxDurability - item.durability) * 2;
            if (playerRef.gold < repairCost) {
                std::cout << "Not enough gold to repair!\n";
                return false;
            }

            playerRef.gold -= repairCost;
            item.durability = item.maxDurability;
            std::cout << "Item repaired: " << item.name << "\n";
            return true;
        }
    }
    return false;
}

int CraftingSystem::getEnchantCost(EnchantmentType type, int level) const {
    int baseCost = 250;
    switch (type) {
        case EnchantmentType::SHARPNESS: baseCost = 500; break;
        case EnchantmentType::LIFESTEAL: baseCost = 750; break;
        default: baseCost = 250;
    }
    return baseCost * level;
}

void CraftingSystem::displayEnchantableItems() const {
    std::cout << "\n========================================\n";
    std::cout << "       ENCHANTABLE / UPGRADABLE ITEMS\n";
    std::cout << "========================================\n\n";
    
    for (size_t i = 0; i < playerRef.inventory.size(); ++i) {
        const auto& item = playerRef.inventory[i];
        if (item.type == ItemType::WEAPON || item.type == ItemType::ARMOR ||
            item.type == ItemType::ACCESSORY) {
            std::cout << "[" << (i + 1) << "] " << item.name;
            if (item.isEnchanted) std::cout << " (Enchanted: " << item.enchantmentName << ")";
            std::cout << " | iLvl " << item.itemLevel << " ("
                      << itemLevelTierToString(item.itemLevel) << ")";
            if (item.hasUniquePerk && !item.uniquePerkName.empty()) {
                std::cout << " | Perk: " << item.uniquePerkName;
            }
            std::cout << " | Durability: " << item.durability << "/" << item.maxDurability << "\n";
        }
    }
}

bool CraftingSystem::reforgeItem(int itemId, int targetItemLevel) {
    for (auto& item : playerRef.inventory) {
        if (item.id != itemId) {
            continue;
        }

        if (item.type != ItemType::WEAPON && item.type != ItemType::ARMOR &&
            item.type != ItemType::ACCESSORY) {
            std::cout << "Only equipment can be reforged.\n";
            return false;
        }

        if (item.rarity != ItemRarity::LEGENDARY) {
            std::cout << "Reforging is reserved for legendary gear.\n";
            return false;
        }

        const int currentItemLevel = std::max(1, item.itemLevel);
        if (targetItemLevel <= currentItemLevel) {
            std::cout << "Target iLvl must be higher than current iLvl.\n";
            return false;
        }

        const int cappedTarget = std::min(targetItemLevel, currentItemLevel + 25);
        const int levelDelta = cappedTarget - currentItemLevel;
        const int goldCost = (levelDelta * levelDelta * 180) +
                             ((item.upgradeLevel + 1) * 400);
        const int essenceCost = std::max(4, (levelDelta / 2) + 2);
        const int dragonScaleCost = std::max(1, (levelDelta / 8) +
            ((cappedTarget >= 30) ? 1 : 0));
        const int crystalCost = std::max(2, (levelDelta / 4) + 1);

        if (playerRef.gold < goldCost) {
            std::cout << "Not enough gold! Need " << goldCost << "\n";
            return false;
        }

        if (getMaterialCount(6) < essenceCost ||
            getMaterialCount(9) < dragonScaleCost ||
            getMaterialCount(10) < crystalCost) {
            std::cout << "Missing reforging materials. Need: "
                      << essenceCost << " Essence, "
                      << dragonScaleCost << " Dragon Scale, "
                      << crystalCost << " Magic Crystal.\n";
            return false;
        }

        playerRef.gold -= goldCost;
        consumeMaterials({
            {6, essenceCost},
            {9, dragonScaleCost},
            {10, crystalCost}
        });

        applyItemLevelScaling(item, cappedTarget, true);
        item.value += goldCost / 2;

        if (item.isEquipped) {
            refreshAverageItemLevel(playerRef);
        }

        std::cout << "Reforged " << item.name << " to iLvl " << item.itemLevel
                  << " (" << itemLevelTierToString(item.itemLevel) << ")!\n";
        if (item.hasUniquePerk && !item.uniquePerkName.empty()) {
            std::cout << "New perk resonance: " << item.uniquePerkName << "\n";
        }
        return true;
    }

    std::cout << "Item not found!\n";
    return false;
}

std::vector<CraftingMaterial> CraftingSystem::disenchantItem(int itemId) {
    std::vector<CraftingMaterial> result;
    for (auto it = playerRef.inventory.begin(); it != playerRef.inventory.end(); ++it) {
        if (it->id == itemId) {
            std::cout << "Disenchanting " << it->name << "...\n";
            
            int matCount = 1 + static_cast<int>(it->rarity);
            addMaterial(1, matCount * 2); // Iron
            addMaterial(2, matCount);     // Wood
            
            if (it->isEnchanted) {
                addMaterial(6, 1); // Essence
            }

            playerRef.inventory.erase(it);
            break;
        }
    }
    return result;
}

bool CraftingSystem::convertMaterial(int fromId, int toId, int quantity) {
    if (getMaterialCount(fromId) < quantity) {
        std::cout << "Not enough materials to convert!\n";
        return false;
    }

    if (quantity < 5) {
        std::cout << "Minimum 5 materials required for conversion.\n";
        return false;
    }

    consumeMaterials({{fromId, 5}});
    addMaterial(toId, 1);
    std::cout << "Converted 5 materials into 1 higher-tier material.\n";
    return true;
}

void CraftingSystem::runCraftingMenu() {
    bool inMenu = true;
    while (inMenu) {
        std::cout << "\n========================================\n";
        std::cout << "        ADVANCED CRAFTING SYSTEM\n";
        std::cout << "========================================\n";
        std::cout << "1. View Materials\n";
        std::cout << "2. View Recipes & Craft\n";
        std::cout << "3. Upgrade Item (+Stats)\n";
        std::cout << "4. Enchant Item (Magic)\n";
        std::cout << "5. Repair Item\n";
        std::cout << "6. Disenchant Item (Break down)\n";
        std::cout << "7. Material Conversion\n";
        std::cout << "8. Reforge Legendary (Raise iLvl)\n";
        std::cout << "0. Exit Crafting\n";

        int choice = getValidatedInt("Choice: ", 0, 8);

        switch (choice) {
            case 1:
                displayMaterials();
                break;
            case 2:
                displayDiscoveredRecipes();
                {
                int rid = getValidatedInt("Enter Recipe ID to craft (0 to cancel): ", 0, 99999);
                if (rid != 0) craftItem(rid);
                }
                break;
            case 3:
                displayEnchantableItems();
                {
                int uidx = getValidatedInt("Enter Item Index to upgrade (0 to cancel): ", 0,
                                           static_cast<int>(playerRef.inventory.size()));
                if (uidx > 0 && uidx <= static_cast<int>(playerRef.inventory.size())) {
                    upgradeItem(playerRef.inventory[uidx - 1].id);
                }
                }
                break;
            case 4:
                displayEnchantableItems();
                {
                int eidx = getValidatedInt("Enter Item Index to enchant (0 to cancel): ", 0,
                                           static_cast<int>(playerRef.inventory.size()));
                if (eidx > 0 && eidx <= static_cast<int>(playerRef.inventory.size())) {
                    std::cout << "Select Enchantment:\n";
                    std::cout << "1. Fire (+Attack)\n2. Ice (+Slow)\n3. Sharpness (++Attack)\n4. Protection (+Defense)\n";
                    int etype = getValidatedInt("Choose enchantment: ", 1, 4);
                    EnchantmentType t = EnchantmentType::SHARPNESS;
                    if (etype == 1) t = EnchantmentType::FIRE_DAMAGE;
                    else if (etype == 2) t = EnchantmentType::ICE_DAMAGE;
                    else if (etype == 4) t = EnchantmentType::PROTECTION;
                    enchantItem(playerRef.inventory[eidx - 1].id, t);
                }
                }
                break;
            case 5:
                displayEnchantableItems();
                {
                int ridx = getValidatedInt("Enter Item Index to repair (0 to cancel): ", 0,
                                           static_cast<int>(playerRef.inventory.size()));
                if (ridx > 0 && ridx <= static_cast<int>(playerRef.inventory.size())) {
                    repairItem(playerRef.inventory[ridx - 1].id);
                }
                }
                break;
            case 6:
                displayEnchantableItems();
                {
                int didx = getValidatedInt("Enter Item Index to disenchant (0 to cancel): ", 0,
                                           static_cast<int>(playerRef.inventory.size()));
                if (didx > 0 && didx <= static_cast<int>(playerRef.inventory.size())) {
                    disenchantItem(playerRef.inventory[didx - 1].id);
                }
                }
                break;
            case 7:
                if (getYesNo("Convert 5 Iron Ore into 1 Magical Essence")) {
                    convertMaterial(1, 6, 5);
                }
                break;
            case 8:
                displayEnchantableItems();
                {
                int fidx = getValidatedInt("Enter Item Index to reforge (0 to cancel): ", 0,
                                           static_cast<int>(playerRef.inventory.size()));
                if (fidx > 0 && fidx <= static_cast<int>(playerRef.inventory.size())) {
                    const Item& selected = playerRef.inventory[fidx - 1];
                    int minTarget = std::max(2, selected.itemLevel + 1);
                    int maxTarget = std::max(minTarget, selected.itemLevel + 20);
                    std::cout << "Reforge cost scales heavily with target iLvl.\n";
                    int target = getValidatedInt("Enter target iLvl: ", minTarget, maxTarget);
                    reforgeItem(selected.id, target);
                }
                }
                break;
            case 0:
                inMenu = false;
                break;
            default:
                std::cout << "Invalid choice.\n";
        }
    }
}

std::string CraftingSystem::serializeCrafting() const {
    std::stringstream ss;
    for (const auto& pair : materials) {
        ss << pair.first << ":" << pair.second.quantity << ",";
    }
    ss << "|";
    for (const auto& recipe : recipes) {
        if (recipe.isDiscovered) {
            ss << recipe.id << ",";
        }
    }
    return ss.str();
}

void CraftingSystem::deserializeCrafting(const std::string& data) {
    if (data.empty()) return;
    
    size_t pipePos = data.find('|');
    if (pipePos == std::string::npos) return;
    
    // Parse materials
    std::string matData = data.substr(0, pipePos);
    std::stringstream matSs(matData);
    std::string token;
    
    while (std::getline(matSs, token, ',')) {
        if (token.empty()) continue;
        size_t colonPos = token.find(':');
        if (colonPos != std::string::npos) {
            int id = std::stoi(token.substr(0, colonPos));
            int qty = std::stoi(token.substr(colonPos + 1));
            if (materials.find(id) != materials.end()) {
                materials[id].quantity = qty;
            }
        }
    }
    
    // Parse discovered recipes
    std::string recipeData = data.substr(pipePos + 1);
    std::stringstream recipeSs(recipeData);
    
    while (std::getline(recipeSs, token, ',')) {
        if (token.empty()) continue;
        int recipeId = std::stoi(token);
        Recipe* recipe = findRecipe(recipeId);
        if (recipe) {
            recipe->isDiscovered = true;
        }
    }
}

void CraftingSystem::initializeRecipes() {
    // ==================== WEAPON RECIPES ====================
    
    // Swords
    Recipe ironSword;
    ironSword.id = 1001;
    ironSword.name = "Iron Sword";
    ironSword.description = "A basic iron sword";
    ironSword.resultItemId = 10;
    ironSword.resultQuantity = 1;
    ironSword.requiredMaterials = {{1, 5}, {2, 3}};  // 5 iron, 3 wood
    ironSword.requiredGold = 50;
    ironSword.requiredLevel = 1;
    ironSword.isDiscovered = true;
    ironSword.craftingStation = "Forge";
    ironSword.successChance = 0.9f;
    ironSword.experienceGained = 20;
    recipes.push_back(ironSword);
    
    Recipe bronzeSword;
    bronzeSword.id = 1002;
    bronzeSword.name = "Bronze Sword";
    bronzeSword.description = "A basic bronze blade";
    bronzeSword.resultItemId = 15;
    bronzeSword.resultQuantity = 1;
    bronzeSword.requiredMaterials = {{1, 3}, {2, 2}};  // 3 iron, 2 wood
    bronzeSword.requiredGold = 25;
    bronzeSword.requiredLevel = 1;
    bronzeSword.isDiscovered = true;
    bronzeSword.craftingStation = "Forge";
    bronzeSword.successChance = 0.95f;
    bronzeSword.experienceGained = 15;
    recipes.push_back(bronzeSword);
    
    Recipe steelSword;
    steelSword.id = 1003;
    steelSword.name = "Steel Sword";
    steelSword.description = "A sturdy steel sword";
    steelSword.resultItemId = 16;
    steelSword.resultQuantity = 1;
    steelSword.requiredMaterials = {{1, 10}, {2, 5}};  // 10 iron, 5 wood
    steelSword.requiredGold = 150;
    steelSword.requiredLevel = 5;
    steelSword.isDiscovered = false;
    steelSword.craftingStation = "Forge";
    steelSword.successChance = 0.8f;
    steelSword.experienceGained = 50;
    recipes.push_back(steelSword);
    
    Recipe mithrilSword;
    mithrilSword.id = 1004;
    mithrilSword.name = "Mithril Sword";
    mithrilSword.description = "A lightweight superior blade";
    mithrilSword.resultItemId = 17;
    mithrilSword.resultQuantity = 1;
    mithrilSword.requiredMaterials = {{1, 15}, {5, 3}, {10, 1}};  // 15 iron, 3 gems, 1 crystal
    mithrilSword.requiredGold = 400;
    mithrilSword.requiredLevel = 10;
    mithrilSword.isDiscovered = false;
    mithrilSword.craftingStation = "Master Forge";
    mithrilSword.successChance = 0.7f;
    mithrilSword.experienceGained = 100;
    recipes.push_back(mithrilSword);
    
    Recipe flamingSword;
    flamingSword.id = 1005;
    flamingSword.name = "Flaming Longsword";
    flamingSword.description = "A blade wreathed in eternal flames";
    flamingSword.resultItemId = 18;
    flamingSword.resultQuantity = 1;
    flamingSword.requiredMaterials = {{1, 20}, {6, 5}, {10, 2}};  // 20 iron, 5 essence, 2 crystal
    flamingSword.requiredGold = 600;
    flamingSword.requiredLevel = 15;
    flamingSword.isDiscovered = false;
    flamingSword.craftingStation = "Master Forge";
    flamingSword.successChance = 0.6f;
    flamingSword.experienceGained = 150;
    recipes.push_back(flamingSword);
    
    // Daggers
    Recipe rustyDagger;
    rustyDagger.id = 1010;
    rustyDagger.name = "Rusty Dagger";
    rustyDagger.description = "A simple dagger";
    rustyDagger.resultItemId = 70;
    rustyDagger.resultQuantity = 1;
    rustyDagger.requiredMaterials = {{1, 2}, {2, 1}};
    rustyDagger.requiredGold = 15;
    rustyDagger.requiredLevel = 1;
    rustyDagger.isDiscovered = true;
    rustyDagger.craftingStation = "Forge";
    rustyDagger.successChance = 0.95f;
    rustyDagger.experienceGained = 10;
    recipes.push_back(rustyDagger);
    
    Recipe poisonDagger;
    poisonDagger.id = 1011;
    poisonDagger.name = "Poison Dagger";
    poisonDagger.description = "Coated with deadly venom";
    poisonDagger.resultItemId = 71;
    poisonDagger.resultQuantity = 1;
    poisonDagger.requiredMaterials = {{1, 5}, {7, 5}, {8, 2}};  // 5 iron, 5 herbs, 2 bones
    poisonDagger.requiredGold = 120;
    poisonDagger.requiredLevel = 6;
    poisonDagger.isDiscovered = false;
    poisonDagger.craftingStation = "Alchemy Lab";
    poisonDagger.successChance = 0.75f;
    poisonDagger.experienceGained = 60;
    recipes.push_back(poisonDagger);
    
    Recipe shadowDagger;
    shadowDagger.id = 1012;
    shadowDagger.name = "Shadow Fang";
    shadowDagger.description = "Forged in darkness";
    shadowDagger.resultItemId = 72;
    shadowDagger.resultQuantity = 1;
    shadowDagger.requiredMaterials = {{1, 8}, {6, 3}, {8, 3}};  // 8 iron, 3 essence, 3 bones
    shadowDagger.requiredGold = 280;
    shadowDagger.requiredLevel = 12;
    shadowDagger.isDiscovered = false;
    shadowDagger.craftingStation = "Master Forge";
    shadowDagger.successChance = 0.65f;
    shadowDagger.experienceGained = 90;
    recipes.push_back(shadowDagger);
    
    // Axes
    Recipe woodcutterAxe;
    woodcutterAxe.id = 1020;
    woodcutterAxe.name = "Woodcutter's Axe";
    woodcutterAxe.description = "A basic axe";
    woodcutterAxe.resultItemId = 80;
    woodcutterAxe.resultQuantity = 1;
    woodcutterAxe.requiredMaterials = {{1, 4}, {2, 4}};
    woodcutterAxe.requiredGold = 35;
    woodcutterAxe.requiredLevel = 1;
    woodcutterAxe.isDiscovered = true;
    woodcutterAxe.craftingStation = "Forge";
    woodcutterAxe.successChance = 0.9f;
    woodcutterAxe.experienceGained = 18;
    recipes.push_back(woodcutterAxe);
    
    Recipe warAxe;
    warAxe.id = 1021;
    warAxe.name = "War Axe";
    warAxe.description = "A brutal weapon designed for war";
    warAxe.resultItemId = 81;
    warAxe.resultQuantity = 1;
    warAxe.requiredMaterials = {{1, 12}, {2, 6}};
    warAxe.requiredGold = 140;
    warAxe.requiredLevel = 7;
    warAxe.isDiscovered = false;
    warAxe.craftingStation = "Forge";
    warAxe.successChance = 0.8f;
    warAxe.experienceGained = 65;
    recipes.push_back(warAxe);
    
    Recipe greatAxe;
    greatAxe.id = 1022;
    greatAxe.name = "Executioner's Greataxe";
    greatAxe.description = "A massive axe that cleaves through armor";
    greatAxe.resultItemId = 82;
    greatAxe.resultQuantity = 1;
    greatAxe.requiredMaterials = {{1, 18}, {2, 8}, {8, 4}};  // 18 iron, 8 wood, 4 bones
    greatAxe.requiredGold = 420;
    greatAxe.requiredLevel = 14;
    greatAxe.isDiscovered = false;
    greatAxe.craftingStation = "Master Forge";
    greatAxe.successChance = 0.65f;
    greatAxe.experienceGained = 120;
    recipes.push_back(greatAxe);
    
    // Spears
    Recipe woodenSpear;
    woodenSpear.id = 1030;
    woodenSpear.name = "Wooden Spear";
    woodenSpear.description = "A simple spear";
    woodenSpear.resultItemId = 85;
    woodenSpear.resultQuantity = 1;
    woodenSpear.requiredMaterials = {{2, 6}, {1, 2}};
    woodenSpear.requiredGold = 20;
    woodenSpear.requiredLevel = 1;
    woodenSpear.isDiscovered = true;
    woodenSpear.craftingStation = "Workbench";
    woodenSpear.successChance = 0.92f;
    woodenSpear.experienceGained = 12;
    recipes.push_back(woodenSpear);
    
    Recipe pike;
    pike.id = 1031;
    pike.name = "Steel Pike";
    pike.description = "A long pike for keeping enemies at bay";
    pike.resultItemId = 86;
    pike.resultQuantity = 1;
    pike.requiredMaterials = {{1, 8}, {2, 10}};
    pike.requiredGold = 100;
    pike.requiredLevel = 5;
    pike.isDiscovered = false;
    pike.craftingStation = "Forge";
    pike.successChance = 0.82f;
    pike.experienceGained = 48;
    recipes.push_back(pike);
    
    Recipe halberd;
    halberd.id = 1032;
    halberd.name = "Halberd";
    halberd.description = "A versatile polearm weapon";
    halberd.resultItemId = 87;
    halberd.resultQuantity = 1;
    halberd.requiredMaterials = {{1, 14}, {2, 12}, {3, 4}};
    halberd.requiredGold = 260;
    halberd.requiredLevel = 11;
    halberd.isDiscovered = false;
    halberd.craftingStation = "Forge";
    halberd.successChance = 0.72f;
    halberd.experienceGained = 85;
    recipes.push_back(halberd);
    
    // Staves
    Recipe apprenticeStaff;
    apprenticeStaff.id = 1040;
    apprenticeStaff.name = "Apprentice's Staff";
    apprenticeStaff.description = "A basic staff for novice mages";
    apprenticeStaff.resultItemId = 90;
    apprenticeStaff.resultQuantity = 1;
    apprenticeStaff.requiredMaterials = {{2, 8}, {5, 1}};  // 8 wood, 1 gem
    apprenticeStaff.requiredGold = 45;
    apprenticeStaff.requiredLevel = 1;
    apprenticeStaff.isDiscovered = true;
    apprenticeStaff.craftingStation = "Workbench";
    apprenticeStaff.successChance = 0.88f;
    apprenticeStaff.experienceGained = 22;
    recipes.push_back(apprenticeStaff);
    
    Recipe fireStaff;
    fireStaff.id = 1041;
    fireStaff.name = "Staff of Flames";
    fireStaff.description = "Channels the power of fire";
    fireStaff.resultItemId = 91;
    fireStaff.resultQuantity = 1;
    fireStaff.requiredMaterials = {{2, 12}, {5, 3}, {6, 2}};  // 12 wood, 3 gems, 2 essence
    fireStaff.requiredGold = 180;
    fireStaff.requiredLevel = 8;
    fireStaff.isDiscovered = false;
    fireStaff.craftingStation = "Alchemy Lab";
    fireStaff.successChance = 0.75f;
    fireStaff.experienceGained = 72;
    recipes.push_back(fireStaff);
    
    Recipe frostStaff;
    frostStaff.id = 1042;
    frostStaff.name = "Staff of Winter";
    frostStaff.description = "Freezes enemies with icy magic";
    frostStaff.resultItemId = 92;
    frostStaff.resultQuantity = 1;
    frostStaff.requiredMaterials = {{2, 15}, {5, 5}, {6, 4}, {10, 1}};
    frostStaff.requiredGold = 320;
    frostStaff.requiredLevel = 13;
    frostStaff.isDiscovered = false;
    frostStaff.craftingStation = "Master Alchemy Lab";
    frostStaff.successChance = 0.68f;
    frostStaff.experienceGained = 105;
    recipes.push_back(frostStaff);
    
    Recipe archmagusStaff;
    archmagusStaff.id = 1043;
    archmagusStaff.name = "Archmagus Staff";
    archmagusStaff.description = "The staff of a grand wizard";
    archmagusStaff.resultItemId = 93;
    archmagusStaff.resultQuantity = 1;
    archmagusStaff.requiredMaterials = {{2, 20}, {5, 8}, {6, 8}, {10, 3}};
    archmagusStaff.requiredGold = 650;
    archmagusStaff.requiredLevel = 18;
    archmagusStaff.isDiscovered = false;
    archmagusStaff.craftingStation = "Master Alchemy Lab";
    archmagusStaff.successChance = 0.55f;
    archmagusStaff.experienceGained = 180;
    recipes.push_back(archmagusStaff);
    
    // ==================== ARMOR RECIPES ====================
    
    // Light Armor
    Recipe clothRobe;
    clothRobe.id = 1100;
    clothRobe.name = "Cloth Robe";
    clothRobe.description = "Simple cloth offering minimal protection";
    clothRobe.resultItemId = 100;
    clothRobe.resultQuantity = 1;
    clothRobe.requiredMaterials = {{4, 5}};  // 5 cloth
    clothRobe.requiredGold = 15;
    clothRobe.requiredLevel = 1;
    clothRobe.isDiscovered = true;
    clothRobe.craftingStation = "Workbench";
    clothRobe.successChance = 0.95f;
    clothRobe.experienceGained = 10;
    recipes.push_back(clothRobe);
    
    Recipe paddedArmor;
    paddedArmor.id = 1101;
    paddedArmor.name = "Padded Armor";
    paddedArmor.description = "Quilted layers provide basic protection";
    paddedArmor.resultItemId = 101;
    paddedArmor.resultQuantity = 1;
    paddedArmor.requiredMaterials = {{4, 8}, {3, 2}};  // 8 cloth, 2 leather
    paddedArmor.requiredGold = 28;
    paddedArmor.requiredLevel = 1;
    paddedArmor.isDiscovered = true;
    paddedArmor.craftingStation = "Workbench";
    paddedArmor.successChance = 0.92f;
    paddedArmor.experienceGained = 15;
    recipes.push_back(paddedArmor);
    
    Recipe hideArmor;
    hideArmor.id = 1102;
    hideArmor.name = "Hide Armor";
    hideArmor.description = "Tough animal hide";
    hideArmor.resultItemId = 102;
    hideArmor.resultQuantity = 1;
    hideArmor.requiredMaterials = {{3, 10}};  // 10 leather
    hideArmor.requiredGold = 50;
    hideArmor.requiredLevel = 3;
    hideArmor.isDiscovered = true;
    hideArmor.craftingStation = "Workbench";
    hideArmor.successChance = 0.90f;
    hideArmor.experienceGained = 25;
    recipes.push_back(hideArmor);
    
    Recipe studdedLeather;
    studdedLeather.id = 1103;
    studdedLeather.name = "Studded Leather Armor";
    studdedLeather.description = "Leather reinforced with metal studs";
    studdedLeather.resultItemId = 103;
    studdedLeather.resultQuantity = 1;
    studdedLeather.requiredMaterials = {{3, 12}, {1, 6}};  // 12 leather, 6 iron
    studdedLeather.requiredGold = 85;
    studdedLeather.requiredLevel = 6;
    studdedLeather.isDiscovered = false;
    studdedLeather.craftingStation = "Forge";
    studdedLeather.successChance = 0.82f;
    studdedLeather.experienceGained = 45;
    recipes.push_back(studdedLeather);
    
    // Medium Armor
    Recipe scalemail;
    scalemail.id = 1110;
    scalemail.name = "Scale Mail";
    scalemail.description = "Overlapping metal scales";
    scalemail.resultItemId = 104;
    scalemail.resultQuantity = 1;
    scalemail.requiredMaterials = {{1, 14}, {3, 8}};  // 14 iron, 8 leather
    scalemail.requiredGold = 130;
    scalemail.requiredLevel = 7;
    scalemail.isDiscovered = false;
    scalemail.craftingStation = "Forge";
    scalemail.successChance = 0.78f;
    scalemail.experienceGained = 60;
    recipes.push_back(scalemail);
    
    Recipe breastplate;
    breastplate.id = 1111;
    breastplate.name = "Breastplate";
    breastplate.description = "Solid chest protection";
    breastplate.resultItemId = 105;
    breastplate.resultQuantity = 1;
    breastplate.requiredMaterials = {{1, 16}, {3, 6}};
    breastplate.requiredGold = 160;
    breastplate.requiredLevel = 9;
    breastplate.isDiscovered = false;
    breastplate.craftingStation = "Forge";
    breastplate.successChance = 0.75f;
    breastplate.experienceGained = 70;
    recipes.push_back(breastplate);
    
    Recipe halfPlate;
    halfPlate.id = 1112;
    halfPlate.name = "Half Plate";
    halfPlate.description = "Partial plate armor with good mobility";
    halfPlate.resultItemId = 106;
    halfPlate.resultQuantity = 1;
    halfPlate.requiredMaterials = {{1, 20}, {3, 10}, {5, 2}};
    halfPlate.requiredGold = 260;
    halfPlate.requiredLevel = 12;
    halfPlate.isDiscovered = false;
    halfPlate.craftingStation = "Master Forge";
    halfPlate.successChance = 0.70f;
    halfPlate.experienceGained = 95;
    recipes.push_back(halfPlate);
    
    // Heavy Armor
    Recipe bandedArmor;
    bandedArmor.id = 1120;
    bandedArmor.name = "Banded Mail";
    bandedArmor.description = "Strips of metal bound together";
    bandedArmor.resultItemId = 107;
    bandedArmor.resultQuantity = 1;
    bandedArmor.requiredMaterials = {{1, 18}, {3, 8}};
    bandedArmor.requiredGold = 180;
    bandedArmor.requiredLevel = 10;
    bandedArmor.isDiscovered = false;
    bandedArmor.craftingStation = "Forge";
    bandedArmor.successChance = 0.72f;
    bandedArmor.experienceGained = 82;
    recipes.push_back(bandedArmor);
    
    Recipe splintArmor;
    splintArmor.id = 1121;
    splintArmor.name = "Splint Armor";
    splintArmor.description = "Vertical metal strips over leather";
    splintArmor.resultItemId = 108;
    splintArmor.resultQuantity = 1;
    splintArmor.requiredMaterials = {{1, 22}, {3, 12}};
    splintArmor.requiredGold = 280;
    splintArmor.requiredLevel = 13;
    splintArmor.isDiscovered = false;
    splintArmor.craftingStation = "Master Forge";
    splintArmor.successChance = 0.68f;
    splintArmor.experienceGained = 110;
    recipes.push_back(splintArmor);
    
    Recipe fullPlate;
    fullPlate.id = 1122;
    fullPlate.name = "Full Plate Armor";
    fullPlate.description = "Complete heavy plate protection";
    fullPlate.resultItemId = 109;
    fullPlate.resultQuantity = 1;
    fullPlate.requiredMaterials = {{1, 30}, {3, 15}, {5, 3}};
    fullPlate.requiredGold = 500;
    fullPlate.requiredLevel = 16;
    fullPlate.isDiscovered = false;
    fullPlate.craftingStation = "Master Forge";
    fullPlate.successChance = 0.62f;
    fullPlate.experienceGained = 145;
    recipes.push_back(fullPlate);
    
    Recipe dragonscale;
    dragonscale.id = 1123;
    dragonscale.name = "Dragonscale Armor";
    dragonscale.description = "Crafted from genuine dragon scales";
    dragonscale.resultItemId = 110;
    dragonscale.resultQuantity = 1;
    dragonscale.requiredMaterials = {{9, 12}, {1, 20}, {6, 8}};  // 12 dragon scales, 20 iron, 8 essence
    dragonscale.requiredGold = 900;
    dragonscale.requiredLevel = 20;
    dragonscale.isDiscovered = false;
    dragonscale.craftingStation = "Legendary Forge";
    dragonscale.successChance = 0.50f;
    dragonscale.experienceGained = 250;
    recipes.push_back(dragonscale);
    
    // Shields
    Recipe woodenShield;
    woodenShield.id = 1130;
    woodenShield.name = "Wooden Shield";
    woodenShield.description = "A basic wooden shield";
    woodenShield.resultItemId = 120;
    woodenShield.resultQuantity = 1;
    woodenShield.requiredMaterials = {{2, 10}, {1, 2}};
    woodenShield.requiredGold = 25;
    woodenShield.requiredLevel = 1;
    woodenShield.isDiscovered = true;
    woodenShield.craftingStation = "Workbench";
    woodenShield.successChance = 0.90f;
    woodenShield.experienceGained = 18;
    recipes.push_back(woodenShield);
    
    Recipe steelShield;
    steelShield.id = 1131;
    steelShield.name = "Steel Shield";
    steelShield.description = "Sturdy metal shield";
    steelShield.resultItemId = 121;
    steelShield.resultQuantity = 1;
    steelShield.requiredMaterials = {{1, 12}, {2, 6}};
    steelShield.requiredGold = 80;
    steelShield.requiredLevel = 5;
    steelShield.isDiscovered = false;
    steelShield.craftingStation = "Forge";
    steelShield.successChance = 0.83f;
    steelShield.experienceGained = 52;
    recipes.push_back(steelShield);
    
    Recipe towerShield;
    towerShield.id = 1132;
    towerShield.name = "Tower Shield";
    towerShield.description = "Massive shield offering great protection";
    towerShield.resultItemId = 122;
    towerShield.resultQuantity = 1;
    towerShield.requiredMaterials = {{1, 20}, {2, 10}, {5, 2}};
    towerShield.requiredGold = 230;
    towerShield.requiredLevel = 11;
    towerShield.isDiscovered = false;
    towerShield.craftingStation = "Master Forge";
    towerShield.successChance = 0.70f;
    towerShield.experienceGained = 88;
    recipes.push_back(towerShield);
    
    // ==================== ACCESSORY RECIPES ====================
    
    Recipe strengthRing;
    strengthRing.id = 1200;
    strengthRing.name = "Ring of Might";
    strengthRing.description = "Increases physical strength";
    strengthRing.resultItemId = 130;
    strengthRing.resultQuantity = 1;
    strengthRing.requiredMaterials = {{1, 5}, {5, 2}};  // 5 iron, 2 gems
    strengthRing.requiredGold = 120;
    strengthRing.requiredLevel = 8;
    strengthRing.isDiscovered = false;
    strengthRing.craftingStation = "Enchanting Table";
    strengthRing.successChance = 0.76f;
    strengthRing.experienceGained = 65;
    recipes.push_back(strengthRing);
    
    Recipe defenseRing;
    defenseRing.id = 1201;
    defenseRing.name = "Ring of Protection";
    defenseRing.description = "Enhances defensive capabilities";
    defenseRing.resultItemId = 131;
    defenseRing.resultQuantity = 1;
    defenseRing.requiredMaterials = {{1, 4}, {5, 2}, {6, 1}};  // 4 iron, 2 gems, 1 essence
    defenseRing.requiredGold = 110;
    defenseRing.requiredLevel = 7;
    defenseRing.isDiscovered = false;
    defenseRing.craftingStation = "Enchanting Table";
    defenseRing.successChance = 0.78f;
    defenseRing.experienceGained = 58;
    recipes.push_back(defenseRing);
    
    Recipe vitalityAmulet;
    vitalityAmulet.id = 1202;
    vitalityAmulet.name = "Amulet of Vitality";
    vitalityAmulet.description = "Increases maximum health";
    vitalityAmulet.resultItemId = 132;
    vitalityAmulet.resultQuantity = 1;
    vitalityAmulet.requiredMaterials = {{5, 4}, {6, 3}, {7, 5}};  // 4 gems, 3 essence, 5 herbs
    vitalityAmulet.requiredGold = 200;
    vitalityAmulet.requiredLevel = 10;
    vitalityAmulet.isDiscovered = false;
    vitalityAmulet.craftingStation = "Enchanting Table";
    vitalityAmulet.successChance = 0.72f;
    vitalityAmulet.experienceGained = 85;
    recipes.push_back(vitalityAmulet);
    
    Recipe shadowCloak;
    shadowCloak.id = 1203;
    shadowCloak.name = "Cloak of Shadows";
    shadowCloak.description = "Enhances stealth and evasion";
    shadowCloak.resultItemId = 133;
    shadowCloak.resultQuantity = 1;
    shadowCloak.requiredMaterials = {{4, 15}, {6, 4}, {8, 3}};  // 15 cloth, 4 essence, 3 bones
    shadowCloak.requiredGold = 250;
    shadowCloak.requiredLevel = 12;
    shadowCloak.isDiscovered = false;
    shadowCloak.craftingStation = "Master Workbench";
    shadowCloak.successChance = 0.68f;
    shadowCloak.experienceGained = 98;
    recipes.push_back(shadowCloak);
    
    Recipe dragonHeart;
    dragonHeart.id = 1204;
    dragonHeart.name = "Heart of the Dragon";
    dragonHeart.description = "Contains the essence of a slain dragon";
    dragonHeart.resultItemId = 134;
    dragonHeart.resultQuantity = 1;
    dragonHeart.requiredMaterials = {{9, 8}, {6, 10}, {10, 4}};  // 8 dragon scales, 10 essence, 4 crystals
    dragonHeart.requiredGold = 750;
    dragonHeart.requiredLevel = 18;
    dragonHeart.isDiscovered = false;
    dragonHeart.craftingStation = "Legendary Enchanting Table";
    dragonHeart.successChance = 0.55f;
    dragonHeart.experienceGained = 220;
    recipes.push_back(dragonHeart);
    
    // ==================== CONSUMABLE RECIPES ====================
    
    // Potions
    Recipe minorHealthPotion;
    minorHealthPotion.id = 1300;
    minorHealthPotion.name = "Minor Health Potion";
    minorHealthPotion.description = "Restores 25 HP";
    minorHealthPotion.resultItemId = 1;
    minorHealthPotion.resultQuantity = 3;
    minorHealthPotion.requiredMaterials = {{7, 2}};  // 2 herbs
    minorHealthPotion.requiredGold = 10;
    minorHealthPotion.requiredLevel = 1;
    minorHealthPotion.isDiscovered = true;
    minorHealthPotion.craftingStation = "Alchemy Lab";
    minorHealthPotion.successChance = 0.95f;
    minorHealthPotion.experienceGained = 8;
    recipes.push_back(minorHealthPotion);
    
    Recipe healthPotion;
    healthPotion.id = 1301;
    healthPotion.name = "Health Potion";
    healthPotion.description = "Restores 50 HP";
    healthPotion.resultItemId = 2;
    healthPotion.resultQuantity = 2;
    healthPotion.requiredMaterials = {{7, 4}};  // 4 herbs
    healthPotion.requiredGold = 25;
    healthPotion.requiredLevel = 3;
    healthPotion.isDiscovered = true;
    healthPotion.craftingStation = "Alchemy Lab";
    healthPotion.successChance = 0.88f;
    healthPotion.experienceGained = 15;
    recipes.push_back(healthPotion);
    
    Recipe greaterHealthPotion;
    greaterHealthPotion.id = 1302;
    greaterHealthPotion.name = "Greater Health Potion";
    greaterHealthPotion.description = "Restores 100 HP";
    greaterHealthPotion.resultItemId = 3;
    greaterHealthPotion.resultQuantity = 1;
    greaterHealthPotion.requiredMaterials = {{7, 6}, {6, 1}};  // 6 herbs, 1 essence
    greaterHealthPotion.requiredGold = 50;
    greaterHealthPotion.requiredLevel = 6;
    greaterHealthPotion.isDiscovered = false;
    greaterHealthPotion.craftingStation = "Alchemy Lab";
    greaterHealthPotion.successChance = 0.80f;
    greaterHealthPotion.experienceGained = 32;
    recipes.push_back(greaterHealthPotion);
    
    Recipe supremePotion;
    supremePotion.id = 1303;
    supremePotion.name = "Supreme Health Potion";
    supremePotion.description = "Restores 200 HP";
    supremePotion.resultItemId = 4;
    supremePotion.resultQuantity = 1;
    supremePotion.requiredMaterials = {{7, 10}, {6, 3}, {10, 1}};  // 10 herbs, 3 essence, 1 crystal
    supremePotion.requiredGold = 140;
    supremePotion.requiredLevel = 12;
    supremePotion.isDiscovered = false;
    supremePotion.craftingStation = "Master Alchemy Lab";
    supremePotion.successChance = 0.70f;
    supremePotion.experienceGained = 75;
    recipes.push_back(supremePotion);
    
    // Elixirs
    Recipe strengthElixir;
    strengthElixir.id = 1310;
    strengthElixir.name = "Elixir of Strength";
    strengthElixir.description = "Temporarily increases attack power";
    strengthElixir.resultItemId = 60;
    strengthElixir.resultQuantity = 1;
    strengthElixir.requiredMaterials = {{7, 3}, {8, 2}};  // 3 herbs, 2 bones
    strengthElixir.requiredGold = 55;
    strengthElixir.requiredLevel = 5;
    strengthElixir.isDiscovered = false;
    strengthElixir.craftingStation = "Alchemy Lab";
    strengthElixir.successChance = 0.82f;
    strengthElixir.experienceGained = 38;
    recipes.push_back(strengthElixir);
    
    Recipe defenseElixir;
    defenseElixir.id = 1311;
    defenseElixir.name = "Elixir of Defense";
    defenseElixir.description = "Temporarily increases defense";
    defenseElixir.resultItemId = 61;
    defenseElixir.resultQuantity = 1;
    defenseElixir.requiredMaterials = {{7, 3}, {1, 3}};  // 3 herbs, 3 iron
    defenseElixir.requiredGold = 55;
    defenseElixir.requiredLevel = 5;
    defenseElixir.isDiscovered = false;
    defenseElixir.craftingStation = "Alchemy Lab";
    defenseElixir.successChance = 0.82f;
    defenseElixir.experienceGained = 38;
    recipes.push_back(defenseElixir);
    
    Recipe speedElixir;
    speedElixir.id = 1312;
    speedElixir.name = "Elixir of Agility";
    speedElixir.description = "Temporarily increases speed and dodge chance";
    speedElixir.resultItemId = 62;
    speedElixir.resultQuantity = 1;
    speedElixir.requiredMaterials = {{7, 5}, {6, 2}};  // 5 herbs, 2 essence
    speedElixir.requiredGold = 85;
    speedElixir.requiredLevel = 8;
    speedElixir.isDiscovered = false;
    speedElixir.craftingStation = "Alchemy Lab";
    speedElixir.successChance = 0.75f;
    speedElixir.experienceGained = 55;
    recipes.push_back(speedElixir);
    
    Recipe berserkPotion;
    berserkPotion.id = 1313;
    berserkPotion.name = "Potion of Berserker Rage";
    berserkPotion.description = "Massive attack boost, reduced defense";
    berserkPotion.resultItemId = 141;
    berserkPotion.resultQuantity = 1;
    berserkPotion.requiredMaterials = {{7, 6}, {8, 4}, {6, 2}};  // 6 herbs, 4 bones, 2 essence
    berserkPotion.requiredGold = 115;
    berserkPotion.requiredLevel = 10;
    berserkPotion.isDiscovered = false;
    berserkPotion.craftingStation = "Master Alchemy Lab";
    berserkPotion.successChance = 0.72f;
    berserkPotion.experienceGained = 68;
    recipes.push_back(berserkPotion);
    
    Recipe invisPotion;
    invisPotion.id = 1314;
    invisPotion.name = "Potion of Invisibility";
    invisPotion.description = "Become invisible to enemies";
    invisPotion.resultItemId = 142;
    invisPotion.resultQuantity = 1;
    invisPotion.requiredMaterials = {{7, 8}, {6, 5}, {10, 2}};  // 8 herbs, 5 essence, 2 crystals
    invisPotion.requiredGold = 220;
    invisPotion.requiredLevel = 14;
    invisPotion.isDiscovered = false;
    invisPotion.craftingStation = "Master Alchemy Lab";
    invisPotion.successChance = 0.65f;
    invisPotion.experienceGained = 95;
    recipes.push_back(invisPotion);
}

void CraftingSystem::initializeMaterials() {
    // Initialize material types
    CraftingMaterial iron;
    iron.id = 1;
    iron.name = "Iron Ore";
    iron.type = MaterialType::METAL;
    iron.rarity = ItemRarity::COMMON;
    iron.quantity = 0;
    materials[1] = iron;
    
    CraftingMaterial wood;
    wood.id = 2;
    wood.name = "Wood";
    wood.type = MaterialType::WOOD;
    wood.rarity = ItemRarity::COMMON;
    wood.quantity = 0;
    materials[2] = wood;
    
    CraftingMaterial leather;
    leather.id = 3;
    leather.name = "Leather";
    leather.type = MaterialType::LEATHER;
    leather.rarity = ItemRarity::COMMON;
    leather.quantity = 0;
    materials[3] = leather;
    
    CraftingMaterial cloth;
    cloth.id = 4;
    cloth.name = "Cloth";
    cloth.type = MaterialType::CLOTH;
    cloth.rarity = ItemRarity::COMMON;
    cloth.quantity = 0;
    materials[4] = cloth;
    
    CraftingMaterial gem;
    gem.id = 5;
    gem.name = "Rough Gem";
    gem.type = MaterialType::GEM;
    gem.rarity = ItemRarity::UNCOMMON;
    gem.quantity = 0;
    materials[5] = gem;
    
    CraftingMaterial essence;
    essence.id = 6;
    essence.name = "Magical Essence";
    essence.type = MaterialType::ESSENCE;
    essence.rarity = ItemRarity::RARE;
    essence.quantity = 0;
    materials[6] = essence;
    
    CraftingMaterial herb;
    herb.id = 7;
    herb.name = "Healing Herb";
    herb.type = MaterialType::HERB;
    herb.rarity = ItemRarity::COMMON;
    herb.quantity = 0;
    materials[7] = herb;
    
    CraftingMaterial bone;
    bone.id = 8;
    bone.name = "Monster Bone";
    bone.type = MaterialType::BONE;
    bone.rarity = ItemRarity::UNCOMMON;
    bone.quantity = 0;
    materials[8] = bone;
    
    CraftingMaterial dragonScale;
    dragonScale.id = 9;
    dragonScale.name = "Dragon Scale";
    dragonScale.type = MaterialType::DRAGON_SCALE;
    dragonScale.rarity = ItemRarity::LEGENDARY;
    dragonScale.quantity = 0;
    materials[9] = dragonScale;
    
    CraftingMaterial crystal;
    crystal.id = 10;
    crystal.name = "Magic Crystal";
    crystal.type = MaterialType::CRYSTAL;
    crystal.rarity = ItemRarity::RARE;
    crystal.quantity = 0;
    materials[10] = crystal;
}

Recipe* CraftingSystem::findRecipe(int recipeId) {
    for (auto& recipe : recipes) {
        if (recipe.id == recipeId) {
            return &recipe;
        }
    }
    return nullptr;
}

float CraftingSystem::calculateSuccessChance(const Recipe& recipe) const {
    float chance = recipe.successChance;
    
    // Bonus for higher level
    int levelDiff = playerRef.level - recipe.requiredLevel;
    if (levelDiff > 0) {
        chance += levelDiff * 0.02f;  // 2% per level above requirement
    }
    
    // Cap at 100%
    if (chance > 1.0f) chance = 1.0f;
    
    return chance;
}

// ---
// MerchantSystem Implementation
// ---

MerchantSystem::MerchantSystem(Player& player) 
    : playerRef(player), reputationPoints(0), merchantTier(0) {
}

void MerchantSystem::displayShop(const MerchantInventory& merchant) {
    std::cout << "\n========================================\n";
    std::cout << "      " << merchant.name << "\n";
    std::cout << "========================================\n\n";
    
    std::cout << merchant.greeting << "\n\n";
    std::cout << "Your Gold: " << playerRef.gold << "\n";
    std::cout << "Merchant Gold: " << merchant.merchantGold << "\n";
    std::cout << "Reputation Discount: " << static_cast<int>(getReputationDiscount() * 100) << "%\n\n";
    
    std::cout << "--- ITEMS FOR SALE ---\n";
    std::cout << std::left << std::setw(7) << "Slot" << std::setw(30) << "Item"
              << std::setw(10) << "Stock" << std::setw(10) << "Price" << "\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (size_t i = 0; i < merchant.items.size(); ++i) {
        const auto& item = merchant.items[i];
        int price = calculateBuyPrice(item, merchant);
        
        // Get stock info
        std::string stockInfo = "∞";
        auto stockIt = merchant.stock.find(item.id);
        if (stockIt != merchant.stock.end() && stockIt->second != -1) {
            stockInfo = std::to_string(stockIt->second);
        }
        
        std::cout << std::left << std::setw(7) << (i + 1)
                  << std::setw(30) << item.name.substr(0, 28)
                  << std::setw(10) << stockInfo
                  << std::setw(10) << price << " g\n";
    }
}

bool MerchantSystem::buyItem(MerchantInventory& merchant, int slotIndex) {
    // Adjust for 1-based indexing from UI
    int index = slotIndex - 1;
    
    if (index < 0 || index >= static_cast<int>(merchant.items.size())) {
        std::cout << "Invalid slot number!\n";
        return false;
    }
    
    Item& item = merchant.items[index];
    int itemId = item.id;
    
    // Check stock
    if (!hasStock(merchant, itemId)) {
        std::cout << "Item is out of stock!\n";
        return false;
    }
    
    int price = calculateBuyPrice(item, merchant);
    
    if (playerRef.gold < price) {
        std::cout << "Not enough gold! Need " << price << " gold.\n";
        return false;
    }
    
    // Execute transaction
    playerRef.gold -= price;
    merchant.merchantGold += price;
    
    // Add to player inventory
    Item purchasedItem = item;
    purchasedItem.quantity = 1;
    playerRef.inventory.push_back(purchasedItem);
    
    // Reduce stock
    reduceStock(merchant, itemId);
    
    std::cout << "\nPurchased " << item.name << " for " << price << " gold!\n";
    updateDemand(itemId, 1);  // Increase demand
    increaseReputation(5);
    
    return true;
}

bool MerchantSystem::sellItem(MerchantInventory& merchant, int slotIndex) {
    // Adjust for 1-based indexing from UI
    int index = slotIndex - 1;
    
    if (index < 0 || index >= static_cast<int>(playerRef.inventory.size())) {
        std::cout << "Invalid slot number!\n";
        return false;
    }
    
    Item& item = playerRef.inventory[index];
    int itemId = item.id;
    
    // Check if merchant accepts this item
    if (!merchant.acceptsAllItems && item.type == ItemType::QUEST) {
        std::cout << "The merchant won't buy quest items.\n";
        return false;
    }
    
    int price = calculateSellPrice(item, merchant);
    
    if (merchant.merchantGold < price) {
        std::cout << "Merchant doesn't have enough gold!\n";
        return false;
    }
    
    // Execute transaction
    playerRef.gold += price;
    merchant.merchantGold -= price;
    
    // Add to merchant inventory
    addItemToStock(merchant, item, 1);
    
    // Remove from player inventory
    std::cout << "\nSold " << item.name << " for " << price << " gold!\n";
    playerRef.inventory.erase(playerRef.inventory.begin() + index);
    
    updateDemand(itemId, -1);  // Decrease demand
    increaseReputation(2);
    
    return true;
}

TradeOffer MerchantSystem::createTradeOffer(const std::vector<int>& offered,
                                            const std::vector<int>& requested) {
    TradeOffer trade;
    trade.offeredItemIds = offered;
    trade.requestedItemIds = requested;
    
    // Calculate value difference
    int offeredValue = 0;
    int requestedValue = 0;
    
    for (int id : offered) {
        (void)id;  // ID would be used to look up item in actual implementation
        offeredValue += evaluateItemValue(Item());  // Simplified
    }
    
    for (int id : requested) {
        (void)id;  // ID would be used to look up item in actual implementation
        requestedValue += evaluateItemValue(Item());  // Simplified
    }
    
    trade.goldDifference = requestedValue - offeredValue;
    trade.isAcceptable = (trade.goldDifference <= 0 || checkBarterSkill(trade.goldDifference));
    
    return trade;
}

bool MerchantSystem::executeTrade(MerchantInventory& merchant, const TradeOffer& trade) {
    (void)merchant;
    
    if (!trade.isAcceptable) {
        std::cout << "Merchant refuses this trade!\n";
        return false;
    }
    
    if (trade.goldDifference > 0 && playerRef.gold < trade.goldDifference) {
        std::cout << "Not enough gold to complete trade!\n";
        return false;
    }
    
    // Execute trade
    playerRef.gold -= trade.goldDifference;
    
    std::cout << "Trade completed!\n";
    return true;
}

int MerchantSystem::evaluateItemValue(const Item& item) const {
    int baseValue = item.value;
    
    // Adjust based on rarity
    switch (item.rarity) {
        case ItemRarity::COMMON: break;
        case ItemRarity::UNCOMMON: baseValue = static_cast<int>(baseValue * 1.5f); break;
        case ItemRarity::RARE: baseValue = static_cast<int>(baseValue * 2.0f); break;
        case ItemRarity::EPIC: baseValue = static_cast<int>(baseValue * 3.0f); break;
        case ItemRarity::LEGENDARY: baseValue = static_cast<int>(baseValue * 5.0f); break;
    }
    
    return baseValue;
}

void MerchantSystem::updatePrices(MerchantInventory& merchant) {
    for (auto& item : merchant.items) {
        auto it = itemDemand.find(item.id);
        int demand = (it != itemDemand.end()) ? it->second : 0;
        
        // Adjust price based on demand
        float multiplier = calculatePriceMultiplier(item.id, merchant.items.size(), demand);
        merchant.priceMultipliers[item.id] = multiplier;
    }
}

float MerchantSystem::calculatePriceMultiplier(int itemId, int supply, int demand) const {
    (void)itemId;
    
    float baseMultiplier = 1.0f;
    
    // Low supply = higher prices
    if (supply < 5) {
        baseMultiplier += 0.1f * (5 - supply);
    }
    
    // High demand = higher prices
    if (demand > 0) {
        baseMultiplier += 0.05f * demand;
    }
    
    return baseMultiplier;
}

void MerchantSystem::increaseReputation(int amount) {
    reputationPoints += amount;
    
    // Update tier based on reputation
    if (reputationPoints >= 1000) {
        merchantTier = 3;
    } else if (reputationPoints >= 500) {
        merchantTier = 2;
    } else if (reputationPoints >= 100) {
        merchantTier = 1;
    }
}

float MerchantSystem::getReputationDiscount() const {
    // 5% discount per tier
    return merchantTier * 0.05f;
}

void MerchantSystem::displayBlackMarket() {
    std::cout << "\n========================================\n";
    std::cout << "         BLACK MARKET\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Shady dealings available here...\n";
    std::cout << "(Requires merchant tier 2 or higher)\n";
    
    if (merchantTier < 2) {
        std::cout << "You don't have access to the black market yet.\n";
        return;
    }
    
    std::cout << "Special items available for purchase.\n";
}

void MerchantSystem::displayTravelingMerchant() {
    std::cout << "\n========================================\n";
    std::cout << "      TRAVELING MERCHANT\n";
    std::cout << "========================================\n\n";
    
    std::cout << "A mysterious traveler offers rare goods...\n";
}

bool MerchantSystem::unlockMerchantTier(int tier) {
    if (tier <= merchantTier) {
        return true;  // Already unlocked
    }
    
    int requiredRep = tier * 300;
    if (reputationPoints >= requiredRep) {
        merchantTier = tier;
        std::cout << "Merchant tier " << tier << " unlocked!\n";
        return true;
    }
    
    std::cout << "Need " << requiredRep << " reputation to unlock tier " << tier << "\n";
    return false;
}

void MerchantSystem::updateDemand(int itemId, int change) {
    itemDemand[itemId] += change;
}

bool MerchantSystem::checkBarterSkill(int difficulty) const {
    // Simple skill check - could be expanded
    int barterSkill = playerRef.level * 2;
    return barterSkill >= difficulty;
}

int MerchantSystem::calculateBuyPrice(const Item& item, const MerchantInventory& merchant) const {
    float multiplier = merchant.buyPriceMultiplier;
    
    // Check for item-specific multiplier
    auto it = merchant.priceMultipliers.find(item.id);
    if (it != merchant.priceMultipliers.end()) {
        multiplier *= it->second;
    }
    
    // Apply reputation discount
    float discount = 1.0f - getReputationDiscount();
    
    return static_cast<int>(item.value * multiplier * discount);
}

int MerchantSystem::calculateSellPrice(const Item& item, const MerchantInventory& merchant) const {
    return static_cast<int>(item.value * merchant.sellPriceMultiplier);
}

bool MerchantSystem::hasStock(const MerchantInventory& merchant, int itemId) const {
    auto it = merchant.stock.find(itemId);
    if (it == merchant.stock.end()) {
        return true;  // No stock tracking = infinite
    }
    return it->second != 0;  // -1 = infinite, >0 = has stock
}

void MerchantSystem::reduceStock(MerchantInventory& merchant, int itemId) {
    auto it = merchant.stock.find(itemId);
    if (it != merchant.stock.end() && it->second > 0) {
        it->second--;
    }
}

void MerchantSystem::restockVendor(MerchantInventory& merchant) {
    // Restock all items to initial quantities
    for (auto& stockPair : merchant.stock) {
        if (stockPair.second != -1) {  // Don't modify infinite stock
            stockPair.second = 3 + (rand() % 5);  // Restock to 3-7 items
        }
    }
    
    // Add some merchant gold
    merchant.merchantGold += 500 + (rand() % 500);
    
    merchant.restockTurns = 0;
    std::cout << merchant.name << " has restocked their inventory!\n";
}

void MerchantSystem::addItemToStock(MerchantInventory& merchant, const Item& item, int quantity) {
    // Check if item already exists
    for (auto& existingItem : merchant.items) {
        if (existingItem.id == item.id) {
            // Update stock
            auto it = merchant.stock.find(item.id);
            if (it != merchant.stock.end() && it->second != -1) {
                it->second += quantity;
            }
            return;
        }
    }
    
    // Add new item
    merchant.items.push_back(item);
    merchant.stock[item.id] = quantity;
}

Item* MerchantSystem::findItemInVendorStock(MerchantInventory& merchant, int itemId) {
    for (auto& item : merchant.items) {
        if (item.id == itemId) {
            return &item;
        }
    }
    return nullptr;
}

Item* MerchantSystem::findItemInPlayerInventory(int itemId) {
    for (auto& item : playerRef.inventory) {
        if (item.id == itemId) {
            return &item;
        }
    }
    return nullptr;
}

void MerchantSystem::runShopMenu(MerchantInventory& merchant) {
    bool shopping = true;
    
    while (shopping) {
        std::cout << "\n========================================\n";
        std::cout << "  " << merchant.name << "\n";
        std::cout << "========================================\n";
        std::cout << merchant.greeting << "\n\n";
        std::cout << "Your Gold: " << playerRef.gold << " | Merchant Gold: " << merchant.merchantGold << "\n\n";
        std::cout << "1. Buy Items\n";
        std::cout << "2. Sell Items\n";
        std::cout << "3. View Inventory\n";
        std::cout << "4. Leave Shop\n";
        std::cout << "\nChoice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(1000, '\n');
        
        switch (choice) {
            case 1:
                displayBuyMenu(merchant);
                break;
            case 2:
                displaySellMenu(merchant);
                break;
            case 3:
                displayShop(merchant);
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                break;
            case 4:
                std::cout << "\nThank you for your business!\n";
                shopping = false;
                break;
            default:
                std::cout << "\nInvalid choice!\n";
        }
    }
}

void MerchantSystem::displayBuyMenu(MerchantInventory& merchant) {
    if (merchant.items.empty()) {
        std::cout << "\nThe merchant has no items for sale.\n";
        std::cout << "Press Enter to continue...";
        std::cin.get();
        return;
    }
    
    displayShop(merchant);
    
    std::cout << "\nEnter slot number to buy (0 to cancel): ";
    int slotIndex;
    std::cin >> slotIndex;
    std::cin.ignore(1000, '\n');
    
    if (slotIndex == 0) return;
    
    if (buyItem(merchant, slotIndex)) {
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    } else {
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

void MerchantSystem::displaySellMenu(MerchantInventory& merchant) {
    if (playerRef.inventory.empty()) {
        std::cout << "\nYou have no items to sell.\n";
        std::cout << "Press Enter to continue...";
        std::cin.get();
        return;
    }
    
    std::cout << "\n--- YOUR INVENTORY ---\n";
    std::cout << std::left << std::setw(7) << "Slot" << std::setw(30) << "Item"
              << std::setw(10) << "Sell Price" << "\n";
    std::cout << std::string(50, '-') << "\n";
    
    for (size_t i = 0; i < playerRef.inventory.size(); ++i) {
        const auto& item = playerRef.inventory[i];
        int sellPrice = calculateSellPrice(item, merchant);
        std::cout << std::left << std::setw(7) << (i + 1)
                  << std::setw(30) << item.name.substr(0, 28)
                  << std::setw(10) << sellPrice << " g\n";
    }
    
    std::cout << "\nEnter slot number to sell (0 to cancel): ";
    int slotIndex;
    std::cin >> slotIndex;
    std::cin.ignore(1000, '\n');
    
    if (slotIndex == 0) return;
    
    if (sellItem(merchant, slotIndex)) {
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    } else {
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

MerchantInventory MerchantSystem::createVendor(VendorType type, int dungeonLevel) {
    switch (type) {
        case VendorType::GENERAL_STORE:
            return createGeneralStore(dungeonLevel);
        case VendorType::WEAPONSMITH:
            return createWeaponsmith(dungeonLevel);
        case VendorType::ARMORER:
            return createArmorer(dungeonLevel);
        case VendorType::APOTHECARY:
            return createApothecary(dungeonLevel);
        case VendorType::ENCHANTER:
            return createEnchanter(dungeonLevel);
        case VendorType::BLACK_MARKET:
            return createBlackMarket(dungeonLevel);
        case VendorType::TRAVELING:
            return createTravelingMerchant(dungeonLevel);
        default:
            return createGeneralStore(dungeonLevel);
    }
}

MerchantInventory MerchantSystem::createGeneralStore(int level) {
    MerchantInventory shop;
    shop.name = "General Store";
    shop.greeting = "Welcome! I have basic supplies for adventurers.";
    shop.vendorType = VendorType::GENERAL_STORE;
    shop.merchantGold = 500 + (level * 100);
    shop.buyPriceMultiplier = 1.3f;
    shop.sellPriceMultiplier = 0.6f;
    shop.acceptsAllItems = true;
    
    // Add basic items (these would come from game's item templates)
    Item potion1, potion2, potion3;
    potion1.id = 1; potion1.name = "Minor Health Potion"; potion1.value = 10; 
    potion1.type = ItemType::CONSUMABLE; potion1.rarity = ItemRarity::COMMON;
    potion2.id = 2; potion2.name = "Health Potion"; potion2.value = 25;
    potion2.type = ItemType::CONSUMABLE; potion2.rarity = ItemRarity::UNCOMMON;
    potion3.id = 3; potion3.name = "Greater Health Potion"; potion3.value = 50;
    potion3.type = ItemType::CONSUMABLE; potion3.rarity = ItemRarity::RARE;

    configureVendorTemplateItem(potion1, level);
    configureVendorTemplateItem(potion2, level);
    configureVendorTemplateItem(potion3, level);
    
    shop.items.push_back(potion1);
    shop.items.push_back(potion2);
    if (level >= 3) shop.items.push_back(potion3);
    
    shop.stock[1] = 10;  // Plenty of minor potions
    shop.stock[2] = 5;   // Some health potions
    if (level >= 3) shop.stock[3] = 2;  // Few greater potions
    
    return shop;
}

MerchantInventory MerchantSystem::createWeaponsmith(int level) {
    MerchantInventory shop;
    shop.name = "The Forge";
    shop.greeting = "Fine weapons forged with skill and care.";
    shop.vendorType = VendorType::WEAPONSMITH;
    shop.merchantGold = 800 + (level * 150);
    shop.buyPriceMultiplier = 1.6f;
    shop.sellPriceMultiplier = 0.5f;
    shop.acceptsAllItems = false;
    
    Item sword, axe, staff;
    sword.id = 10; sword.name = "Iron Sword"; sword.value = 30;
    sword.type = ItemType::WEAPON; sword.rarity = ItemRarity::COMMON;
    axe.id = 11; axe.name = "Battle Axe"; axe.value = 60;
    axe.type = ItemType::WEAPON; axe.rarity = ItemRarity::UNCOMMON;
    staff.id = 12; staff.name = "Arcane Staff"; staff.value = 100;
    staff.type = ItemType::WEAPON; staff.rarity = ItemRarity::RARE;

    configureVendorTemplateItem(sword, level);
    configureVendorTemplateItem(axe, level);
    configureVendorTemplateItem(staff, level);
    
    shop.items.push_back(sword);
    if (level >= 2) shop.items.push_back(axe);
    if (level >= 4) shop.items.push_back(staff);
    
    shop.stock[10] = 3;
    if (level >= 2) shop.stock[11] = 2;
    if (level >= 4) shop.stock[12] = 1;
    
    return shop;
}

MerchantInventory MerchantSystem::createArmorer(int level) {
    MerchantInventory shop;
    shop.name = "Armor & Defense";
    shop.greeting = "The best protection gold can buy!";
    shop.vendorType = VendorType::ARMORER;
    shop.merchantGold = 700 + (level * 120);
    shop.buyPriceMultiplier = 1.5f;
    shop.sellPriceMultiplier = 0.5f;
    shop.acceptsAllItems = false;
    
    Item leather, chain, plate;
    leather.id = 20; leather.name = "Leather Armor"; leather.value = 25;
    leather.type = ItemType::ARMOR; leather.rarity = ItemRarity::COMMON;
    chain.id = 21; chain.name = "Chainmail"; chain.value = 75;
    chain.type = ItemType::ARMOR; chain.rarity = ItemRarity::UNCOMMON;
    plate.id = 22; plate.name = "Plate Armor"; plate.value = 200;
    plate.type = ItemType::ARMOR; plate.rarity = ItemRarity::RARE;

    configureVendorTemplateItem(leather, level);
    configureVendorTemplateItem(chain, level);
    configureVendorTemplateItem(plate, level);
    
    shop.items.push_back(leather);
    if (level >= 2) shop.items.push_back(chain);
    if (level >= 5) shop.items.push_back(plate);
    
    shop.stock[20] = 3;
    if (level >= 2) shop.stock[21] = 2;
    if (level >= 5) shop.stock[22] = 1;
    
    return shop;
}

MerchantInventory MerchantSystem::createApothecary(int level) {
    MerchantInventory shop;
    shop.name = "The Apothecary";
    shop.greeting = "Potions, elixirs, and remedies of all kinds.";
    shop.vendorType = VendorType::APOTHECARY;
    shop.merchantGold = 600 + (level * 100);
    shop.buyPriceMultiplier = 1.4f;
    shop.sellPriceMultiplier = 0.6f;
    shop.acceptsAllItems = true;
    
    Item potion1, potion2, potion3, elixir;
    potion1.id = 1; potion1.name = "Minor Health Potion"; potion1.value = 10;
    potion1.type = ItemType::CONSUMABLE; potion1.rarity = ItemRarity::COMMON;
    potion2.id = 2; potion2.name = "Health Potion"; potion2.value = 25;
    potion2.type = ItemType::CONSUMABLE; potion2.rarity = ItemRarity::UNCOMMON;
    potion3.id = 3; potion3.name = "Greater Health Potion"; potion3.value = 50;
    potion3.type = ItemType::CONSUMABLE; potion3.rarity = ItemRarity::RARE;
    elixir.id = 60; elixir.name = "Elixir of Strength"; elixir.value = 40;
    elixir.type = ItemType::CONSUMABLE; elixir.rarity = ItemRarity::UNCOMMON;

    configureVendorTemplateItem(potion1, level);
    configureVendorTemplateItem(potion2, level);
    configureVendorTemplateItem(potion3, level);
    configureVendorTemplateItem(elixir, level);
    
    shop.items.push_back(potion1);
    shop.items.push_back(potion2);
    shop.items.push_back(potion3);
    if (level >= 3) shop.items.push_back(elixir);
    
    shop.stock[1] = 15;
    shop.stock[2] = 8;
    shop.stock[3] = 4;
    if (level >= 3) shop.stock[60] = 3;
    
    return shop;
}

MerchantInventory MerchantSystem::createEnchanter(int level) {
    MerchantInventory shop;
    shop.name = "Arcane Emporium";
    shop.greeting = "Magical wares and mystical items await.";
    shop.vendorType = VendorType::ENCHANTER;
    shop.merchantGold = 1000 + (level * 200);
    shop.buyPriceMultiplier = 2.0f;
    shop.sellPriceMultiplier = 0.4f;
    shop.acceptsAllItems = false;
    
    Item ring, amulet;
    ring.id = 30; ring.name = "Ring of Swiftness"; ring.value = 80;
    ring.type = ItemType::ACCESSORY; ring.rarity = ItemRarity::UNCOMMON;
    applyAccessoryStats(ring);
    amulet.id = 31; amulet.name = "Amulet of Fortune"; amulet.value = 150;
    amulet.type = ItemType::ACCESSORY; amulet.rarity = ItemRarity::RARE;
    applyAccessoryStats(amulet);

    configureVendorTemplateItem(ring, level);
    configureVendorTemplateItem(amulet, level);
    
    if (level >= 2) shop.items.push_back(ring);
    if (level >= 4) shop.items.push_back(amulet);
    
    if (level >= 2) shop.stock[30] = 2;
    if (level >= 4) shop.stock[31] = 1;
    
    return shop;
}

MerchantInventory MerchantSystem::createBlackMarket(int level) {
    MerchantInventory shop;
    shop.name = "The Shadowy Corner";
    shop.greeting = "Psst... Looking for something... special?";
    shop.vendorType = VendorType::BLACK_MARKET;
    shop.merchantGold = 2000 + (level * 300);
    shop.buyPriceMultiplier = 2.5f;
    shop.sellPriceMultiplier = 0.7f;
    shop.acceptsAllItems = true;
    
    Item artifact;
    artifact.id = 50; artifact.name = "Ancient Artifact"; artifact.value = 1000;
    artifact.type = ItemType::SPECIAL; artifact.rarity = ItemRarity::LEGENDARY;

    configureVendorTemplateItem(artifact, level);
    
    if (level >= 6) {
        shop.items.push_back(artifact);
        shop.stock[50] = 1;
    }
    
    return shop;
}

MerchantInventory MerchantSystem::createTravelingMerchant(int level) {
    MerchantInventory shop;
    shop.name = "Traveling Merchant";
    shop.greeting = "Greetings traveler! I bring wares from distant lands.";
    shop.vendorType = VendorType::TRAVELING;
    shop.merchantGold = 1500 + (level * 250);
    shop.buyPriceMultiplier = 1.8f;
    shop.sellPriceMultiplier = 0.55f;
    shop.acceptsAllItems = true;
    
    Item potion, weapon, accessory;
    potion.id = 3; potion.name = "Greater Health Potion"; potion.value = 50;
    potion.type = ItemType::CONSUMABLE; potion.rarity = ItemRarity::RARE;
    weapon.id = 12; weapon.name = "Arcane Staff"; weapon.value = 100;
    weapon.type = ItemType::WEAPON; weapon.rarity = ItemRarity::RARE;
    accessory.id = 31; accessory.name = "Amulet of Fortune"; accessory.value = 150;
    accessory.type = ItemType::ACCESSORY; accessory.rarity = ItemRarity::RARE;
    applyAccessoryStats(accessory);

    configureVendorTemplateItem(potion, level);
    configureVendorTemplateItem(weapon, level);
    configureVendorTemplateItem(accessory, level);
    
    shop.items.push_back(potion);
    shop.items.push_back(weapon);
    shop.items.push_back(accessory);
    
    shop.stock[3] = 3;
    shop.stock[12] = 1;
    shop.stock[31] = 1;
    
    return shop;
}

}  // namespace AQE
