/*******************************************************************************
 * Companion.cpp - Full implementation for Companion System
 ******************************************************************************/

#include "../include/Companion.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cstdlib>

namespace AQE {

//==============================================================================
// CompanionManager Implementation
//==============================================================================

CompanionManager::CompanionManager(Player& player) 
    : playerRef(player), activeCompanionId(-1), maxActiveCompanions(1) {
    initializeCompanions();
}

void CompanionManager::displayAvailableCompanions() const {
    std::cout << "\n========================================\n";
    std::cout << "      AVAILABLE COMPANIONS\n";
    std::cout << "========================================\n\n";
    
    for (const auto& comp : allCompanions) {
        bool recruited = std::find(recruitedCompanionIds.begin(), recruitedCompanionIds.end(), 
                                   comp.id) != recruitedCompanionIds.end();
        
        std::cout << "[" << comp.id << "] " << comp.name;
        
        if (recruited) {
            std::cout << " (RECRUITED)";
            if (comp.id == activeCompanionId) {
                std::cout << " [ACTIVE]";
            }
        }
        
        std::cout << "\n";
        std::cout << "  Class: ";
        switch (comp.companionClass) {
            case CompanionClass::ARCHER: std::cout << "Archer"; break;
            case CompanionClass::HEALER: std::cout << "Healer"; break;
            case CompanionClass::TANK: std::cout << "Tank"; break;
            case CompanionClass::MAGE_SUPPORT: std::cout << "Mage"; break;
            case CompanionClass::THIEF: std::cout << "Thief"; break;
            case CompanionClass::BEAST: std::cout << "Beast"; break;
        }
        std::cout << " | Level: " << comp.experienceLevel;
        
        if (recruited) {
            std::cout << " | Loyalty: " << comp.loyaltyPoints << "/100";
        }
        std::cout << "\n\n";
    }
}

bool CompanionManager::recruitCompanion(int companionId) {
    if (std::find(recruitedCompanionIds.begin(), recruitedCompanionIds.end(), 
                  companionId) != recruitedCompanionIds.end()) {
        std::cout << "Companion already recruited!\n";
        return false;
    }
    
    Companion* comp = findCompanion(companionId);
    if (comp == nullptr) {
        std::cout << "Companion not found!\n";
        return false;
    }
    
    recruitedCompanionIds.push_back(companionId);
    std::cout << comp->name << " has joined your party!\n";
    
    // Auto-set as active if no active companion
    if (activeCompanionId == -1) {
        setActiveCompanion(companionId);
    }
    
    return true;
}

bool CompanionManager::dismissCompanion(int companionId) {
    auto it = std::find(recruitedCompanionIds.begin(), recruitedCompanionIds.end(), companionId);
    if (it == recruitedCompanionIds.end()) {
        std::cout << "Companion not in your party!\n";
        return false;
    }
    
    Companion* comp = findCompanion(companionId);
    if (comp && comp->cantDie) {
        std::cout << comp->name << " refuses to leave your side!\n";
        return false;
    }
    
    recruitedCompanionIds.erase(it);
    
    if (activeCompanionId == companionId) {
        activeCompanionId = -1;
        if (!recruitedCompanionIds.empty()) {
            setActiveCompanion(recruitedCompanionIds[0]);
        }
    }
    
    std::cout << "Companion dismissed from party.\n";
    return true;
}

int CompanionManager::getMaxCompanions() const {
    // Base + bonus from level
    return maxActiveCompanions + (playerRef.level / 10);
}

void CompanionManager::setActiveCompanion(int companionId) {
    if (std::find(recruitedCompanionIds.begin(), recruitedCompanionIds.end(), 
                  companionId) == recruitedCompanionIds.end()) {
        std::cout << "Must recruit companion first!\n";
        return;
    }
    
    activeCompanionId = companionId;
    
    Companion* comp = findCompanion(companionId);
    if (comp) {
        comp->isActiveInCombat = true;
        std::cout << comp->name << " is now your active companion.\n";
    }
}

Companion* CompanionManager::getActiveCompanion() {
    if (activeCompanionId == -1) {
        return nullptr;
    }
    return findCompanion(activeCompanionId);
}

void CompanionManager::displayCompanionInfo(int companionId) const {
    const Companion* comp = nullptr;
    for (const auto& c : allCompanions) {
        if (c.id == companionId) {
            comp = &c;
            break;
        }
    }
    
    if (comp == nullptr) {
        std::cout << "Companion not found!\n";
        return;
    }
    
    std::cout << "\n========================================\n";
    std::cout << "      " << comp->name << "\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Class: ";
    switch (comp->companionClass) {
        case CompanionClass::ARCHER: std::cout << "Archer"; break;
        case CompanionClass::HEALER: std::cout << "Healer"; break;
        case CompanionClass::TANK: std::cout << "Tank"; break;
        case CompanionClass::MAGE_SUPPORT: std::cout << "Mage Support"; break;
        case CompanionClass::THIEF: std::cout << "Thief"; break;
        case CompanionClass::BEAST: std::cout << "Beast"; break;
    }
    std::cout << "\n";
    
    std::cout << "Level: " << comp->experienceLevel << "\n";
    std::cout << "HP: " << comp->stats.currentHP << "/" << comp->stats.maxHP << "\n";
    std::cout << "Loyalty: " << comp->loyaltyPoints << "/100\n";
    std::cout << "Mood: ";
    switch (comp->mood) {
        case CompanionMood::ANGRY: std::cout << "Angry"; break;
        case CompanionMood::UNHAPPY: std::cout << "Unhappy"; break;
        case CompanionMood::NEUTRAL: std::cout << "Neutral"; break;
        case CompanionMood::HAPPY: std::cout << "Happy"; break;
        case CompanionMood::DEVOTED: std::cout << "Devoted"; break;
    }
    std::cout << "\n\n";
    
    std::cout << "Backstory:\n" << comp->backstory << "\n\n";
    
    std::cout << "Abilities:\n";
    for (const auto& ability : comp->abilities) {
        std::cout << "  - " << ability.name;
        if (ability.isPassive) {
            std::cout << " [PASSIVE]";
        }
        std::cout << "\n    " << ability.description << "\n";
    }
    
    std::cout << "\nLikes: ";
    for (size_t i = 0; i < comp->likes.size(); ++i) {
        std::cout << comp->likes[i];
        if (i < comp->likes.size() - 1) std::cout << ", ";
    }
    
    std::cout << "\nDislikes: ";
    for (size_t i = 0; i < comp->dislikes.size(); ++i) {
        std::cout << comp->dislikes[i];
        if (i < comp->dislikes.size() - 1) std::cout << ", ";
    }
    std::cout << "\n";
}

void CompanionManager::manageCompanions() {
    std::cout << "\n========================================\n";
    std::cout << "      COMPANION MANAGEMENT\n";
    std::cout << "========================================\n\n";
    
    std::cout << "1. View All Companions\n";
    std::cout << "2. Set Active Companion\n";
    std::cout << "3. Talk to Companion\n";
    std::cout << "4. Give Gift\n";
    std::cout << "5. Dismiss Companion\n";
    std::cout << "6. Back\n";
    
    // This would be called from a menu loop in the game engine
}

void CompanionManager::changeLoyalty(int companionId, int amount, const std::string& reason) {
    Companion* comp = findCompanion(companionId);
    if (comp == nullptr) return;
    
    comp->loyaltyPoints += amount;
    
    // Clamp to 0-100
    if (comp->loyaltyPoints < 0) comp->loyaltyPoints = 0;
    if (comp->loyaltyPoints > 100) comp->loyaltyPoints = 100;
    
    if (amount > 0) {
        std::cout << comp->name << " is pleased! (+" << amount << " loyalty)\n";
    } else {
        std::cout << comp->name << " is displeased! (" << amount << " loyalty)\n";
    }
    std::cout << "Reason: " << reason << "\n";
    
    updateMood(companionId);
    triggerLoyaltyDialogue(companionId, amount > 0);
}

void CompanionManager::updateMood(int companionId) {
    Companion* comp = findCompanion(companionId);
    if (comp == nullptr) return;
    
    if (comp->loyaltyPoints <= 20) {
        comp->mood = CompanionMood::ANGRY;
    } else if (comp->loyaltyPoints <= 40) {
        comp->mood = CompanionMood::UNHAPPY;
    } else if (comp->loyaltyPoints <= 60) {
        comp->mood = CompanionMood::NEUTRAL;
    } else if (comp->loyaltyPoints <= 80) {
        comp->mood = CompanionMood::HAPPY;
    } else {
        comp->mood = CompanionMood::DEVOTED;
    }
}

void CompanionManager::checkLoyaltyEvents() {
    for (int id : recruitedCompanionIds) {
        Companion* comp = findCompanion(id);
        if (comp == nullptr) continue;
        
        // Check if companion might leave
        if (comp->mood == CompanionMood::ANGRY && !comp->cantDie) {
            int leaveChance = rand() % 100;
            if (leaveChance < 10) {  // 10% chance to leave when angry
                std::cout << comp->name << " has left your party due to low loyalty!\n";
                dismissCompanion(id);
            }
        }
    }
}

void CompanionManager::companionTakeTurn(Enemy& enemy, std::vector<std::string>& combatLog) {
    Companion* comp = getActiveCompanion();
    if (comp == nullptr || !comp->isActiveInCombat) return;
    
    // Get effectiveness modifier based on mood
    float effectiveMod = 1.0f;
    switch (comp->mood) {
        case CompanionMood::ANGRY: effectiveMod = 0.8f; break;
        case CompanionMood::UNHAPPY: effectiveMod = 0.9f; break;
        case CompanionMood::NEUTRAL: effectiveMod = 1.0f; break;
        case CompanionMood::HAPPY: effectiveMod = 1.1f; break;
        case CompanionMood::DEVOTED: effectiveMod = 1.25f; break;
    }
    
    // Find an ability to use
    for (auto& ability : comp->abilities) {
        if (ability.isPassive) continue;
        
        if (ability.currentCooldown > 0) {
            ability.currentCooldown--;
            continue;
        }
        
        // Use ability
        if (ability.damage > 0) {
            int damage = static_cast<int>(ability.damage * effectiveMod);
            enemy.stats.currentHP -= damage;
            
            std::stringstream ss;
            ss << comp->name << " uses " << ability.name << " for " << damage << " damage!";
            combatLog.push_back(ss.str());
        }
        
        if (ability.healing > 0) {
            int healing = static_cast<int>(ability.healing * effectiveMod);
            playerRef.stats.currentHP += healing;
            if (playerRef.stats.currentHP > playerRef.stats.maxHP) {
                playerRef.stats.currentHP = playerRef.stats.maxHP;
            }
            
            std::stringstream ss;
            ss << comp->name << " heals you for " << healing << " HP!";
            combatLog.push_back(ss.str());
        }
        
        ability.currentCooldown = ability.cooldown;
        break;
    }
}

void CompanionManager::healCompanion(int companionId, int amount) {
    Companion* comp = findCompanion(companionId);
    if (comp == nullptr) return;
    
    comp->stats.currentHP += amount;
    if (comp->stats.currentHP > comp->stats.maxHP) {
        comp->stats.currentHP = comp->stats.maxHP;
    }
    
    std::cout << comp->name << " healed for " << amount << " HP.\n";
}

void CompanionManager::companionLevelUp(int companionId) {
    Companion* comp = findCompanion(companionId);
    if (comp == nullptr) return;
    
    comp->experienceLevel++;
    comp->stats.maxHP += 10;
    comp->stats.currentHP = comp->stats.maxHP;
    comp->stats.attack += 2;
    comp->stats.defense += 1;
    
    std::cout << comp->name << " leveled up to " << comp->experienceLevel << "!\n";
}

void CompanionManager::talkToCompanion(int companionId) {
    Companion* comp = findCompanion(companionId);
    if (comp == nullptr) {
        std::cout << "Companion not found!\n";
        return;
    }
    
    std::cout << "\n" << comp->name << " says:\n";
    
    if (!comp->dialogueOptions.empty()) {
        int idx = rand() % comp->dialogueOptions.size();
        std::cout << "\"" << comp->dialogueOptions[idx] << "\"\n";
    } else {
        switch (comp->mood) {
            case CompanionMood::ANGRY:
                std::cout << "\"I don't want to talk right now.\"\n";
                break;
            case CompanionMood::UNHAPPY:
                std::cout << "\"What do you need?\"\n";
                break;
            case CompanionMood::NEUTRAL:
                std::cout << "\"How can I help?\"\n";
                break;
            case CompanionMood::HAPPY:
                std::cout << "\"Great to see you! What's on your mind?\"\n";
                break;
            case CompanionMood::DEVOTED:
                std::cout << "\"I'll follow you anywhere, my friend!\"\n";
                break;
        }
    }
    
    // Talking increases loyalty slightly
    changeLoyalty(companionId, 1, "conversation");
}

void CompanionManager::giveGift(int companionId, int itemId) {
    Companion* comp = findCompanion(companionId);
    if (comp == nullptr) {
        std::cout << "Companion not found!\n";
        return;
    }
    
    // Find and remove item from player inventory
    for (auto it = playerRef.inventory.begin(); it != playerRef.inventory.end(); ++it) {
        if (it->id == itemId) {
            std::cout << "You give " << it->name << " to " << comp->name << ".\n";
            
            // Base loyalty gain from item value
            int loyaltyGain = it->value / 20;
            if (loyaltyGain < 1) loyaltyGain = 1;
            if (loyaltyGain > 20) loyaltyGain = 20;
            
            changeLoyalty(companionId, loyaltyGain, "received gift");
            
            playerRef.inventory.erase(it);
            return;
        }
    }
    
    std::cout << "You don't have that item!\n";
}

void CompanionManager::unlockPersonalQuest(int companionId) {
    Companion* comp = findCompanion(companionId);
    if (comp == nullptr) return;
    
    if (comp->conversationLevel < 3) {
        std::cout << "Need higher conversation level to unlock personal quest.\n";
        return;
    }
    
    if (!comp->personalQuestIds.empty()) {
        std::cout << comp->name << "'s personal quest is now available!\n";
    }
}

float CompanionManager::getTotalXPBonus() const {
    float bonus = 0.0f;
    
    Companion* active = const_cast<CompanionManager*>(this)->getActiveCompanion();
    if (active && active->grantsXPBonus) {
        bonus += active->xpBonusPercent;
    }
    
    return bonus;
}

float CompanionManager::getTotalGoldBonus() const {
    float bonus = 0.0f;
    
    Companion* active = const_cast<CompanionManager*>(this)->getActiveCompanion();
    if (active && active->grantsGoldBonus) {
        bonus += active->goldBonusPercent;
    }
    
    return bonus;
}

bool CompanionManager::hasActiveLootBonus() const {
    Companion* active = const_cast<CompanionManager*>(this)->getActiveCompanion();
    return active != nullptr && active->grantsLootBonus;
}

void CompanionManager::initializeCompanions() {
    // Create Archer Companion - Lyra Swiftarrow
    Companion lyra;
    lyra.id = 1;
    lyra.name = "Lyra Swiftarrow";
    lyra.backstory = "A skilled ranger from the Whispering Woods, seeking vengeance against those who destroyed her village.";
    lyra.companionClass = CompanionClass::ARCHER;
    lyra.stats.maxHP = 80;
    lyra.stats.currentHP = 80;
    lyra.stats.attack = 15;
    lyra.stats.defense = 5;
    lyra.loyaltyPoints = 50;
    lyra.mood = CompanionMood::NEUTRAL;
    lyra.experienceLevel = 1;
    lyra.likes = {"Helping innocents", "Precision", "Nature"};
    lyra.dislikes = {"Recklessness", "Cruelty", "Destruction of nature"};
    lyra.moralityPreference = 10;  // Prefers good
    lyra.grantsXPBonus = true;
    lyra.xpBonusPercent = 0.1f;
    lyra.dialogueOptions = {
        "The wind tells many secrets if you listen.",
        "My arrows never miss... well, almost never.",
        "I hunt those who prey on the innocent."
    };
    
    CompanionAbility volley;
    volley.name = "Volley";
    volley.description = "Fire arrows at all enemies for 60% damage each";
    volley.damage = 20;
    volley.cooldown = 4;
    volley.targetsAllEnemies = true;
    lyra.abilities.push_back(volley);
    
    CompanionAbility precision;
    precision.name = "Precision Aura";
    precision.description = "Increases player dodge chance by 10%";
    precision.isPassive = true;
    precision.critChanceBonus = 0.1f;
    lyra.abilities.push_back(precision);
    
    allCompanions.push_back(lyra);
    
    // Create Healer Companion - Brother Aldric
    Companion aldric;
    aldric.id = 2;
    aldric.name = "Brother Aldric";
    aldric.backstory = "A monk from the Order of Light, dedicated to healing the wounded and protecting the innocent.";
    aldric.companionClass = CompanionClass::HEALER;
    aldric.stats.maxHP = 60;
    aldric.stats.currentHP = 60;
    aldric.stats.attack = 5;
    aldric.stats.defense = 8;
    aldric.loyaltyPoints = 50;
    aldric.mood = CompanionMood::NEUTRAL;
    aldric.experienceLevel = 1;
    aldric.likes = {"Mercy", "Peace", "Helping others"};
    aldric.dislikes = {"Evil choices", "Unnecessary killing", "Dark magic"};
    aldric.moralityPreference = 20;  // Strongly prefers good
    aldric.grantsGoldBonus = false;
    aldric.dialogueOptions = {
        "May the Light guide your path.",
        "Healing is not just for the body, but the soul.",
        "Violence should always be the last resort."
    };
    
    CompanionAbility massHeal;
    massHeal.name = "Healing Light";
    massHeal.description = "Heal the player for 40 HP";
    massHeal.healing = 40;
    massHeal.cooldown = 3;
    aldric.abilities.push_back(massHeal);
    
    CompanionAbility healAura;
    healAura.name = "Healing Aura";
    healAura.description = "Increases healing received by 15%";
    healAura.isPassive = true;
    aldric.abilities.push_back(healAura);
    
    allCompanions.push_back(aldric);
    
    // Create Tank Companion - Gorath Ironshield
    Companion gorath;
    gorath.id = 3;
    gorath.name = "Gorath Ironshield";
    gorath.backstory = "A veteran soldier of the Iron Legion, known for his unbreakable defense and unwavering loyalty.";
    gorath.companionClass = CompanionClass::TANK;
    gorath.stats.maxHP = 150;
    gorath.stats.currentHP = 150;
    gorath.stats.attack = 10;
    gorath.stats.defense = 15;
    gorath.loyaltyPoints = 50;
    gorath.mood = CompanionMood::NEUTRAL;
    gorath.experienceLevel = 1;
    gorath.likes = {"Honor", "Bravery", "Protecting others"};
    gorath.dislikes = {"Cowardice", "Theft", "Breaking oaths"};
    gorath.moralityPreference = 5;  // Slightly prefers good
    gorath.grantsLootBonus = false;
    gorath.dialogueOptions = {
        "A shield is only as strong as the arm that wields it.",
        "I will stand between you and any threat.",
        "Honor above all else."
    };
    
    CompanionAbility shieldWall;
    shieldWall.name = "Shield Wall";
    shieldWall.description = "Block the next attack against you";
    shieldWall.shieldAmount = 50;
    shieldWall.cooldown = 5;
    gorath.abilities.push_back(shieldWall);
    
    CompanionAbility defenseAura;
    defenseAura.name = "Iron Defense";
    defenseAura.description = "Increases player defense by 10%";
    defenseAura.isPassive = true;
    gorath.abilities.push_back(defenseAura);
    
    allCompanions.push_back(gorath);
    
    // Create Mage Companion - Elara Flamecaster
    Companion elara;
    elara.id = 4;
    elara.name = "Elara Flamecaster";
    elara.backstory = "A pyromancer who left the Mage's Circle to pursue forbidden knowledge of elemental fusion.";
    elara.companionClass = CompanionClass::MAGE_SUPPORT;
    elara.stats.maxHP = 50;
    elara.stats.currentHP = 50;
    elara.stats.attack = 20;
    elara.stats.defense = 3;
    elara.stats.magicPower = 25;
    elara.loyaltyPoints = 50;
    elara.mood = CompanionMood::NEUTRAL;
    elara.experienceLevel = 1;
    elara.likes = {"Magic", "Knowledge", "Experimentation"};
    elara.dislikes = {"Anti-magic sentiments", "Closed-mindedness", "Destruction of books"};
    elara.moralityPreference = 0;  // Neutral
    elara.grantsXPBonus = true;
    elara.xpBonusPercent = 0.15f;
    elara.dialogueOptions = {
        "Fire is not just destruction - it's transformation.",
        "The arcane arts hold infinite possibilities.",
        "Knowledge is the greatest power of all."
    };
    
    CompanionAbility fireBlast;
    fireBlast.name = "Fire Blast";
    fireBlast.description = "Deal 35 fire damage to an enemy";
    fireBlast.damage = 35;
    fireBlast.cooldown = 2;
    elara.abilities.push_back(fireBlast);
    
    CompanionAbility magicAura;
    magicAura.name = "Arcane Amplification";
    magicAura.description = "Increases player magic damage by 15%";
    magicAura.isPassive = true;
    elara.abilities.push_back(magicAura);
    
    allCompanions.push_back(elara);
    
    // Create Thief Companion - Shadow
    Companion shadow;
    shadow.id = 5;
    shadow.name = "Shadow";
    shadow.backstory = "A mysterious figure from the Thieves' Guild. Their real name and past are unknown to all.";
    shadow.companionClass = CompanionClass::THIEF;
    shadow.stats.maxHP = 70;
    shadow.stats.currentHP = 70;
    shadow.stats.attack = 18;
    shadow.stats.defense = 4;
    shadow.loyaltyPoints = 50;
    shadow.mood = CompanionMood::NEUTRAL;
    shadow.experienceLevel = 1;
    shadow.likes = {"Stealth", "Cunning", "Freedom"};
    shadow.dislikes = {"Authority", "Direct confrontation", "Being in the spotlight"};
    shadow.moralityPreference = -5;  // Slightly prefers chaos
    shadow.grantsGoldBonus = true;
    shadow.goldBonusPercent = 0.2f;
    shadow.grantsLootBonus = true;
    shadow.dialogueOptions = {
        "The best fights are the ones you avoid.",
        "Every lock has a key. Sometimes it's just hidden.",
        "Trust no one. Including me."
    };
    
    CompanionAbility backstab;
    backstab.name = "Backstab";
    backstab.description = "Deal 45 critical damage to an enemy";
    backstab.damage = 45;
    backstab.cooldown = 4;
    shadow.abilities.push_back(backstab);
    
    CompanionAbility lockpick;
    lockpick.name = "Keen Eye";
    lockpick.description = "Better chance to find hidden items and gold";
    lockpick.isPassive = true;
    shadow.abilities.push_back(lockpick);
    
    allCompanions.push_back(shadow);
}

Companion* CompanionManager::findCompanion(int companionId) {
    for (auto& comp : allCompanions) {
        if (comp.id == companionId) {
            return &comp;
        }
    }
    return nullptr;
}

void CompanionManager::triggerLoyaltyDialogue(int companionId, bool isPositive) {
    Companion* comp = findCompanion(companionId);
    if (comp == nullptr) return;
    
    if (isPositive) {
        if (comp->loyaltyPoints >= 80) {
            std::cout << comp->name << ": \"I would follow you to the ends of the earth!\"\n";
        } else if (comp->loyaltyPoints >= 60) {
            std::cout << comp->name << ": \"You're a true friend.\"\n";
        }
    } else {
        if (comp->loyaltyPoints <= 20) {
            std::cout << comp->name << ": \"I'm not sure I can continue like this...\"\n";
        } else if (comp->loyaltyPoints <= 40) {
            std::cout << comp->name << ": \"We need to talk about some things.\"\n";
        }
    }
}

//==============================================================================
// FactionManager Implementation
//==============================================================================

FactionManager::FactionManager(Player& player) : playerRef(player) {
    initializeFactions();
}

void FactionManager::gainReputation(FactionId faction, int amount, const std::string& reason) {
    factionReps[faction].reputationPoints += amount;
    
    std::cout << "Gained " << amount << " reputation with ";
    switch (faction) {
        case FactionId::KNIGHTS_ORDER: std::cout << "Knights Order"; break;
        case FactionId::THIEVES_GUILD: std::cout << "Thieves Guild"; break;
        case FactionId::MAGES_CIRCLE: std::cout << "Mages Circle"; break;
        case FactionId::CULT_OF_SHADOWS: std::cout << "Cult of Shadows"; break;
        case FactionId::MERCHANTS_LEAGUE: std::cout << "Merchants League"; break;
        case FactionId::DRAGON_RIDERS: std::cout << "Dragon Riders"; break;
        case FactionId::UNDEAD_LEGION: std::cout << "Undead Legion"; break;
    }
    std::cout << " (" << reason << ")\n";
    
    checkRankUp(faction);
    checkFactionConflicts(faction);
}

void FactionManager::loseReputation(FactionId faction, int amount, const std::string& reason) {
    factionReps[faction].reputationPoints -= amount;
    if (factionReps[faction].reputationPoints < 0) {
        factionReps[faction].reputationPoints = 0;
    }
    
    std::cout << "Lost " << amount << " reputation with faction (" << reason << ")\n";
    
    factionReps[faction].rank = calculateRank(factionReps[faction].reputationPoints);
}

FactionRank FactionManager::getFactionRank(FactionId faction) const {
    auto it = factionReps.find(faction);
    if (it != factionReps.end()) {
        return it->second.rank;
    }
    return FactionRank::NEUTRAL;
}

int FactionManager::getReputationPoints(FactionId faction) const {
    auto it = factionReps.find(faction);
    if (it != factionReps.end()) {
        return it->second.reputationPoints;
    }
    return 0;
}

void FactionManager::checkRankUp(FactionId faction) {
    FactionRank newRank = calculateRank(factionReps[faction].reputationPoints);
    
    if (newRank != factionReps[faction].rank) {
        factionReps[faction].rank = newRank;
        std::cout << "Faction rank changed to: ";
        switch (newRank) {
            case FactionRank::HOSTILE: std::cout << "Hostile"; break;
            case FactionRank::UNFRIENDLY: std::cout << "Unfriendly"; break;
            case FactionRank::NEUTRAL: std::cout << "Neutral"; break;
            case FactionRank::FRIENDLY: std::cout << "Friendly"; break;
            case FactionRank::HONORED: std::cout << "Honored"; break;
            case FactionRank::REVERED: std::cout << "Revered"; break;
            case FactionRank::EXALTED: std::cout << "Exalted"; break;
        }
        std::cout << "!\n";
    }
}

void FactionManager::displayReputationStatus() const {
    std::cout << "\n========================================\n";
    std::cout << "       FACTION REPUTATION\n";
    std::cout << "========================================\n\n";
    
    for (const auto& pair : factionReps) {
        std::cout << "  ";
        switch (pair.first) {
            case FactionId::KNIGHTS_ORDER: std::cout << "Knights Order"; break;
            case FactionId::THIEVES_GUILD: std::cout << "Thieves Guild"; break;
            case FactionId::MAGES_CIRCLE: std::cout << "Mages Circle"; break;
            case FactionId::CULT_OF_SHADOWS: std::cout << "Cult of Shadows"; break;
            case FactionId::MERCHANTS_LEAGUE: std::cout << "Merchants League"; break;
            case FactionId::DRAGON_RIDERS: std::cout << "Dragon Riders"; break;
            case FactionId::UNDEAD_LEGION: std::cout << "Undead Legion"; break;
        }
        
        std::cout << ": " << pair.second.reputationPoints << " (";
        switch (pair.second.rank) {
            case FactionRank::HOSTILE: std::cout << "Hostile"; break;
            case FactionRank::UNFRIENDLY: std::cout << "Unfriendly"; break;
            case FactionRank::NEUTRAL: std::cout << "Neutral"; break;
            case FactionRank::FRIENDLY: std::cout << "Friendly"; break;
            case FactionRank::HONORED: std::cout << "Honored"; break;
            case FactionRank::REVERED: std::cout << "Revered"; break;
            case FactionRank::EXALTED: std::cout << "Exalted"; break;
        }
        std::cout << ")\n";
    }
}

std::vector<int> FactionManager::getAvailableQuests(FactionId faction) const {
    std::vector<int> quests;
    
    auto it = factionReps.find(faction);
    if (it != factionReps.end()) {
        quests = it->second.unlockedQuests;
    }
    
    return quests;
}

std::vector<Item> FactionManager::getFactionShop(FactionId faction) const {
    std::vector<Item> items;
    
    // Return faction-specific items based on rank
    auto it = factionReps.find(faction);
    if (it != factionReps.end() && static_cast<int>(it->second.rank) >= 1) {
        Item factionItem;
        factionItem.name = "Faction Token";
        factionItem.value = 100;
        items.push_back(factionItem);
    }
    
    return items;
}

bool FactionManager::unlockFactionAbility(FactionId faction) {
    if (getFactionRank(faction) >= FactionRank::EXALTED) {
        std::cout << "Faction ability unlocked!\n";
        return true;
    }
    
    std::cout << "Need Exalted rank to unlock faction ability.\n";
    return false;
}

bool FactionManager::areFactionsAllied(FactionId faction1, FactionId faction2) const {
    auto it = alliedFactions.find(faction1);
    if (it != alliedFactions.end()) {
        return std::find(it->second.begin(), it->second.end(), faction2) != it->second.end();
    }
    return false;
}

void FactionManager::checkFactionConflicts(FactionId gainedRep) {
    auto it = enemyFactions.find(gainedRep);
    if (it != enemyFactions.end()) {
        for (FactionId enemy : it->second) {
            applyFactionConflict(gainedRep, enemy, 25);
        }
    }
}

void FactionManager::declareFactionWar(FactionId faction) {
    factionReps[faction].rank = FactionRank::HOSTILE;
    factionReps[faction].reputationPoints = 0;
    std::cout << "You are now at war with this faction!\n";
}

std::string FactionManager::getCurrentTitle(FactionId faction) const {
    auto it = factionReps.find(faction);
    if (it == factionReps.end()) return "Stranger";
    
    switch (it->second.rank) {
        case FactionRank::HOSTILE: return "Enemy";
        case FactionRank::UNFRIENDLY: return "Outsider";
        case FactionRank::NEUTRAL: return "Stranger";
        case FactionRank::FRIENDLY: return "Friend";
        case FactionRank::HONORED: return "Honored Member";
        case FactionRank::REVERED: return "Champion";
        case FactionRank::EXALTED: return "Paragon";
    }
    return "Unknown";
}

bool FactionManager::canWearTabard(FactionId faction) const {
    auto it = factionReps.find(faction);
    if (it != factionReps.end()) {
        return it->second.hasTabard;
    }
    return false;
}

void FactionManager::initializeFactions() {
    // Initialize all factions with neutral standing
    FactionReputation knights;
    knights.faction = FactionId::KNIGHTS_ORDER;
    knights.reputationPoints = 500;
    knights.rank = FactionRank::NEUTRAL;
    factionReps[FactionId::KNIGHTS_ORDER] = knights;
    
    FactionReputation thieves;
    thieves.faction = FactionId::THIEVES_GUILD;
    thieves.reputationPoints = 500;
    thieves.rank = FactionRank::NEUTRAL;
    factionReps[FactionId::THIEVES_GUILD] = thieves;
    
    FactionReputation mages;
    mages.faction = FactionId::MAGES_CIRCLE;
    mages.reputationPoints = 500;
    mages.rank = FactionRank::NEUTRAL;
    factionReps[FactionId::MAGES_CIRCLE] = mages;
    
    FactionReputation cult;
    cult.faction = FactionId::CULT_OF_SHADOWS;
    cult.reputationPoints = 500;
    cult.rank = FactionRank::NEUTRAL;
    factionReps[FactionId::CULT_OF_SHADOWS] = cult;
    
    FactionReputation merchants;
    merchants.faction = FactionId::MERCHANTS_LEAGUE;
    merchants.reputationPoints = 500;
    merchants.rank = FactionRank::NEUTRAL;
    factionReps[FactionId::MERCHANTS_LEAGUE] = merchants;
    
    FactionReputation dragons;
    dragons.faction = FactionId::DRAGON_RIDERS;
    dragons.reputationPoints = 500;
    dragons.rank = FactionRank::NEUTRAL;
    factionReps[FactionId::DRAGON_RIDERS] = dragons;
    
    FactionReputation undead;
    undead.faction = FactionId::UNDEAD_LEGION;
    undead.reputationPoints = 500;
    undead.rank = FactionRank::NEUTRAL;
    factionReps[FactionId::UNDEAD_LEGION] = undead;
    
    // Set up faction conflicts
    enemyFactions[FactionId::KNIGHTS_ORDER] = {FactionId::THIEVES_GUILD, FactionId::CULT_OF_SHADOWS, FactionId::UNDEAD_LEGION};
    enemyFactions[FactionId::THIEVES_GUILD] = {FactionId::KNIGHTS_ORDER, FactionId::MERCHANTS_LEAGUE};
    enemyFactions[FactionId::CULT_OF_SHADOWS] = {FactionId::KNIGHTS_ORDER, FactionId::DRAGON_RIDERS};
    enemyFactions[FactionId::UNDEAD_LEGION] = {FactionId::KNIGHTS_ORDER, FactionId::DRAGON_RIDERS};
    
    // Set up faction alliances
    alliedFactions[FactionId::KNIGHTS_ORDER] = {FactionId::DRAGON_RIDERS};
    alliedFactions[FactionId::DRAGON_RIDERS] = {FactionId::KNIGHTS_ORDER};
}

FactionRank FactionManager::calculateRank(int reputationPoints) const {
    if (reputationPoints < 100) return FactionRank::HOSTILE;
    if (reputationPoints < 300) return FactionRank::UNFRIENDLY;
    if (reputationPoints < 700) return FactionRank::NEUTRAL;
    if (reputationPoints < 1500) return FactionRank::FRIENDLY;
    if (reputationPoints < 3000) return FactionRank::HONORED;
    if (reputationPoints < 6000) return FactionRank::REVERED;
    return FactionRank::EXALTED;
}

void FactionManager::applyFactionConflict(FactionId faction1, FactionId faction2, int amount) {
    (void)faction1;
    // Gaining rep with one enemy causes rep loss with the other
    factionReps[faction2].reputationPoints -= amount;
    if (factionReps[faction2].reputationPoints < 0) {
        factionReps[faction2].reputationPoints = 0;
    }
    
    std::cout << "(Conflict: lost reputation with opposing faction)\n";
    factionReps[faction2].rank = calculateRank(factionReps[faction2].reputationPoints);
}

}  // namespace AQE
