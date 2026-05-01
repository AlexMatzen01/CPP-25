





#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include "Types.h"
#include <vector>
#include <string>
#include <map>

namespace AQE {





enum class AchievementCategory {
    COMBAT,          
    EXPLORATION,     
    COLLECTION,      
    MASTERY,         
    CHALLENGE,       
    STORY,           
    HIDDEN           
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
    std::string secretDescription;       
    AchievementCategory category;
    AchievementTier tier;
    int rewardPoints;                    
    bool isUnlocked;
    bool isHidden;                       
    
    
    int currentProgress;
    int requiredProgress;
    bool isPercentBased;                 
    
    
    int goldReward;
    int experienceReward;
    std::vector<int> itemRewards;
    std::string unlockedPerkId;          
    
    Achievement() : id(0), category(AchievementCategory::COMBAT),
                   tier(AchievementTier::BRONZE), rewardPoints(10),
                   isUnlocked(false), isHidden(false), currentProgress(0),
                   requiredProgress(1), isPercentBased(false),
                   goldReward(0), experienceReward(0) {}
};





struct PermanentPerk {
    std::string id;
    std::string name;
    std::string description;
    bool isUnlocked;
    int achievementPointCost;
    
    
    float startingGoldMultiplier;        
    float startingXPMultiplier;          
    int startingLevel;                   
    bool startsWithRecipes;              
    bool increasedDropRate;              
    float dropRateBonus;
    bool increasedCritChance;            
    int critChanceBonus;
    bool unlocksFastTravel;              
    bool unlocksStorage;                 
    int storageSize;
    bool grantsVision;                   
    
    PermanentPerk() : isUnlocked(false), achievementPointCost(0),
                     startingGoldMultiplier(1.0f), startingXPMultiplier(1.0f),
                     startingLevel(1), startsWithRecipes(false),
                     increasedDropRate(false), dropRateBonus(0.0f),
                     increasedCritChance(false), critChanceBonus(0),
                     unlocksFastTravel(false), unlocksStorage(false),
                     storageSize(0), grantsVision(false) {}
};





struct NewGamePlusModifiers {
    int difficultyMultiplier;            
    bool retainLevel;                    
    bool retainGold;                     
    bool retainItems;                    
    bool retainSkills;                   
    
    
    bool hasPermadeath;                  
    bool hasTimeLimit;                   
    int turnLimit;
    bool hasNoHealing;                   
    bool hasRandomizedClasses;           
    bool hasCursedItems;                 
    
    
    float goldRewardMultiplier;          
    float xpRewardMultiplier;            
    bool unlocksBonusEnding;
    
    NewGamePlusModifiers() : difficultyMultiplier(1), retainLevel(false),
                            retainGold(false), retainItems(false),
                            retainSkills(false), hasPermadeath(false),
                            hasTimeLimit(false), turnLimit(0),
                            hasNoHealing(false), hasRandomizedClasses(false),
                            hasCursedItems(false), goldRewardMultiplier(1.0f),
                            xpRewardMultiplier(1.0f), unlocksBonusEnding(false) {}
};





enum class ChallengeMode {
    NONE,
    IRONMAN,             
    SPEEDRUN,            
    PACIFIST,            
    POVERTY,             
    GLASS_CANNON,        
    SOLO,                
    COLLECTOR,           
    MINIMALIST           
};

struct ChallengeRun {
    ChallengeMode mode;
    std::string name;
    std::string description;
    bool isActive;
    bool isCompleted;
    
    
    std::map<std::string, bool> rules;
    
    
    int achievementPoints;
    std::string unlockedTitle;
    int cosmeticRewardId;
    
    ChallengeRun() : mode(ChallengeMode::NONE), isActive(false),
                    isCompleted(false), achievementPoints(0),
                    cosmeticRewardId(-1) {}
};





struct PlayerStatistics {
    
    int totalKills;
    int totalDamageDealt;
    int totalDamageTaken;
    int totalHealing;
    int criticalHits;
    int dodgedAttacks;
    int perfectDodges;                   
    int abilitiesUsed;
    int combosCompleted;
    
    
    int roomsVisited;
    int secretsFound;
    int treasuresOpened;
    int trapsTriggered;
    int puzzlesSolved;
    int merchantsVisited;
    
    
    int questsCompleted;
    int achievementsUnlocked;
    int maxLevelReached;
    int totalGoldEarned;
    int totalGoldSpent;
    int itemsCrafted;
    int itemsEnchanted;
    
    
    int totalRuns;
    int completedRuns;
    int fastestRunTurns;
    int longestSurvivalTurns;
    int currentStreak;                   
    int bestStreak;
    
    
    std::vector<std::string> uniqueDeaths;  
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





class AchievementManager {
public:
    AchievementManager();
    
    
    void checkAchievements(const Player& player, const GameState& state);
    void updateProgress(int achievementId, int progress);
    void unlockAchievement(int achievementId);
    bool isUnlocked(int achievementId) const;
    
    
    void displayAchievements() const;
    void displayAchievementsByCategory(AchievementCategory category) const;
    void displayRecentUnlocks() const;
    float getCompletionPercentage() const;
    
    
    void claimAchievementReward(int achievementId, Player& player);
    int getTotalAchievementPoints() const;
    
    
    void displayPerks() const;
    bool unlockPerk(const std::string& perkId);
    bool hasPerk(const std::string& perkId) const;
    void applyPerksToNewRun(Player& player);
    
    
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





class StatisticsManager {
public:
    StatisticsManager();
    
    
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
    
    
    void displayStatistics() const;
    void displayCombatStats() const;
    void displayExplorationStats() const;
    void displayProgressStats() const;
    
    
    void updateLeaderboard(const std::string& playerName, int score);
    void displayLeaderboard() const;
    
    
    std::string serializeStats() const;
    void deserializeStats(const std::string& data);
    
    const PlayerStatistics& getStats() const { return stats; }
    
private:
    PlayerStatistics stats;
    std::vector<std::pair<std::string, int>> leaderboard;  
    
    int calculateScore(const Player& player, int turns, bool won) const;
};

}  

#endif 
