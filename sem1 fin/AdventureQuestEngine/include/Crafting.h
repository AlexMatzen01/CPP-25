





#ifndef CRAFTING_H
#define CRAFTING_H

#include "Types.h"
#include <vector>
#include <string>
#include <map>

namespace AQE {





enum class MaterialType {
    WOOD,
    METAL,
    LEATHER,
    CLOTH,
    GEM,
    ESSENCE,         
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
    int resultItemId;                         
    int resultQuantity;                       
    std::map<int, int> requiredMaterials;     
    int requiredGold;                         
    int requiredLevel;                        
    bool isDiscovered;                        
    std::string craftingStation;              
    float successChance;                      
    int experienceGained;                     
    
    Recipe() : id(0), resultItemId(0), resultQuantity(1), requiredGold(0),
               requiredLevel(1), isDiscovered(false), successChance(1.0f),
               experienceGained(10) {}
};





enum class EnchantmentType {
    FIRE_DAMAGE,         
    ICE_DAMAGE,          
    LIGHTNING_DAMAGE,    
    LIFESTEAL,           
    CRITICAL_STRIKE,     
    ARMOR_PENETRATION,   
    SPEED,               
    REGENERATION,        
    MANA_RECOVERY,       
    THORNS,              
    LUCK,                
    EXPERIENCE,          
    UNBREAKING,          
    SHARPNESS,           
    PROTECTION           
};

struct Enchantment {
    EnchantmentType type;
    int level;                       
    int power;                       
    std::string displayName;
    std::string description;
    int manaCostIncrease;            
    
    Enchantment() : type(EnchantmentType::SHARPNESS), level(1), 
                   power(0), manaCostIncrease(0) {}
};

struct EnchantedItem {
    Item baseItem;
    std::vector<Enchantment> enchantments;
    int maxEnchantments;             
    int upgradeLevel;                
    int durability;                  
    int maxDurability;
    
    EnchantedItem() : maxEnchantments(3), upgradeLevel(0), 
                     durability(100), maxDurability(100) {}
};





enum class VendorType {
    GENERAL_STORE,    
    WEAPONSMITH,      
    ARMORER,          
    APOTHECARY,       
    ENCHANTER,        
    BLACK_MARKET,     
    TRAVELING,        
    QUEST_VENDOR      
};

struct MerchantInventory {
    std::string name;
    std::string greeting;
    VendorType vendorType;
    std::vector<Item> items;
    std::map<int, int> stock;                
    std::map<int, float> priceMultipliers;   
    int merchantGold;
    float buyPriceMultiplier;                
    float sellPriceMultiplier;               
    int reputationLevel;                     
    int restockTurns;                        
    bool acceptsAllItems;                    
    
    MerchantInventory() : name("Merchant"), greeting("Welcome to my shop!"),
                         vendorType(VendorType::GENERAL_STORE),
                         merchantGold(1000), buyPriceMultiplier(1.5f),
                         sellPriceMultiplier(0.5f), reputationLevel(0),
                         restockTurns(0), acceptsAllItems(true) {}
};

struct TradeOffer {
    std::vector<int> offeredItemIds;         
    std::vector<int> requestedItemIds;       
    int goldDifference;                      
    bool isAcceptable;                       
    
    TradeOffer() : goldDifference(0), isAcceptable(false) {}
};





class CraftingSystem {
public:
    CraftingSystem(Player& player);
    
    
    void addMaterial(int materialId, int quantity);
    int getMaterialCount(int materialId) const;
    void displayMaterials() const;
    bool hasMaterials(const std::map<int, int>& required) const;
    void consumeMaterials(const std::map<int, int>& required);
    
    
    void displayRecipes() const;
    void displayDiscoveredRecipes() const;
    bool craftItem(int recipeId, int quantity = 1);
    bool canCraft(int recipeId) const;
    void discoverRecipe(int recipeId);
    void learnRecipeFromBook(int recipeBookId);
    
    
    bool enchantItem(int itemId, EnchantmentType enchantType);
    bool upgradeItem(int itemId);                    
    bool reforgeItem(int itemId, int targetItemLevel);  
    bool repairItem(int itemId);
    int getEnchantCost(EnchantmentType type, int level) const;
    void displayEnchantableItems() const;
    
    
    std::vector<CraftingMaterial> disenchantItem(int itemId);
    
    
    bool convertMaterial(int fromId, int toId, int quantity);
    
    
    void runCraftingMenu();
    
    
    std::string serializeCrafting() const;
    void deserializeCrafting(const std::string& data);
    
private:
    Player& playerRef;
    std::map<int, CraftingMaterial> materials;
    std::vector<Recipe> recipes;
    std::vector<EnchantedItem> enchantedItems;

    
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





class MerchantSystem {
public:
    MerchantSystem(Player& player);
    
    
    void displayShop(const MerchantInventory& merchant);
    bool buyItem(MerchantInventory& merchant, int slotIndex);
    bool sellItem(MerchantInventory& merchant, int slotIndex);
    int calculateBuyPrice(const Item& item, const MerchantInventory& merchant) const;
    int calculateSellPrice(const Item& item, const MerchantInventory& merchant) const;
    
    
    bool hasStock(const MerchantInventory& merchant, int itemId) const;
    void reduceStock(MerchantInventory& merchant, int itemId);
    void restockVendor(MerchantInventory& merchant);
    void addItemToStock(MerchantInventory& merchant, const Item& item, int quantity);
    
    
    MerchantInventory createVendor(VendorType type, int dungeonLevel);
    MerchantInventory createGeneralStore(int level);
    MerchantInventory createWeaponsmith(int level);
    MerchantInventory createArmorer(int level);
    MerchantInventory createApothecary(int level);
    MerchantInventory createEnchanter(int level);
    MerchantInventory createBlackMarket(int level);
    MerchantInventory createTravelingMerchant(int level);
    
    
    TradeOffer createTradeOffer(const std::vector<int>& offered,
                               const std::vector<int>& requested);
    bool executeTrade(MerchantInventory& merchant, const TradeOffer& trade);
    int evaluateItemValue(const Item& item) const;
    
    
    void runShopMenu(MerchantInventory& merchant);
    
    
    void updatePrices(MerchantInventory& merchant);
    float calculatePriceMultiplier(int itemId, int supply, int demand) const;
    void increaseReputation(int amount);
    float getReputationDiscount() const;
    
    
    void displayBlackMarket();                
    void displayTravelingMerchant();          
    bool unlockMerchantTier(int tier);        
    
private:
    Player& playerRef;
    int reputationPoints;
    int merchantTier;
    std::map<int, int> itemDemand;            
    
    void updateDemand(int itemId, int change);
    bool checkBarterSkill(int difficulty) const;
    void displayBuyMenu(MerchantInventory& merchant);
    void displaySellMenu(MerchantInventory& merchant);
    Item* findItemInVendorStock(MerchantInventory& merchant, int itemId);
    Item* findItemInPlayerInventory(int itemId);
};

}  

#endif 
