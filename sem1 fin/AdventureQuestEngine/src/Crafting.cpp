//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Crafting system implementation (partially implemented - many features scrapped)

#include "../include/Crafting.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cstdlib>

namespace AQE {

// ---
// CraftingSystem Implementation
// ---

CraftingSystem::CraftingSystem(Player& player) : playerRef(player) {
    initializeMaterials();
    initializeRecipes();
}

void CraftingSystem::addMaterial(int materialId, int quantity) {
    if (materials.find(materialId) != materials.end()) {
        materials[materialId].quantity += quantity;
    } else {
        CraftingMaterial mat;
        mat.id = materialId;
        mat.quantity = quantity;
        materials[materialId] = mat;
    }
    std::cout << "Added " << quantity << " materials.\n";
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
    craftedItem.id = recipe->resultItemId;
    craftedItem.name = recipe->name;
    craftedItem.description = "Crafted item: " + recipe->description;
    craftedItem.value = recipe->requiredGold * 2;
    
    playerRef.inventory.push_back(craftedItem);
    playerRef.experience += recipe->experienceGained;
    
    std::cout << "Successfully crafted: " << recipe->name << "!\n";
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
    
    int cost = getEnchantCost(enchantType, 1);
    if (playerRef.gold < cost) {
        std::cout << "Not enough gold! Need " << cost << "\n";
        return false;
    }
    
    playerRef.gold -= cost;
    
    // Create enchantment
    Enchantment enchant;
    enchant.type = enchantType;
    enchant.level = 1;
    
    switch (enchantType) {
        case EnchantmentType::FIRE_DAMAGE:
            enchant.displayName = "Fiery";
            enchant.power = 10;
            break;
        case EnchantmentType::ICE_DAMAGE:
            enchant.displayName = "Frozen";
            enchant.power = 8;
            break;
        case EnchantmentType::LIFESTEAL:
            enchant.displayName = "Vampiric";
            enchant.power = 5;
            break;
        case EnchantmentType::CRITICAL_STRIKE:
            enchant.displayName = "Precise";
            enchant.power = 5;
            break;
        case EnchantmentType::SHARPNESS:
            enchant.displayName = "Sharp";
            enchant.power = 10;
            break;
        case EnchantmentType::PROTECTION:
            enchant.displayName = "Protected";
            enchant.power = 8;
            break;
        default:
            enchant.displayName = "Enchanted";
            enchant.power = 5;
    }
    
    // Apply to item name
    item->name = enchant.displayName + " " + item->name;
    item->value += cost;
    
    std::cout << "Successfully enchanted: " << item->name << "!\n";
    return true;
}

bool CraftingSystem::upgradeItem(int itemId) {
    // Find item in inventory
    for (auto& item : playerRef.inventory) {
        if (item.id == itemId) {
            int upgradeCost = item.value / 2;
            if (playerRef.gold < upgradeCost) {
                std::cout << "Not enough gold! Need " << upgradeCost << "\n";
                return false;
            }
            
            playerRef.gold -= upgradeCost;
            item.name = item.name + " +1";
            item.value += upgradeCost;
            
            std::cout << "Item upgraded to: " << item.name << "!\n";
            return true;
        }
    }
    
    std::cout << "Item not found!\n";
    return false;
}

bool CraftingSystem::repairItem(int itemId) {
    // Find item in inventory
    for (auto& item : playerRef.inventory) {
        if (item.id == itemId) {
            int repairCost = item.value / 4;
            if (playerRef.gold < repairCost) {
                std::cout << "Not enough gold! Need " << repairCost << "\n";
                return false;
            }
            
            playerRef.gold -= repairCost;
            std::cout << "Item repaired!\n";
            return true;
        }
    }
    
    std::cout << "Item not found!\n";
    return false;
}

int CraftingSystem::getEnchantCost(EnchantmentType type, int level) const {
    int baseCost = 100;
    
    switch (type) {
        case EnchantmentType::FIRE_DAMAGE:
        case EnchantmentType::ICE_DAMAGE:
        case EnchantmentType::LIGHTNING_DAMAGE:
            baseCost = 150;
            break;
        case EnchantmentType::LIFESTEAL:
            baseCost = 300;
            break;
        case EnchantmentType::CRITICAL_STRIKE:
            baseCost = 200;
            break;
        case EnchantmentType::UNBREAKING:
            baseCost = 250;
            break;
        default:
            baseCost = 100;
    }
    
    return baseCost * level;
}

void CraftingSystem::displayEnchantableItems() const {
    std::cout << "\n========================================\n";
    std::cout << "       ENCHANTABLE ITEMS\n";
    std::cout << "========================================\n\n";
    
    for (const auto& item : playerRef.inventory) {
        if (item.type == ItemType::WEAPON || item.type == ItemType::ARMOR) {
            std::cout << "[" << item.id << "] " << item.name << "\n";
        }
    }
}

std::vector<CraftingMaterial> CraftingSystem::disenchantItem(int itemId) {
    std::vector<CraftingMaterial> result;
    
    // Find and remove item
    for (auto it = playerRef.inventory.begin(); it != playerRef.inventory.end(); ++it) {
        if (it->id == itemId) {
            // Return some materials based on item value
            CraftingMaterial mat;
            mat.id = 1;
            mat.name = "Essence";
            mat.quantity = it->value / 50;
            result.push_back(mat);
            
            addMaterial(1, mat.quantity);
            
            playerRef.inventory.erase(it);
            std::cout << "Item disenchanted! Gained " << mat.quantity << " essence.\n";
            break;
        }
    }
    
    return result;
}

bool CraftingSystem::convertMaterial(int fromId, int toId, int quantity) {
    if (getMaterialCount(fromId) < quantity * 10) {
        std::cout << "Not enough materials to convert!\n";
        return false;
    }
    
    materials[fromId].quantity -= quantity * 10;
    addMaterial(toId, quantity);
    
    std::cout << "Converted materials successfully!\n";
    return true;
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
    // Basic Weapon Recipes
    Recipe ironSword;
    ironSword.id = 1001;
    ironSword.name = "Iron Sword";
    ironSword.description = "A basic iron sword";
    ironSword.resultItemId = 3001;
    ironSword.resultQuantity = 1;
    ironSword.requiredMaterials = {{1, 5}, {2, 3}};  // 5 iron, 3 wood
    ironSword.requiredGold = 50;
    ironSword.requiredLevel = 1;
    ironSword.isDiscovered = true;  // Starting recipe
    ironSword.craftingStation = "Forge";
    ironSword.successChance = 0.9f;
    ironSword.experienceGained = 20;
    recipes.push_back(ironSword);
    
    Recipe steelSword;
    steelSword.id = 1002;
    steelSword.name = "Steel Sword";
    steelSword.description = "A sturdy steel sword";
    steelSword.resultItemId = 3002;
    steelSword.resultQuantity = 1;
    steelSword.requiredMaterials = {{1, 10}, {2, 5}};
    steelSword.requiredGold = 150;
    steelSword.requiredLevel = 5;
    steelSword.isDiscovered = false;
    steelSword.craftingStation = "Forge";
    steelSword.successChance = 0.8f;
    steelSword.experienceGained = 50;
    recipes.push_back(steelSword);
    
    // Basic Armor Recipes
    Recipe leatherArmor;
    leatherArmor.id = 1010;
    leatherArmor.name = "Leather Armor";
    leatherArmor.description = "Light leather armor";
    leatherArmor.resultItemId = 4001;
    leatherArmor.resultQuantity = 1;
    leatherArmor.requiredMaterials = {{3, 8}};  // 8 leather
    leatherArmor.requiredGold = 40;
    leatherArmor.requiredLevel = 1;
    leatherArmor.isDiscovered = true;
    leatherArmor.craftingStation = "Workbench";
    leatherArmor.successChance = 0.95f;
    leatherArmor.experienceGained = 15;
    recipes.push_back(leatherArmor);
    
    // Potion Recipes
    Recipe healthPotion;
    healthPotion.id = 1020;
    healthPotion.name = "Health Potion";
    healthPotion.description = "Restores 50 HP";
    healthPotion.resultItemId = 5001;
    healthPotion.resultQuantity = 1;
    healthPotion.requiredMaterials = {{7, 3}};  // 3 herbs
    healthPotion.requiredGold = 20;
    healthPotion.requiredLevel = 1;
    healthPotion.isDiscovered = true;
    healthPotion.craftingStation = "Alchemy Lab";
    healthPotion.successChance = 0.85f;
    healthPotion.experienceGained = 10;
    recipes.push_back(healthPotion);
    
    Recipe manaPotion;
    manaPotion.id = 1021;
    manaPotion.name = "Mana Potion";
    manaPotion.description = "Restores 30 Mana";
    manaPotion.resultItemId = 5002;
    manaPotion.resultQuantity = 1;
    manaPotion.requiredMaterials = {{7, 2}, {6, 1}};  // 2 herbs, 1 essence
    manaPotion.requiredGold = 30;
    manaPotion.requiredLevel = 3;
    manaPotion.isDiscovered = false;
    manaPotion.craftingStation = "Alchemy Lab";
    manaPotion.successChance = 0.80f;
    manaPotion.experienceGained = 15;
    recipes.push_back(manaPotion);
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
    std::cout << "           MERCHANT SHOP\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Your Gold: " << playerRef.gold << "\n";
    std::cout << "Reputation Discount: " << static_cast<int>(getReputationDiscount() * 100) << "%\n\n";
    
    std::cout << "--- FOR SALE ---\n";
    for (const auto& item : merchant.items) {
        float multiplier = merchant.buyPriceMultiplier;
        auto it = merchant.priceMultipliers.find(item.id);
        if (it != merchant.priceMultipliers.end()) {
            multiplier *= it->second;
        }
        
        int price = static_cast<int>(item.value * multiplier * (1.0f - getReputationDiscount()));
        std::cout << "[" << item.id << "] " << item.name;
        std::cout << " - " << price << " gold\n";
    }
}

bool MerchantSystem::buyItem(MerchantInventory& merchant, int itemId) {
    for (auto it = merchant.items.begin(); it != merchant.items.end(); ++it) {
        if (it->id == itemId) {
            float multiplier = merchant.buyPriceMultiplier;
            auto priceIt = merchant.priceMultipliers.find(itemId);
            if (priceIt != merchant.priceMultipliers.end()) {
                multiplier *= priceIt->second;
            }
            
            int price = static_cast<int>(it->value * multiplier * (1.0f - getReputationDiscount()));
            
            if (playerRef.gold < price) {
                std::cout << "Not enough gold! Need " << price << "\n";
                return false;
            }
            
            playerRef.gold -= price;
            merchant.merchantGold += price;
            playerRef.inventory.push_back(*it);
            merchant.items.erase(it);
            
            std::cout << "Purchased item!\n";
            updateDemand(itemId, 1);  // Increase demand
            return true;
        }
    }
    
    std::cout << "Item not available!\n";
    return false;
}

bool MerchantSystem::sellItem(MerchantInventory& merchant, int itemId) {
    for (auto it = playerRef.inventory.begin(); it != playerRef.inventory.end(); ++it) {
        if (it->id == itemId) {
            int price = static_cast<int>(it->value * merchant.sellPriceMultiplier);
            
            if (merchant.merchantGold < price) {
                std::cout << "Merchant doesn't have enough gold!\n";
                return false;
            }
            
            playerRef.gold += price;
            merchant.merchantGold -= price;
            merchant.items.push_back(*it);
            playerRef.inventory.erase(it);
            
            std::cout << "Sold item for " << price << " gold!\n";
            updateDemand(itemId, -1);  // Decrease demand
            return true;
        }
    }
    
    std::cout << "You don't have that item!\n";
    return false;
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

}  // namespace AQE
