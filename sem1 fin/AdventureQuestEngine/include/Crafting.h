//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Crafting and economy system (partially implemented - many features scrapped)

#ifndef CRAFTING_H
#define CRAFTING_H

#include "Types.h"
#include <vector>
#include <string>
#include <map>

namespace AQE {

// ---
// CRAFTING MATERIALS AND RECIPES
// ---

enum class MaterialType {
    WOOD,
    METAL,
    LEATHER,
    CLOTH,
    GEM,
    ESSENCE,         // Magical crafting component
    HERB,
    BONE,
    DRAGON_SCALE,
    CRYSTAL
};

struct CraftingMaterial {
    int id;
    std::string name;
    MaterialType type;
    ItemRarity rarity;
    int quantity;
    std::string description;
    
    CraftingMaterial() : id(0), type(MaterialType::WOOD), 
                        rarity(ItemRarity::COMMON), quantity(0) {}
};

struct Recipe {
    int id;
    std::string name;
    std::string description;
    int resultItemId;                         // What you craft
    int resultQuantity;                       // How many you get
    std::map<int, int> requiredMaterials;     // MaterialID -> Quantity
    int requiredGold;                         // Gold cost
    int requiredLevel;                        // Player level requirement
    bool isDiscovered;                        // Has player found recipe?
    std::string craftingStation;              // "Forge", "Alchemy Lab", etc.
    float successChance;                      // Chance to succeed (0-1)
    int experienceGained;                     // XP for crafting
    
    Recipe() : id(0), resultItemId(0), resultQuantity(1), requiredGold(0),
               requiredLevel(1), isDiscovered(false), successChance(1.0f),
               experienceGained(10) {}
};

// ---
// ENCHANTING SYSTEM
// ---

enum class EnchantmentType {
    FIRE_DAMAGE,         // +X fire damage
    ICE_DAMAGE,          // +X ice damage + slow
    LIGHTNING_DAMAGE,    // +X lightning + chain
    LIFESTEAL,           // Heal X% of damage dealt
    CRITICAL_STRIKE,     // +X% crit chance
    ARMOR_PENETRATION,   // Ignore X defense
    SPEED,               // +X speed
    REGENERATION,        // +X HP per turn
    MANA_RECOVERY,       // +X mana per turn
    THORNS,              // Reflect X damage
    LUCK,                // +X% better loot
    EXPERIENCE,          // +X% XP gain
    UNBREAKING,          // Item doesn't degrade
    SHARPNESS,           // +X% damage
    PROTECTION           // +X% damage reduction
};

struct Enchantment {
    EnchantmentType type;
    int level;                       // 1-5 enchantment level
    int power;                       // Strength of effect
    std::string displayName;
    std::string description;
    int manaCostIncrease;            // For weapons
    
    Enchantment() : type(EnchantmentType::SHARPNESS), level(1), 
                   power(0), manaCostIncrease(0) {}
};

struct EnchantedItem {
    Item baseItem;
    std::vector<Enchantment> enchantments;
    int maxEnchantments;             // Usually 3
    int upgradeLevel;                // +0 to +10
    int durability;                  // Degrades with use
    int maxDurability;
    
    EnchantedItem() : maxEnchantments(3), upgradeLevel(0), 
                     durability(100), maxDurability(100) {}
};

// ---
// MERCHANT AND ECONOMY
// ---

enum class VendorType {
    GENERAL_STORE,    // Basic items, potions, food
    WEAPONSMITH,      // Weapons and weapon upgrades
    ARMORER,          // Armor and defensive gear
    APOTHECARY,       // Potions, healing items, buffs
    ENCHANTER,        // Magical items and enchantments
    BLACK_MARKET,     // Rare/illegal items, high prices
    TRAVELING,        // Random rare goods, changes stock
    QUEST_VENDOR      // Quest-related items only
};

struct MerchantInventory {
    std::string name;
    std::string greeting;
    VendorType vendorType;
    std::vector<Item> items;
    std::map<int, int> stock;                // ItemID -> Quantity (-1 = infinite)
    std::map<int, float> priceMultipliers;   // ItemID -> Price multiplier
    int merchantGold;
    float buyPriceMultiplier;                // Usually 1.5x
    float sellPriceMultiplier;               // Usually 0.5x
    int reputationLevel;                     // Better deals at high rep
    int restockTurns;                        // Turns until restock
    bool acceptsAllItems;                    // Will buy any item?
    
    MerchantInventory() : name("Merchant"), greeting("Welcome to my shop!"),
                         vendorType(VendorType::GENERAL_STORE),
                         merchantGold(1000), buyPriceMultiplier(1.5f),
                         sellPriceMultiplier(0.5f), reputationLevel(0),
                         restockTurns(0), acceptsAllItems(true) {}
};

struct TradeOffer {
    std::vector<int> offeredItemIds;         // What player offers
    std::vector<int> requestedItemIds;       // What merchant wants
    int goldDifference;                      // + player pays, - player receives
    bool isAcceptable;                       // Merchant accepts?
    
    TradeOffer() : goldDifference(0), isAcceptable(false) {}
};

// ---
// CRAFTING SYSTEM MANAGER
// ---

class CraftingSystem {
public:
    CraftingSystem(Player& player);
    
    // Material management
    void addMaterial(int materialId, int quantity);
    int getMaterialCount(int materialId) const;
    void displayMaterials() const;
    bool hasMaterials(const std::map<int, int>& required) const;
    void consumeMaterials(const std::map<int, int>& required);
    
    // Crafting
    void displayRecipes() const;
    void displayDiscoveredRecipes() const;
    bool craftItem(int recipeId, int quantity = 1);
    bool canCraft(int recipeId) const;
    void discoverRecipe(int recipeId);
    void learnRecipeFromBook(int recipeBookId);
    
    // Enchanting
    bool enchantItem(int itemId, EnchantmentType enchantType);
    bool upgradeItem(int itemId);                    // +1 upgrade level
    bool reforgeItem(int itemId, int targetItemLevel);  // Raise iLvl for aging legendary gear
    bool repairItem(int itemId);
    int getEnchantCost(EnchantmentType type, int level) const;
    void displayEnchantableItems() const;
    
    // Disenchanting (break item for materials)
    std::vector<CraftingMaterial> disenchantItem(int itemId);
    
    // Material conversion (e.g., 10 wood -> 1 refined wood)
    bool convertMaterial(int fromId, int toId, int quantity);
    
    // Interactive Menu
    void runCraftingMenu();
    
    // Save/Load
    std::string serializeCrafting() const;
    void deserializeCrafting(const std::string& data);
    
    //==========================================================================
    // UTILITY METHODS - Status & Checking
    //==========================================================================
    
    /**
// Check if recipe is known
     * @param recipeId Recipe ID to check
     * @return True if discovered
     */
    bool isRecipeDiscovered(int recipeId) const;
    
    /**
// Get recipe by ID
     * @param recipeId Recipe ID to search for
     * @return Pointer to recipe or nullptr
     */
    Recipe* getRecipeById(int recipeId);
    
    /**
// Get crafting skill level
     * @return Current crafting level
     */
    int getCraftingLevel() const;
    
    /**
// Get material count
     * @param materialId Material to count
     * @return Quantity of material owned
     */
    int getMaterialQuantity(int materialId) const;
    
    /**
// Check if recipe requirements are met
     * @param recipeId Recipe to check
     * @return True if can be crafted now
     */
    bool canCraftRecipe(int recipeId) const;
    
    /**
// Get total number of discovered recipes
     * @return Count of known recipes
     */
    int getDiscoveredRecipeCount() const;
    
    /**
// Get success rate for recipe
     * @param recipeId Recipe to check
     * @return Success percentage (0-100)
     */
    float getRecipeSuccessRate(int recipeId) const;
    
private:
    Player& playerRef;
    std::map<int, CraftingMaterial> materials;
    std::vector<Recipe> recipes;
    std::vector<EnchantedItem> enchantedItems;

    // Crafting progression and mastery
    int craftingSkillLevel;
    int craftingSkillXP;
    int totalCraftAttempts;
    int totalCraftSuccesses;
    int consecutiveFailures;
    std::map<int, int> recipeMastery;
    std::map<std::string, int> stationMastery;
    
    void initializeRecipes();
    void initializeMaterials();
    Recipe* findRecipe(int recipeId);
    float calculateSuccessChance(const Recipe& recipe) const;
    int calculateMaterialPreservationChance(const Recipe& recipe) const;
    int calculateMaxCraftableByMaterials(const Recipe& recipe) const;
    int getRecipeMasteryLevel(int recipeId) const;
    int getStationMasteryLevel(const std::string& stationName) const;
    int xpToNextCraftingLevel() const;
    void addCraftingXP(int amount);
    void displayCraftingProgress() const;
    Item buildCraftedItem(const Recipe& recipe, bool masterwork) const;
};

// ---
// MERCHANT SYSTEM
// ---

class MerchantSystem {
public:
    MerchantSystem(Player& player);
    
    // Shopping
    void displayShop(const MerchantInventory& merchant);
    bool buyItem(MerchantInventory& merchant, int slotIndex);
    bool sellItem(MerchantInventory& merchant, int slotIndex);
    int calculateBuyPrice(const Item& item, const MerchantInventory& merchant) const;
    int calculateSellPrice(const Item& item, const MerchantInventory& merchant) const;
    
    // Stock management
    bool hasStock(const MerchantInventory& merchant, int itemId) const;
    void reduceStock(MerchantInventory& merchant, int itemId);
    void restockVendor(MerchantInventory& merchant);
    void addItemToStock(MerchantInventory& merchant, const Item& item, int quantity);
    
    // Vendor creation and types
    MerchantInventory createVendor(VendorType type, int dungeonLevel);
    MerchantInventory createGeneralStore(int level);
    MerchantInventory createWeaponsmith(int level);
    MerchantInventory createArmorer(int level);
    MerchantInventory createApothecary(int level);
    MerchantInventory createEnchanter(int level);
    MerchantInventory createBlackMarket(int level);
    MerchantInventory createTravelingMerchant(int level);
    
    // Trading/bartering
    TradeOffer createTradeOffer(const std::vector<int>& offered,
                               const std::vector<int>& requested);
    bool executeTrade(MerchantInventory& merchant, const TradeOffer& trade);
    int evaluateItemValue(const Item& item) const;
    
    // Interactive menu
    void runShopMenu(MerchantInventory& merchant);
    
    // Dynamic pricing
    void updatePrices(MerchantInventory& merchant);
    float calculatePriceMultiplier(int itemId, int supply, int demand) const;
    void increaseReputation(int amount);
    float getReputationDiscount() const;
    
    // Special merchants
    void displayBlackMarket();                // Rare illegal items
    void displayTravelingMerchant();          // Random rare goods
    bool unlockMerchantTier(int tier);        // Access better items
    
private:
    Player& playerRef;
    int reputationPoints;
    int merchantTier;
    std::map<int, int> itemDemand;            // ItemID -> Demand level
    
    void updateDemand(int itemId, int change);
    bool checkBarterSkill(int difficulty) const;
    void displayBuyMenu(MerchantInventory& merchant);
    void displaySellMenu(MerchantInventory& merchant);
    Item* findItemInVendorStock(MerchantInventory& merchant, int itemId);
    Item* findItemInPlayerInventory(int itemId);
};

}  // namespace AQE

#endif // CRAFTING_H
