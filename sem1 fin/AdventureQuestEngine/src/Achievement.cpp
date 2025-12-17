/*******************************************************************************
 * Achievement.cpp - Full implementation for Achievement System
 ******************************************************************************/

#include "../include/Achievement.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace AQE {

//==============================================================================
// AchievementManager Implementation
//==============================================================================

AchievementManager::AchievementManager() : totalPoints(0), spentPoints(0) {
    initializeAchievements();
    initializePerks();
}

void AchievementManager::checkAchievements(const Player& player, const GameState& state) {
    (void)state;  // May use for additional checks
    
    // Check kill achievements
    if (player.totalKills >= 1) {
        updateProgress(1001, player.totalKills);
    }
    if (player.totalKills >= 100) {
        updateProgress(1002, player.totalKills);
    }
    if (player.totalKills >= 1000) {
        updateProgress(1003, player.totalKills);
    }
    
    // Check exploration achievements
    updateProgress(2001, static_cast<int>(player.visitedRooms.size()));
    updateProgress(2002, player.secretsFound);
    
    // Check level achievements
    updateProgress(3001, player.level);
    
    // Check gold achievements
    updateProgress(4001, player.gold);
    
    // Check quest achievements
    updateProgress(5001, static_cast<int>(player.completedQuests.size()));
}

void AchievementManager::updateProgress(int achievementId, int progress) {
    Achievement* ach = findAchievement(achievementId);
    if (ach == nullptr || ach->isUnlocked) return;
    
    ach->currentProgress = progress;
    
    if (ach->currentProgress >= ach->requiredProgress) {
        unlockAchievement(achievementId);
    }
}

void AchievementManager::unlockAchievement(int achievementId) {
    Achievement* ach = findAchievement(achievementId);
    if (ach == nullptr || ach->isUnlocked) return;
    
    ach->isUnlocked = true;
    ach->currentProgress = ach->requiredProgress;
    totalPoints += ach->rewardPoints;
    
    std::cout << "\n*** ACHIEVEMENT UNLOCKED: " << ach->name << " ***\n";
    std::cout << "    " << ach->description << "\n";
    std::cout << "    +" << ach->rewardPoints << " Achievement Points!\n\n";
    
    recentUnlocks.push_back(*ach);
    if (recentUnlocks.size() > 5) {
        recentUnlocks.erase(recentUnlocks.begin());
    }
}

bool AchievementManager::isUnlocked(int achievementId) const {
    for (const auto& ach : achievements) {
        if (ach.id == achievementId) {
            return ach.isUnlocked;
        }
    }
    return false;
}

void AchievementManager::displayAchievements() const {
    std::cout << "\n========================================\n";
    std::cout << "         ACHIEVEMENTS\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Total Points: " << totalPoints << "\n";
    std::cout << "Completion: " << std::fixed << std::setprecision(1) 
              << getCompletionPercentage() << "%\n\n";
    
    int unlockedCount = 0;
    for (const auto& ach : achievements) {
        if (ach.isUnlocked) unlockedCount++;
    }
    std::cout << "Unlocked: " << unlockedCount << "/" << achievements.size() << "\n\n";
    
    for (const auto& ach : achievements) {
        if (ach.isHidden && !ach.isUnlocked) {
            std::cout << "[?] Hidden Achievement\n";
            continue;
        }
        
        std::cout << "[" << (ach.isUnlocked ? "X" : " ") << "] " << ach.name;
        
        // Show tier
        std::cout << " (";
        switch (ach.tier) {
            case AchievementTier::BRONZE: std::cout << "Bronze"; break;
            case AchievementTier::SILVER: std::cout << "Silver"; break;
            case AchievementTier::GOLD: std::cout << "Gold"; break;
            case AchievementTier::PLATINUM: std::cout << "Platinum"; break;
            case AchievementTier::DIAMOND: std::cout << "Diamond"; break;
        }
        std::cout << " - " << ach.rewardPoints << " pts)\n";
        
        std::cout << "    " << ach.description << "\n";
        
        if (!ach.isUnlocked) {
            std::cout << "    Progress: " << ach.currentProgress << "/" << ach.requiredProgress;
            if (ach.isPercentBased) {
                float pct = (static_cast<float>(ach.currentProgress) / ach.requiredProgress) * 100;
                std::cout << " (" << std::fixed << std::setprecision(0) << pct << "%)";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

void AchievementManager::displayAchievementsByCategory(AchievementCategory category) const {
    std::cout << "\n========================================\n";
    std::cout << "    ACHIEVEMENTS - ";
    switch (category) {
        case AchievementCategory::COMBAT: std::cout << "COMBAT"; break;
        case AchievementCategory::EXPLORATION: std::cout << "EXPLORATION"; break;
        case AchievementCategory::COLLECTION: std::cout << "COLLECTION"; break;
        case AchievementCategory::MASTERY: std::cout << "MASTERY"; break;
        case AchievementCategory::CHALLENGE: std::cout << "CHALLENGE"; break;
        case AchievementCategory::STORY: std::cout << "STORY"; break;
        case AchievementCategory::HIDDEN: std::cout << "HIDDEN"; break;
    }
    std::cout << "\n========================================\n\n";
    
    for (const auto& ach : achievements) {
        if (ach.category != category) continue;
        
        std::cout << "[" << (ach.isUnlocked ? "X" : " ") << "] " << ach.name << "\n";
        std::cout << "    " << ach.description << "\n\n";
    }
}

void AchievementManager::displayRecentUnlocks() const {
    if (recentUnlocks.empty()) {
        std::cout << "No recent achievements.\n";
        return;
    }
    
    std::cout << "\n--- Recent Achievements ---\n";
    for (const auto& ach : recentUnlocks) {
        std::cout << "* " << ach.name << " (" << ach.rewardPoints << " pts)\n";
    }
}

float AchievementManager::getCompletionPercentage() const {
    if (achievements.empty()) return 0.0f;
    
    int unlocked = 0;
    for (const auto& ach : achievements) {
        if (ach.isUnlocked) unlocked++;
    }
    
    return (static_cast<float>(unlocked) / achievements.size()) * 100.0f;
}

void AchievementManager::claimAchievementReward(int achievementId, Player& player) {
    Achievement* ach = findAchievement(achievementId);
    if (ach == nullptr || !ach->isUnlocked) {
        std::cout << "Achievement not unlocked!\n";
        return;
    }
    
    // Give rewards
    if (ach->goldReward > 0) {
        player.gold += ach->goldReward;
        std::cout << "Received " << ach->goldReward << " gold!\n";
    }
    
    if (ach->experienceReward > 0) {
        player.experience += ach->experienceReward;
        std::cout << "Received " << ach->experienceReward << " XP!\n";
    }
    
    for (int itemId : ach->itemRewards) {
        Item reward;
        reward.id = itemId;
        reward.name = "Achievement Reward";
        reward.value = 100;
        player.inventory.push_back(reward);
        std::cout << "Received item!\n";
    }
    
    if (!ach->unlockedPerkId.empty()) {
        unlockPerk(ach->unlockedPerkId);
    }
}

int AchievementManager::getTotalAchievementPoints() const {
    return totalPoints;
}

void AchievementManager::displayPerks() const {
    std::cout << "\n========================================\n";
    std::cout << "       PERMANENT PERKS\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Available Points: " << (totalPoints - spentPoints) << "\n\n";
    
    for (const auto& perk : perks) {
        std::cout << "[" << (perk.isUnlocked ? "X" : " ") << "] " << perk.name;
        std::cout << " (" << perk.achievementPointCost << " pts)\n";
        std::cout << "    " << perk.description << "\n\n";
    }
}

bool AchievementManager::unlockPerk(const std::string& perkId) {
    PermanentPerk* perk = findPerk(perkId);
    if (perk == nullptr) {
        std::cout << "Perk not found!\n";
        return false;
    }
    
    if (perk->isUnlocked) {
        std::cout << "Perk already unlocked!\n";
        return false;
    }
    
    int availablePoints = totalPoints - spentPoints;
    if (availablePoints < perk->achievementPointCost) {
        std::cout << "Not enough achievement points! Need " << perk->achievementPointCost << "\n";
        return false;
    }
    
    spentPoints += perk->achievementPointCost;
    perk->isUnlocked = true;
    
    std::cout << "Perk unlocked: " << perk->name << "!\n";
    return true;
}

bool AchievementManager::hasPerk(const std::string& perkId) const {
    for (const auto& perk : perks) {
        if (perk.id == perkId) {
            return perk.isUnlocked;
        }
    }
    return false;
}

void AchievementManager::applyPerksToNewRun(Player& player) {
    for (const auto& perk : perks) {
        if (!perk.isUnlocked) continue;
        
        // Apply starting gold bonus
        if (perk.startingGoldMultiplier > 1.0f) {
            player.gold = static_cast<int>(player.gold * perk.startingGoldMultiplier);
        }
        
        // Apply starting level bonus
        if (perk.startingLevel > 1) {
            player.level = perk.startingLevel;
        }
        
        // Apply crit bonus
        if (perk.increasedCritChance) {
            player.stats.critChance += perk.critChanceBonus;
        }
    }
    
    std::cout << "Permanent perks applied to new run!\n";
}

std::string AchievementManager::serializeAchievements() const {
    std::stringstream ss;
    
    ss << totalPoints << ";" << spentPoints << "|";
    
    for (const auto& ach : achievements) {
        if (ach.isUnlocked) {
            ss << ach.id << ",";
        }
    }
    ss << "|";
    
    for (const auto& perk : perks) {
        if (perk.isUnlocked) {
            ss << perk.id << ",";
        }
    }
    
    return ss.str();
}

void AchievementManager::deserializeAchievements(const std::string& data) {
    if (data.empty()) return;
    
    std::stringstream ss(data);
    std::string segment;
    
    // First segment: points
    if (std::getline(ss, segment, '|')) {
        size_t semicolon = segment.find(';');
        if (semicolon != std::string::npos) {
            totalPoints = std::stoi(segment.substr(0, semicolon));
            spentPoints = std::stoi(segment.substr(semicolon + 1));
        }
    }
    
    // Second segment: unlocked achievements
    if (std::getline(ss, segment, '|')) {
        std::stringstream achSs(segment);
        std::string achId;
        while (std::getline(achSs, achId, ',')) {
            if (achId.empty()) continue;
            int id = std::stoi(achId);
            Achievement* ach = findAchievement(id);
            if (ach) {
                ach->isUnlocked = true;
                ach->currentProgress = ach->requiredProgress;
            }
        }
    }
    
    // Third segment: unlocked perks
    if (std::getline(ss, segment, '|')) {
        std::stringstream perkSs(segment);
        std::string perkId;
        while (std::getline(perkSs, perkId, ',')) {
            if (perkId.empty()) continue;
            PermanentPerk* perk = findPerk(perkId);
            if (perk) {
                perk->isUnlocked = true;
            }
        }
    }
}

void AchievementManager::initializeAchievements() {
    // Combat Achievements
    Achievement firstBlood;
    firstBlood.id = 1001;
    firstBlood.name = "First Blood";
    firstBlood.description = "Kill your first enemy";
    firstBlood.category = AchievementCategory::COMBAT;
    firstBlood.tier = AchievementTier::BRONZE;
    firstBlood.rewardPoints = 10;
    firstBlood.requiredProgress = 1;
    firstBlood.goldReward = 25;
    achievements.push_back(firstBlood);
    
    Achievement centurion;
    centurion.id = 1002;
    centurion.name = "Centurion";
    centurion.description = "Kill 100 enemies";
    centurion.category = AchievementCategory::COMBAT;
    centurion.tier = AchievementTier::SILVER;
    centurion.rewardPoints = 25;
    centurion.requiredProgress = 100;
    centurion.goldReward = 100;
    achievements.push_back(centurion);
    
    Achievement legendaryWarrior;
    legendaryWarrior.id = 1003;
    legendaryWarrior.name = "Legendary Warrior";
    legendaryWarrior.description = "Kill 1000 enemies";
    legendaryWarrior.category = AchievementCategory::COMBAT;
    legendaryWarrior.tier = AchievementTier::DIAMOND;
    legendaryWarrior.rewardPoints = 100;
    legendaryWarrior.requiredProgress = 1000;
    legendaryWarrior.goldReward = 500;
    legendaryWarrior.unlockedPerkId = "warrior_bonus";
    achievements.push_back(legendaryWarrior);
    
    // Exploration Achievements
    Achievement curiousExplorer;
    curiousExplorer.id = 2001;
    curiousExplorer.name = "Curious Explorer";
    curiousExplorer.description = "Visit 50 rooms";
    curiousExplorer.category = AchievementCategory::EXPLORATION;
    curiousExplorer.tier = AchievementTier::BRONZE;
    curiousExplorer.rewardPoints = 10;
    curiousExplorer.requiredProgress = 50;
    curiousExplorer.goldReward = 50;
    achievements.push_back(curiousExplorer);
    
    Achievement secretSeeker;
    secretSeeker.id = 2002;
    secretSeeker.name = "Secret Seeker";
    secretSeeker.description = "Find 10 secret rooms";
    secretSeeker.category = AchievementCategory::EXPLORATION;
    secretSeeker.tier = AchievementTier::SILVER;
    secretSeeker.rewardPoints = 25;
    secretSeeker.requiredProgress = 10;
    secretSeeker.goldReward = 100;
    achievements.push_back(secretSeeker);
    
    // Mastery Achievements
    Achievement levelUp;
    levelUp.id = 3001;
    levelUp.name = "Rising Star";
    levelUp.description = "Reach level 10";
    levelUp.category = AchievementCategory::MASTERY;
    levelUp.tier = AchievementTier::BRONZE;
    levelUp.rewardPoints = 15;
    levelUp.requiredProgress = 10;
    levelUp.goldReward = 50;
    achievements.push_back(levelUp);
    
    // Collection Achievements
    Achievement wealthy;
    wealthy.id = 4001;
    wealthy.name = "Wealthy Adventurer";
    wealthy.description = "Accumulate 1000 gold";
    wealthy.category = AchievementCategory::COLLECTION;
    wealthy.tier = AchievementTier::SILVER;
    wealthy.rewardPoints = 20;
    wealthy.requiredProgress = 1000;
    wealthy.goldReward = 100;
    achievements.push_back(wealthy);
    
    // Quest Achievements
    Achievement questComplete;
    questComplete.id = 5001;
    questComplete.name = "Quest Master";
    questComplete.description = "Complete 10 quests";
    questComplete.category = AchievementCategory::STORY;
    questComplete.tier = AchievementTier::GOLD;
    questComplete.rewardPoints = 50;
    questComplete.requiredProgress = 10;
    questComplete.goldReward = 200;
    achievements.push_back(questComplete);
    
    // Hidden Achievements
    Achievement speedDemon;
    speedDemon.id = 9001;
    speedDemon.name = "Speed Demon";
    speedDemon.description = "Complete the game in under 100 turns";
    speedDemon.secretDescription = "Complete the game very quickly";
    speedDemon.category = AchievementCategory::HIDDEN;
    speedDemon.tier = AchievementTier::PLATINUM;
    speedDemon.rewardPoints = 75;
    speedDemon.requiredProgress = 1;
    speedDemon.isHidden = true;
    speedDemon.goldReward = 300;
    achievements.push_back(speedDemon);
    
    Achievement pacifist;
    pacifist.id = 9002;
    pacifist.name = "Pacifist";
    pacifist.description = "Complete the game without killing anyone";
    pacifist.secretDescription = "Find a peaceful way through";
    pacifist.category = AchievementCategory::HIDDEN;
    pacifist.tier = AchievementTier::PLATINUM;
    pacifist.rewardPoints = 75;
    pacifist.requiredProgress = 1;
    pacifist.isHidden = true;
    pacifist.goldReward = 300;
    achievements.push_back(pacifist);
}

void AchievementManager::initializePerks() {
    PermanentPerk wealthyStart;
    wealthyStart.id = "wealthy_start";
    wealthyStart.name = "Wealthy Start";
    wealthyStart.description = "Start each run with +100 gold";
    wealthyStart.achievementPointCost = 50;
    wealthyStart.startingGoldMultiplier = 2.0f;
    perks.push_back(wealthyStart);
    
    PermanentPerk quickLearner;
    quickLearner.id = "quick_learner";
    quickLearner.name = "Quick Learner";
    quickLearner.description = "+15% XP gain in all runs";
    quickLearner.achievementPointCost = 75;
    quickLearner.startingXPMultiplier = 1.15f;
    perks.push_back(quickLearner);
    
    PermanentPerk luckyCharm;
    luckyCharm.id = "lucky_charm";
    luckyCharm.name = "Lucky Charm";
    luckyCharm.description = "+10% drop rate for items";
    luckyCharm.achievementPointCost = 100;
    luckyCharm.increasedDropRate = true;
    luckyCharm.dropRateBonus = 0.1f;
    perks.push_back(luckyCharm);
    
    PermanentPerk vaultAccess;
    vaultAccess.id = "vault_access";
    vaultAccess.name = "Vault Access";
    vaultAccess.description = "Keep 10 items between runs";
    vaultAccess.achievementPointCost = 125;
    vaultAccess.unlocksStorage = true;
    vaultAccess.storageSize = 10;
    perks.push_back(vaultAccess);
    
    PermanentPerk fastTravel;
    fastTravel.id = "fast_travel";
    fastTravel.name = "Fast Travel";
    fastTravel.description = "Teleport to any previously visited room";
    fastTravel.achievementPointCost = 150;
    fastTravel.unlocksFastTravel = true;
    perks.push_back(fastTravel);
    
    PermanentPerk trueSight;
    trueSight.id = "true_sight";
    trueSight.name = "True Sight";
    trueSight.description = "See all secret rooms on the map";
    trueSight.achievementPointCost = 200;
    trueSight.grantsVision = true;
    perks.push_back(trueSight);
    
    PermanentPerk critBonus;
    critBonus.id = "crit_bonus";
    critBonus.name = "Critical Eye";
    critBonus.description = "+5% critical hit chance";
    critBonus.achievementPointCost = 100;
    critBonus.increasedCritChance = true;
    critBonus.critChanceBonus = 5;
    perks.push_back(critBonus);
    
    PermanentPerk warriorBonus;
    warriorBonus.id = "warrior_bonus";
    warriorBonus.name = "Veteran's Experience";
    warriorBonus.description = "Start at level 2 instead of level 1";
    warriorBonus.achievementPointCost = 75;
    warriorBonus.startingLevel = 2;
    perks.push_back(warriorBonus);
}

Achievement* AchievementManager::findAchievement(int achievementId) {
    for (auto& ach : achievements) {
        if (ach.id == achievementId) {
            return &ach;
        }
    }
    return nullptr;
}

PermanentPerk* AchievementManager::findPerk(const std::string& perkId) {
    for (auto& perk : perks) {
        if (perk.id == perkId) {
            return &perk;
        }
    }
    return nullptr;
}

//==============================================================================
// StatisticsManager Implementation
//==============================================================================

StatisticsManager::StatisticsManager() {
    // Stats are initialized to 0 by PlayerStatistics constructor
}

void StatisticsManager::recordKill() {
    stats.totalKills++;
}

void StatisticsManager::recordDamage(int amount, bool dealt) {
    if (dealt) {
        stats.totalDamageDealt += amount;
    } else {
        stats.totalDamageTaken += amount;
    }
}

void StatisticsManager::recordHealing(int amount) {
    stats.totalHealing += amount;
}

void StatisticsManager::recordCrit() {
    stats.criticalHits++;
}

void StatisticsManager::recordDodge() {
    stats.dodgedAttacks++;
}

void StatisticsManager::recordAbility() {
    stats.abilitiesUsed++;
}

void StatisticsManager::recordCombo() {
    stats.combosCompleted++;
}

void StatisticsManager::recordRoomVisit() {
    stats.roomsVisited++;
}

void StatisticsManager::recordSecret() {
    stats.secretsFound++;
}

void StatisticsManager::recordTreasure() {
    stats.treasuresOpened++;
}

void StatisticsManager::recordTrap() {
    stats.trapsTriggered++;
}

void StatisticsManager::recordPuzzle() {
    stats.puzzlesSolved++;
}

void StatisticsManager::recordMerchant() {
    stats.merchantsVisited++;
}

void StatisticsManager::recordQuest() {
    stats.questsCompleted++;
}

void StatisticsManager::recordCraft() {
    stats.itemsCrafted++;
}

void StatisticsManager::recordEnchant() {
    stats.itemsEnchanted++;
}

void StatisticsManager::recordGold(int amount, bool earned) {
    if (earned) {
        stats.totalGoldEarned += amount;
    } else {
        stats.totalGoldSpent += amount;
    }
}

void StatisticsManager::recordRunStart() {
    stats.totalRuns++;
}

void StatisticsManager::recordRunEnd(bool won, int turns) {
    if (won) {
        stats.completedRuns++;
        stats.currentStreak++;
        if (stats.currentStreak > stats.bestStreak) {
            stats.bestStreak = stats.currentStreak;
        }
        if (turns < stats.fastestRunTurns) {
            stats.fastestRunTurns = turns;
        }
    } else {
        stats.currentStreak = 0;
    }
    
    if (turns > stats.longestSurvivalTurns) {
        stats.longestSurvivalTurns = turns;
    }
}

void StatisticsManager::recordDeath(const std::string& cause) {
    if (std::find(stats.uniqueDeaths.begin(), stats.uniqueDeaths.end(), cause) == stats.uniqueDeaths.end()) {
        stats.uniqueDeaths.push_back(cause);
    }
}

void StatisticsManager::recordEnding(const std::string& ending) {
    if (std::find(stats.endingsUnlocked.begin(), stats.endingsUnlocked.end(), ending) == stats.endingsUnlocked.end()) {
        stats.endingsUnlocked.push_back(ending);
    }
}

void StatisticsManager::recordTitle(const std::string& title) {
    if (std::find(stats.titlesEarned.begin(), stats.titlesEarned.end(), title) == stats.titlesEarned.end()) {
        stats.titlesEarned.push_back(title);
    }
}

void StatisticsManager::displayStatistics() const {
    std::cout << "\n========================================\n";
    std::cout << "       PLAYER STATISTICS\n";
    std::cout << "========================================\n\n";
    
    displayCombatStats();
    displayExplorationStats();
    displayProgressStats();
}

void StatisticsManager::displayCombatStats() const {
    std::cout << "--- COMBAT ---\n";
    std::cout << "  Total Kills: " << stats.totalKills << "\n";
    std::cout << "  Damage Dealt: " << stats.totalDamageDealt << "\n";
    std::cout << "  Damage Taken: " << stats.totalDamageTaken << "\n";
    std::cout << "  Healing Done: " << stats.totalHealing << "\n";
    std::cout << "  Critical Hits: " << stats.criticalHits << "\n";
    std::cout << "  Attacks Dodged: " << stats.dodgedAttacks << "\n";
    std::cout << "  Abilities Used: " << stats.abilitiesUsed << "\n";
    std::cout << "  Combos Completed: " << stats.combosCompleted << "\n\n";
}

void StatisticsManager::displayExplorationStats() const {
    std::cout << "--- EXPLORATION ---\n";
    std::cout << "  Rooms Visited: " << stats.roomsVisited << "\n";
    std::cout << "  Secrets Found: " << stats.secretsFound << "\n";
    std::cout << "  Treasures Opened: " << stats.treasuresOpened << "\n";
    std::cout << "  Traps Triggered: " << stats.trapsTriggered << "\n";
    std::cout << "  Puzzles Solved: " << stats.puzzlesSolved << "\n";
    std::cout << "  Merchants Visited: " << stats.merchantsVisited << "\n\n";
}

void StatisticsManager::displayProgressStats() const {
    std::cout << "--- PROGRESSION ---\n";
    std::cout << "  Total Runs: " << stats.totalRuns << "\n";
    std::cout << "  Completed Runs: " << stats.completedRuns << "\n";
    std::cout << "  Win Rate: " << (stats.totalRuns > 0 ? 
        (static_cast<float>(stats.completedRuns) / stats.totalRuns * 100) : 0) << "%\n";
    std::cout << "  Fastest Run: " << stats.fastestRunTurns << " turns\n";
    std::cout << "  Longest Survival: " << stats.longestSurvivalTurns << " turns\n";
    std::cout << "  Best Win Streak: " << stats.bestStreak << "\n";
    std::cout << "  Quests Completed: " << stats.questsCompleted << "\n";
    std::cout << "  Max Level Reached: " << stats.maxLevelReached << "\n";
    std::cout << "  Total Gold Earned: " << stats.totalGoldEarned << "\n";
    std::cout << "  Items Crafted: " << stats.itemsCrafted << "\n";
    std::cout << "  Items Enchanted: " << stats.itemsEnchanted << "\n\n";
    
    std::cout << "  Unique Endings: " << stats.endingsUnlocked.size() << "\n";
    std::cout << "  Titles Earned: " << stats.titlesEarned.size() << "\n";
}

void StatisticsManager::updateLeaderboard(const std::string& playerName, int score) {
    leaderboard.push_back({playerName, score});
    
    // Sort by score descending
    std::sort(leaderboard.begin(), leaderboard.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Keep only top 10
    if (leaderboard.size() > 10) {
        leaderboard.resize(10);
    }
}

void StatisticsManager::displayLeaderboard() const {
    std::cout << "\n========================================\n";
    std::cout << "         LEADERBOARD\n";
    std::cout << "========================================\n\n";
    
    if (leaderboard.empty()) {
        std::cout << "No entries yet!\n";
        return;
    }
    
    int rank = 1;
    for (const auto& entry : leaderboard) {
        std::cout << std::setw(2) << rank << ". " << std::setw(20) << std::left 
                  << entry.first << " - " << entry.second << " pts\n";
        rank++;
    }
}

std::string StatisticsManager::serializeStats() const {
    std::stringstream ss;
    ss << stats.totalKills << "," << stats.totalDamageDealt << "," << stats.totalDamageTaken << ",";
    ss << stats.totalHealing << "," << stats.criticalHits << "," << stats.dodgedAttacks << ",";
    ss << stats.abilitiesUsed << "," << stats.combosCompleted << "," << stats.roomsVisited << ",";
    ss << stats.secretsFound << "," << stats.treasuresOpened << "," << stats.trapsTriggered << ",";
    ss << stats.puzzlesSolved << "," << stats.merchantsVisited << "," << stats.questsCompleted << ",";
    ss << stats.maxLevelReached << "," << stats.totalGoldEarned << "," << stats.totalGoldSpent << ",";
    ss << stats.itemsCrafted << "," << stats.itemsEnchanted << "," << stats.totalRuns << ",";
    ss << stats.completedRuns << "," << stats.fastestRunTurns << "," << stats.longestSurvivalTurns << ",";
    ss << stats.currentStreak << "," << stats.bestStreak;
    return ss.str();
}

void StatisticsManager::deserializeStats(const std::string& data) {
    if (data.empty()) return;
    
    std::stringstream ss(data);
    std::string token;
    std::vector<int> values;
    
    while (std::getline(ss, token, ',')) {
        if (!token.empty()) {
            values.push_back(std::stoi(token));
        }
    }
    
    if (values.size() >= 26) {
        stats.totalKills = values[0];
        stats.totalDamageDealt = values[1];
        stats.totalDamageTaken = values[2];
        stats.totalHealing = values[3];
        stats.criticalHits = values[4];
        stats.dodgedAttacks = values[5];
        stats.abilitiesUsed = values[6];
        stats.combosCompleted = values[7];
        stats.roomsVisited = values[8];
        stats.secretsFound = values[9];
        stats.treasuresOpened = values[10];
        stats.trapsTriggered = values[11];
        stats.puzzlesSolved = values[12];
        stats.merchantsVisited = values[13];
        stats.questsCompleted = values[14];
        stats.maxLevelReached = values[15];
        stats.totalGoldEarned = values[16];
        stats.totalGoldSpent = values[17];
        stats.itemsCrafted = values[18];
        stats.itemsEnchanted = values[19];
        stats.totalRuns = values[20];
        stats.completedRuns = values[21];
        stats.fastestRunTurns = values[22];
        stats.longestSurvivalTurns = values[23];
        stats.currentStreak = values[24];
        stats.bestStreak = values[25];
    }
}

int StatisticsManager::calculateScore(const Player& player, int turns, bool won) const {
    int score = 0;
    
    // Base score from level
    score += player.level * 100;
    
    // Score from gold
    score += player.gold;
    
    // Score from kills
    score += stats.totalKills * 10;
    
    // Score from exploration
    score += stats.roomsVisited * 5;
    score += stats.secretsFound * 50;
    score += stats.puzzlesSolved * 25;
    
    // Time bonus (faster = more points)
    if (turns < 200 && won) {
        score += (200 - turns) * 5;
    }
    
    // Victory bonus
    if (won) {
        score += 500;
    }
    
    return score;
}

}  // namespace AQE
