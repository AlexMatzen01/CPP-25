//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Companion and faction system (partially implemented - many features scrapped)

#ifndef COMPANION_H
#define COMPANION_H

#include "Types.h"
#include <vector>
#include <string>
#include <map>

namespace AQE {

// ---
// COMPANION SYSTEM
// ---

enum class CompanionClass {
    ARCHER,          // Ranged DPS
    HEALER,          // Support
    TANK,            // Defensive
    MAGE_SUPPORT,    // Buffs and crowd control
    THIEF,           // Utility and debuffs
    BEAST            // Pet companion
};

enum class CompanionMood {
    ANGRY,           // -20% effectiveness
    UNHAPPY,         // -10% effectiveness
    NEUTRAL,         // Normal
    HAPPY,           // +10% effectiveness
    DEVOTED          // +25% effectiveness, unlocks special ability
};

struct CompanionAbility {
    std::string name;
    std::string description;
    int cooldown;
    int currentCooldown;
    bool isPassive;                  // Always active?
    
    // Ability effects
    int damage;
    int healing;
    StatusEffect appliedEffect;
    bool targetsAllEnemies;          // AoE?
    float critChanceBonus;           // For passive abilities
    int shieldAmount;                // Shield on player
    
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
    
    // Relationship
    int loyaltyPoints;               // 0-100
    CompanionMood mood;
    int conversationLevel;           // Unlocks personal quests
    std::vector<int> personalQuestIds;
    std::vector<std::string> dialogueOptions;
    
    // Combat
    std::vector<CompanionAbility> abilities;
    int experienceLevel;
    bool isActiveInCombat;           // Deployed?
    
    // Preferences (affects loyalty)
    std::vector<std::string> likes;  // Actions they like
    std::vector<std::string> dislikes;
    int moralityPreference;          // Negative = evil, Positive = good, 0 = neutral
    
    // Special perks
    bool grantsXPBonus;              // +10% XP when active
    float xpBonusPercent;
    bool grantsGoldBonus;            // +10% gold when active
    float goldBonusPercent;
    bool grantsLootBonus;            // Better loot chance
    bool cantDie;                    // Cannot be killed (plot armor)
    
    Companion() : id(0), companionClass(CompanionClass::ARCHER), loyaltyPoints(50),
                  mood(CompanionMood::NEUTRAL), conversationLevel(0), experienceLevel(1),
                  isActiveInCombat(false), moralityPreference(0), grantsXPBonus(false),
                  xpBonusPercent(0.0f), grantsGoldBonus(false), goldBonusPercent(0.0f),
                  grantsLootBonus(false), cantDie(false) {}
};

// ---
// FACTION SYSTEM
// ---

enum class FactionId {
    KNIGHTS_ORDER,       // Lawful good
    THIEVES_GUILD,       // Chaotic neutral
    MAGES_CIRCLE,        // Neutral
    CULT_OF_SHADOWS,     // Evil
    MERCHANTS_LEAGUE,    // Neutral good
    DRAGON_RIDERS,       // Neutral good
    UNDEAD_LEGION        // Evil
};

enum class FactionRank {
    HOSTILE = -3,        // Attack on sight
    UNFRIENDLY = -2,     // Refuse service
    NEUTRAL = 0,         // Default
    FRIENDLY = 1,        // Small discounts
    HONORED = 2,         // Good discounts, special items
    REVERED = 3,         // Best discounts, unique quests
    EXALTED = 4          // Faction champion, best rewards
};

struct FactionReputation {
    FactionId faction;
    int reputationPoints;            // 0-10000
    FactionRank rank;
    std::vector<int> unlockedQuests;
    std::vector<int> unlockedItems;  // Special shop items
    bool hasTabard;                  // Can wear faction colors
    
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

// ---
// COMPANION MANAGER
// ---

class CompanionManager {
public:
    CompanionManager(Player& player);
    
    // Recruitment
    void displayAvailableCompanions() const;
    bool recruitCompanion(int companionId);
    bool dismissCompanion(int companionId);
    int getMaxCompanions() const;                    // Starts at 1, increases
    
    // Management
    void setActiveCompanion(int companionId);
    Companion* getActiveCompanion();
    void displayCompanionInfo(int companionId) const;
    void manageCompanions();                         // Interactive menu
    
    // Loyalty
    void changeLoyalty(int companionId, int amount, const std::string& reason);
    void updateMood(int companionId);
    void checkLoyaltyEvents();                       // Random loyalty checks
    
    // Combat integration
    void companionTakeTurn(Enemy& enemy, std::vector<std::string>& combatLog);
    void healCompanion(int companionId, int amount);
    void companionLevelUp(int companionId);
    
    // Conversation
    void talkToCompanion(int companionId);
    void giveGift(int companionId, int itemId);
    void unlockPersonalQuest(int companionId);
    
    // Bonuses
    float getTotalXPBonus() const;
    float getTotalGoldBonus() const;
    bool hasActiveLootBonus() const;
    
    //==========================================================================
    // UTILITY METHODS - Status & Checking
    //==========================================================================
    
    /**
// Check if companion is recruited
     * @param companionId Companion to check
     * @return True if recruited
     */
    bool isRecruited(int companionId) const;
    
    /**
// Get total recruited companions
     * @return Number of companions in party
     */
    int getRecruitedCount() const;
    
    /**
// Get companion by ID
     * @param companionId Companion ID to search
     * @return Pointer to companion or nullptr
     */
    Companion* getCompanionById(int companionId);
    
    /**
// Get companion's loyalty level
     * @param companionId Companion to check
     * @return Loyalty points (0-100)
     */
    int getCompanionLoyalty(int companionId) const;
    
    /**
// Get companion's mood
     * @param companionId Companion to check
     * @return Current mood
     */
    CompanionMood getCompanionMood(int companionId) const;
    
    /**
// Check if companion is in active party
     * @param companionId Companion to check
     * @return True if active
     */
    bool isCompanionActive(int companionId) const;
    
    /**
// Get all active companion bonuses
     * @return Combined bonuses string
     */
    std::string getActiveBonusesText() const;
    
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

// ---
// FACTION MANAGER
// ---

class FactionManager {
public:
    FactionManager(Player& player);
    
    // Reputation
    void gainReputation(FactionId faction, int amount, const std::string& reason);
    void loseReputation(FactionId faction, int amount, const std::string& reason);
    FactionRank getFactionRank(FactionId faction) const;
    int getReputationPoints(FactionId faction) const;
    
    // Ranks
    void checkRankUp(FactionId faction);
    void displayReputationStatus() const;
    
    // Rewards
    std::vector<int> getAvailableQuests(FactionId faction) const;
    std::vector<Item> getFactionShop(FactionId faction) const;
    bool unlockFactionAbility(FactionId faction);    // Special abilities
    
    // Conflicts
    bool areFactionsAllied(FactionId faction1, FactionId faction2) const;
    void checkFactionConflicts(FactionId gainedRep);  // Losing rep elsewhere
    void declareFactionWar(FactionId faction);       // Go hostile
    
    // Titles and cosmetics
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

}  // namespace AQE

#endif // COMPANION_H
