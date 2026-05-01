//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Achievement system (partially implemented - some features scrapped)

#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include "Types.h"
#include <vector>
#include <string>
#include <map>

namespace AQE {

// ---
// ACHIEVEMENT SYSTEM
// ---

enum class AchievementCategory {
    COMBAT,          // Kill X enemies, etc.
    EXPLORATION,     // Visit X rooms, find secrets
    COLLECTION,      // Collect items, complete quests
    MASTERY,         // Reach level X, max out skills
    CHALLENGE,       // Complete difficult tasks
    STORY,           // Story milestones
    HIDDEN           // Secret achievements
};

enum class AchievementTier {
    BRONZE,
    SILVER,
    GOLD,
    PLATINUM,
    DIAMOND
};

struct Achievement {
    int id;
    std::string name;
    std::string description;
    std::string secretDescription;       // Hidden until unlocked
    AchievementCategory category;
    AchievementTier tier;
    int rewardPoints;                    // Achievement points
    bool isUnlocked;
    bool isHidden;                       // Don't show until close to completion
    
    // Progress tracking
    int currentProgress;
    int requiredProgress;
    bool isPercentBased;                 // Show as X%?
    
    // Unlock rewards
    int goldReward;
    int experienceReward;
    std::vector<int> itemRewards;
    std::string unlockedPerkId;          // Permanent perk
    
    Achievement() : id(0), category(AchievementCategory::COMBAT),
                   tier(AchievementTier::BRONZE), rewardPoints(10),
                   isUnlocked(false), isHidden(false), currentProgress(0),
                   requiredProgress(1), isPercentBased(false),
                   goldReward(0), experienceReward(0) {}
};

// ---
// PERMANENT PERKS (META-PROGRESSION)
// ---

struct PermanentPerk {
    std::string id;
    std::string name;
    std::string description;
    bool isUnlocked;
    int achievementPointCost;
    
    // Bonuses (apply to ALL future runs)
    float startingGoldMultiplier;        // Start with more gold
    float startingXPMultiplier;          // Start with bonus XP
    int startingLevel;                   // Start at higher level
    bool startsWithRecipes;              // Know crafting recipes
    bool increasedDropRate;              // +10% drop rate
    float dropRateBonus;
    bool increasedCritChance;            // +5% crit
    int critChanceBonus;
    bool unlocksFastTravel;              // Can teleport to visited rooms
    bool unlocksStorage;                 // Bank storage between runs
    int storageSize;
    bool grantsVision;                   // See hidden rooms on map
    
    PermanentPerk() : isUnlocked(false), achievementPointCost(0),
                     startingGoldMultiplier(1.0f), startingXPMultiplier(1.0f),
                     startingLevel(1), startsWithRecipes(false),
                     increasedDropRate(false), dropRateBonus(0.0f),
                     increasedCritChance(false), critChanceBonus(0),
                     unlocksFastTravel(false), unlocksStorage(false),
                     storageSize(0), grantsVision(false) {}
};

// ---
// NEW GAME+ SYSTEM
// ---

struct NewGamePlusModifiers {
    int difficultyMultiplier;            // 1.5x, 2x, 3x enemy stats
    bool retainLevel;                    // Keep your level
    bool retainGold;                     // Keep your gold
    bool retainItems;                    // Keep your items
    bool retainSkills;                   // Keep talent tree
    
    // New challenges
    bool hasPermadeath;                  // One life only
    bool hasTimeLimit;                   // Must complete in X turns
    int turnLimit;
    bool hasNoHealing;                   // Can't heal consumables
    bool hasRandomizedClasses;           // Class changes each run
    bool hasCursedItems;                 // All items have drawbacks
    
    // Rewards
    float goldRewardMultiplier;          // 2x gold
    float xpRewardMultiplier;            // 2x XP
    bool unlocksBonusEnding;
    
    NewGamePlusModifiers() : difficultyMultiplier(1), retainLevel(false),
                            retainGold(false), retainItems(false),
                            retainSkills(false), hasPermadeath(false),
                            hasTimeLimit(false), turnLimit(0),
                            hasNoHealing(false), hasRandomizedClasses(false),
                            hasCursedItems(false), goldRewardMultiplier(1.0f),
                            xpRewardMultiplier(1.0f), unlocksBonusEnding(false) {}
};

// ---
// CHALLENGE MODES
// ---

enum class ChallengeMode {
    NONE,
    IRONMAN,             // Permadeath, no saves
    SPEEDRUN,            // Complete ASAP
    PACIFIST,            // Minimal combat
    POVERTY,             // Start with no gold, worse drops
    GLASS_CANNON,        // Double damage dealt and received
    SOLO,                // No companions
    COLLECTOR,           // Must collect all items
    MINIMALIST           // Limited inventory space
};

struct ChallengeRun {
    ChallengeMode mode;
    std::string name;
    std::string description;
    bool isActive;
    bool isCompleted;
    
    // Special rules
    std::map<std::string, bool> rules;
    
    // Completion rewards
    int achievementPoints;
    std::string unlockedTitle;
    int cosmeticRewardId;
    
    ChallengeRun() : mode(ChallengeMode::NONE), isActive(false),
                    isCompleted(false), achievementPoints(0),
                    cosmeticRewardId(-1) {}
};

// ---
// STATISTICS TRACKING
// ---

struct PlayerStatistics {
    // Combat stats
    int totalKills;
    int totalDamageDealt;
    int totalDamageTaken;
    int totalHealing;
    int criticalHits;
    int dodgedAttacks;
    int perfectDodges;                   // Dodged without taking damage
    int abilitiesUsed;
    int combosCompleted;
    
    // Exploration stats
    int roomsVisited;
    int secretsFound;
    int treasuresOpened;
    int trapsTriggered;
    int puzzlesSolved;
    int merchantsVisited;
    
    // Progression stats
    int questsCompleted;
    int achievementsUnlocked;
    int maxLevelReached;
    int totalGoldEarned;
    int totalGoldSpent;
    int itemsCrafted;
    int itemsEnchanted;
    
    // Run stats
    int totalRuns;
    int completedRuns;
    int fastestRunTurns;
    int longestSurvivalTurns;
    int currentStreak;                   // Consecutive wins
    int bestStreak;
    
    // Unique accomplishments
    std::vector<std::string> uniqueDeaths;  // How you died
    std::vector<std::string> endingsUnlocked;
    std::vector<std::string> titlesEarned;
    
    PlayerStatistics() : totalKills(0), totalDamageDealt(0), totalDamageTaken(0),
                        totalHealing(0), criticalHits(0), dodgedAttacks(0),
                        perfectDodges(0), abilitiesUsed(0), combosCompleted(0),
                        roomsVisited(0), secretsFound(0), treasuresOpened(0),
                        trapsTriggered(0), puzzlesSolved(0), merchantsVisited(0),
                        questsCompleted(0), achievementsUnlocked(0), maxLevelReached(1),
                        totalGoldEarned(0), totalGoldSpent(0), itemsCrafted(0),
                        itemsEnchanted(0), totalRuns(0), completedRuns(0),
                        fastestRunTurns(999999), longestSurvivalTurns(0),
                        currentStreak(0), bestStreak(0) {}
};

// ---
// ACHIEVEMENT MANAGER
// ---

class AchievementManager {
public:
    AchievementManager();
    
    // Achievement tracking
    void checkAchievements(const Player& player, const GameState& state);
    void updateProgress(int achievementId, int progress);
    void unlockAchievement(int achievementId);
    bool isUnlocked(int achievementId) const;
    
    // Display
    void displayAchievements() const;
    void displayAchievementsByCategory(AchievementCategory category) const;
    void displayRecentUnlocks() const;
    float getCompletionPercentage() const;
    
    // Rewards
    void claimAchievementReward(int achievementId, Player& player);
    int getTotalAchievementPoints() const;
    
    // Perks
    void displayPerks() const;
    bool unlockPerk(const std::string& perkId);
    bool hasPerk(const std::string& perkId) const;
    void applyPerksToNewRun(Player& player);
    
    // Save/Load
    std::string serializeAchievements() const;
    void deserializeAchievements(const std::string& data);
    
private:
    std::vector<Achievement> achievements;
    std::vector<PermanentPerk> perks;
    std::vector<Achievement> recentUnlocks;
    int totalPoints;
    int spentPoints;
    
    void initializeAchievements();
    void initializePerks();
    Achievement* findAchievement(int achievementId);
    PermanentPerk* findPerk(const std::string& perkId);
};

// ---
// STATISTICS MANAGER
// ---

class StatisticsManager {
public:
    StatisticsManager();
    
    // Tracking
    void recordKill();
    void recordDamage(int amount, bool dealt);
    void recordHealing(int amount);
    void recordCrit();
    void recordDodge();
    void recordAbility();
    void recordCombo();
    void recordRoomVisit();
    void recordSecret();
    void recordTreasure();
    void recordTrap();
    void recordPuzzle();
    void recordMerchant();
    void recordQuest();
    void recordCraft();
    void recordEnchant();
    void recordGold(int amount, bool earned);
    void recordRunStart();
    void recordRunEnd(bool won, int turns);
    void recordDeath(const std::string& cause);
    void recordEnding(const std::string& ending);
    void recordTitle(const std::string& title);
    
    // Display
    void displayStatistics() const;
    void displayCombatStats() const;
    void displayExplorationStats() const;
    void displayProgressStats() const;
    
    // Leaderboard
    void updateLeaderboard(const std::string& playerName, int score);
    void displayLeaderboard() const;
    
    // Save/Load
    std::string serializeStats() const;
    void deserializeStats(const std::string& data);
    
    const PlayerStatistics& getStats() const { return stats; }
    
private:
    PlayerStatistics stats;
    std::vector<std::pair<std::string, int>> leaderboard;  // Name, Score
    
    int calculateScore(const Player& player, int turns, bool won) const;
};

}  // namespace AQE

#endif // ACHIEVEMENT_H
