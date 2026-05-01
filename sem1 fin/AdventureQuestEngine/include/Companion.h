





#ifndef COMPANION_H
#define COMPANION_H

#include "Types.h"
#include <vector>
#include <string>
#include <map>

namespace AQE {





enum class CompanionClass {
    ARCHER,          
    HEALER,          
    TANK,            
    MAGE_SUPPORT,    
    THIEF,           
    BEAST            
};

enum class CompanionMood {
    ANGRY,           
    UNHAPPY,         
    NEUTRAL,         
    HAPPY,           
    DEVOTED          
};

struct CompanionAbility {
    std::string name;
    std::string description;
    int cooldown;
    int currentCooldown;
    bool isPassive;                  
    
    
    int damage;
    int healing;
    StatusEffect appliedEffect;
    bool targetsAllEnemies;          
    float critChanceBonus;           
    int shieldAmount;                
    
    CompanionAbility() : cooldown(3), currentCooldown(0), isPassive(false),
                        damage(0), healing(0), targetsAllEnemies(false),
                        critChanceBonus(0.0f), shieldAmount(0) {}
};

struct Companion {
    int id;
    std::string name;
    std::string backstory;
    CompanionClass companionClass;
    CombatStats stats;
    
    
    int loyaltyPoints;               
    CompanionMood mood;
    int conversationLevel;           
    std::vector<int> personalQuestIds;
    std::vector<std::string> dialogueOptions;
    
    
    std::vector<CompanionAbility> abilities;
    int experienceLevel;
    bool isActiveInCombat;           
    
    
    std::vector<std::string> likes;  
    std::vector<std::string> dislikes;
    int moralityPreference;          
    
    
    bool grantsXPBonus;              
    float xpBonusPercent;
    bool grantsGoldBonus;            
    float goldBonusPercent;
    bool grantsLootBonus;            
    bool cantDie;                    
    
    Companion() : id(0), companionClass(CompanionClass::ARCHER), loyaltyPoints(50),
                  mood(CompanionMood::NEUTRAL), conversationLevel(0), experienceLevel(1),
                  isActiveInCombat(false), moralityPreference(0), grantsXPBonus(false),
                  xpBonusPercent(0.0f), grantsGoldBonus(false), goldBonusPercent(0.0f),
                  grantsLootBonus(false), cantDie(false) {}
};





enum class FactionId {
    KNIGHTS_ORDER,       
    THIEVES_GUILD,       
    MAGES_CIRCLE,        
    CULT_OF_SHADOWS,     
    MERCHANTS_LEAGUE,    
    DRAGON_RIDERS,       
    UNDEAD_LEGION        
};

enum class FactionRank {
    HOSTILE = -3,        
    UNFRIENDLY = -2,     
    NEUTRAL = 0,         
    FRIENDLY = 1,        
    HONORED = 2,         
    REVERED = 3,         
    EXALTED = 4          
};

struct FactionReputation {
    FactionId faction;
    int reputationPoints;            
    FactionRank rank;
    std::vector<int> unlockedQuests;
    std::vector<int> unlockedItems;  
    bool hasTabard;                  
    
    FactionReputation() : faction(FactionId::KNIGHTS_ORDER), reputationPoints(0),
                         rank(FactionRank::NEUTRAL), hasTabard(false) {}
};

struct FactionReward {
    int goldAmount;
    int experienceAmount;
    std::vector<int> itemIds;
    int reputationGain;
    bool grantsTitle;
    std::string titleName;
    
    FactionReward() : goldAmount(0), experienceAmount(0), reputationGain(0),
                     grantsTitle(false) {}
};





class CompanionManager {
public:
    CompanionManager(Player& player);
    
    
    void displayAvailableCompanions() const;
    bool recruitCompanion(int companionId);
    bool dismissCompanion(int companionId);
    int getMaxCompanions() const;                    
    
    
    void setActiveCompanion(int companionId);
    Companion* getActiveCompanion();
    void displayCompanionInfo(int companionId) const;
    void manageCompanions();                         
    
    
    void changeLoyalty(int companionId, int amount, const std::string& reason);
    void updateMood(int companionId);
    void checkLoyaltyEvents();                       
    
    
    void companionTakeTurn(Enemy& enemy, std::vector<std::string>& combatLog);
    void healCompanion(int companionId, int amount);
    void companionLevelUp(int companionId);
    
    
    void talkToCompanion(int companionId);
    void giveGift(int companionId, int itemId);
    void unlockPersonalQuest(int companionId);
    
    
    float getTotalXPBonus() const;
    float getTotalGoldBonus() const;
    bool hasActiveLootBonus() const;
    
private:
    Player& playerRef;
    std::vector<Companion> allCompanions;
    std::vector<int> recruitedCompanionIds;
    int activeCompanionId;
    int maxActiveCompanions;
    
    void initializeCompanions();
    Companion* findCompanion(int companionId);
    void triggerLoyaltyDialogue(int companionId, bool isPositive);
};





class FactionManager {
public:
    FactionManager(Player& player);
    
    
    void gainReputation(FactionId faction, int amount, const std::string& reason);
    void loseReputation(FactionId faction, int amount, const std::string& reason);
    FactionRank getFactionRank(FactionId faction) const;
    int getReputationPoints(FactionId faction) const;
    
    
    void checkRankUp(FactionId faction);
    void displayReputationStatus() const;
    
    
    std::vector<int> getAvailableQuests(FactionId faction) const;
    std::vector<Item> getFactionShop(FactionId faction) const;
    bool unlockFactionAbility(FactionId faction);    
    
    
    bool areFactionsAllied(FactionId faction1, FactionId faction2) const;
    void checkFactionConflicts(FactionId gainedRep);  
    void declareFactionWar(FactionId faction);       
    
    
    std::string getCurrentTitle(FactionId faction) const;
    bool canWearTabard(FactionId faction) const;
    
private:
    Player& playerRef;
    std::map<FactionId, FactionReputation> factionReps;
    std::map<FactionId, std::vector<FactionId>> alliedFactions;
    std::map<FactionId, std::vector<FactionId>> enemyFactions;
    
    void initializeFactions();
    FactionRank calculateRank(int reputationPoints) const;
    void applyFactionConflict(FactionId faction1, FactionId faction2, int amount);
};

}  

#endif 
