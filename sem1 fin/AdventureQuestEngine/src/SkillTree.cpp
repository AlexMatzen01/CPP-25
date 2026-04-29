//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Skill tree system implementation (partially implemented - some features scrapped)

#include "../include/SkillTree.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace AQE {

// ---
// SkillTreeManager Implementation
// ---

SkillTreeManager::SkillTreeManager(Player& player) 
    : playerRef(player), selectedSpecialization(0), totalTalentPoints(0), spentTalentPoints(0) {
    initializeTalentTree();
}

void SkillTreeManager::initializeTalentTree() {
    talentTree.clear();
    
    // Build the appropriate tree based on player class
    switch (playerRef.charClass) {
        case CharacterClass::WARRIOR:
            buildWarriorTree();
            break;
        case CharacterClass::MAGE:
            buildMageTree();
            break;
        case CharacterClass::ROGUE:
            buildRogueTree();
            break;
        case CharacterClass::PALADIN:
            buildPaladinTree();
            break;
        case CharacterClass::RANGER:
            buildRogueTree();
            break;
        case CharacterClass::NECROMANCER:
            buildMageTree();
            break;
        case CharacterClass::BARBARIAN:
            buildWarriorTree();
            break;
        case CharacterClass::MONK:
            buildPaladinTree();
            break;
    }
    
    // Initialize combo chains
    currentCombo = ComboChain();
    totalTalentPoints = playerRef.level;
    spentTalentPoints = 0;
}

void SkillTreeManager::displayTalentTree() const {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "                    TALENT TREE\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    std::cout << "Class: " << (playerRef.charClass == CharacterClass::WARRIOR ? "Warrior" :
                            playerRef.charClass == CharacterClass::MAGE ? "Mage" :
                            playerRef.charClass == CharacterClass::ROGUE ? "Rogue" :
                            playerRef.charClass == CharacterClass::PALADIN ? "Paladin" :
                            playerRef.charClass == CharacterClass::RANGER ? "Ranger" :
                            playerRef.charClass == CharacterClass::NECROMANCER ? "Necromancer" :
                            playerRef.charClass == CharacterClass::BARBARIAN ? "Barbarian" : "Monk") << "\n";
    std::cout << "Level: " << playerRef.level << " | Available Points: " << getAvailableTalentPoints() << "\n";
    std::cout << "Specialization: " << getCurrentSpecialization() << "\n\n";
    
    // Display talents by tier
    for (int tier = 1; tier <= 7; ++tier) {
        std::cout << "\n=== TIER " << tier << " ===";
        int talentsInTier = 0;
        for (const auto& t : talentTree) {
            if (t.tier == tier) talentsInTier++;
        }
        std::cout << " (" << talentsInTier << " talents)\n\n";
        
        bool hasTalent = false;
        for (const auto& talent : talentTree) {
            if (talent.tier == tier) {
                hasTalent = true;
                
                // Check if unlockable
                bool canUnlock = canUnlockTalent(talent.id);
                std::string status = talent.currentRank > 0 ? "[UNLOCKED]" : 
                                    canUnlock ? "[AVAILABLE]" : "[LOCKED]";
                
                std::cout << "[" << talent.id << "] " << talent.name << " " << status;
                if (talent.currentRank > 0) {
                    std::cout << " (" << talent.currentRank << "/" << talent.maxRank << ")";
                }
                
                if (talent.type == TalentType::ACTIVE) {
                    std::cout << " {ACTIVE}";
                } else if (talent.type == TalentType::ULTIMATE) {
                    std::cout << " {ULTIMATE}";
                } else if (talent.type == TalentType::SYNERGY) {
                    std::cout << " {SYNERGY}";
                }
                std::cout << "\n  " << talent.description << "\n";
                
                if (!talent.prerequisites.empty()) {
                    std::cout << "  Requirements: ";
                    for (size_t i = 0; i < talent.prerequisites.size(); ++i) {
                        if (i > 0) std::cout << ", ";
                        std::cout << "[" << talent.prerequisites[i] << "]";
                    }
                    std::cout << "\n";
                }
                std::cout << "\n";
            }
        }
        
        if (!hasTalent) {
            std::cout << "  (No talents in this tier)\n";
        }
    }
    std::cout << "\n" << std::string(60, '=') << "\n";
}

bool SkillTreeManager::unlockTalent(int talentId) {
    if (!canUnlockTalent(talentId)) {
        return false;
    }
    
    TalentNode* talent = findTalent(talentId);
    if (talent == nullptr) {
        return false;
    }
    
    talent->currentRank++;
    spentTalentPoints += talent->pointCost;
    
    // Apply bonuses for this single rank increase (not accumulated)
    playerRef.stats.maxHP += talent->hpBonus;
    playerRef.stats.maxMana += talent->manaBonus;
    playerRef.stats.attack += talent->attackBonus;
    playerRef.stats.defense += talent->defenseBonus;
    playerRef.stats.critChance += talent->critChanceBonus;
    playerRef.stats.dodgeChance += talent->dodgeChanceBonus;
    playerRef.stats.speed += talent->speedBonus;
    playerRef.stats.magicPower += talent->magicPowerBonus;
    playerRef.stats.magicResist += talent->magicResistBonus;
    
    std::cout << "Unlocked: " << talent->name << " (Rank " << talent->currentRank << ")\n";
    return true;
}

bool SkillTreeManager::canUnlockTalent(int talentId) const {
    // Find the talent
    const TalentNode* talent = nullptr;
    for (const auto& t : talentTree) {
        if (t.id == talentId) {
            talent = &t;
            break;
        }
    }
    
    if (talent == nullptr) {
        return false;
    }
    
    // Check if at max rank
    if (talent->currentRank >= talent->maxRank) {
        return false;
    }
    
    // Check available points
    if (getAvailableTalentPoints() < talent->pointCost) {
        return false;
    }
    
    // Check prerequisites
    for (int prereqId : talent->prerequisites) {
        const TalentNode* prereq = nullptr;
        for (const auto& t : talentTree) {
            if (t.id == prereqId) {
                prereq = &t;
                break;
            }
        }
        if (prereq == nullptr || prereq->currentRank == 0) {
            return false;
        }
    }
    
    return true;
}

int SkillTreeManager::getAvailableTalentPoints() const {
    return totalTalentPoints - spentTalentPoints;
}

void SkillTreeManager::chooseSpecialization(int specIndex) {
    if (specIndex >= 0 && specIndex < 3) {
        selectedSpecialization = specIndex;
        std::cout << "Specialization chosen: " << getCurrentSpecialization() << "\n";
    }
}

std::string SkillTreeManager::getCurrentSpecialization() const {
    switch (playerRef.charClass) {
        case CharacterClass::WARRIOR:
            switch (selectedSpecialization) {
                case 0: return "Berserker";
                case 1: return "Tank";
                case 2: return "Bladestorm";
            }
            break;
        case CharacterClass::MAGE:
            switch (selectedSpecialization) {
                case 0: return "Pyromancer";
                case 1: return "Cryomancer";
                case 2: return "Arcanist";
            }
            break;
        case CharacterClass::ROGUE:
            switch (selectedSpecialization) {
                case 0: return "Assassin";
                case 1: return "Shadowdancer";
                case 2: return "Poisoner";
            }
            break;
        case CharacterClass::PALADIN:
            switch (selectedSpecialization) {
                case 0: return "Crusader";
                case 1: return "Protector";
                case 2: return "Justicar";
            }
            break;
        case CharacterClass::RANGER:
            switch (selectedSpecialization) {
                case 0: return "Sharpshooter";
                case 1: return "Beastmaster";
                case 2: return "Skirmisher";
            }
            break;
        case CharacterClass::NECROMANCER:
            switch (selectedSpecialization) {
                case 0: return "Deathbinder";
                case 1: return "Plaguecaller";
                case 2: return "Soulweaver";
            }
            break;
        case CharacterClass::BARBARIAN:
            switch (selectedSpecialization) {
                case 0: return "Ravager";
                case 1: return "Juggernaut";
                case 2: return "Warcry";
            }
            break;
        case CharacterClass::MONK:
            switch (selectedSpecialization) {
                case 0: return "Windwalker";
                case 1: return "Iron Body";
                case 2: return "Spirit Fist";
            }
            break;
    }
    return "None";
}

void SkillTreeManager::applyAllTalentBonuses() {
    // Recalculate total bonuses from all talents and apply them
    // Useful for save/load or talent respec scenarios
    
    int totalHpBonus = 0, totalManaBonus = 0, totalAttackBonus = 0;
    int totalDefenseBonus = 0, totalCritBonus = 0, totalDodgeBonus = 0;
    int totalSpeedBonus = 0, totalMagicBonus = 0, totalResistBonus = 0;
    
    for (const auto& talent : talentTree) {
        if (talent.currentRank > 0) {
            totalHpBonus += talent.hpBonus * talent.currentRank;
            totalManaBonus += talent.manaBonus * talent.currentRank;
            totalAttackBonus += talent.attackBonus * talent.currentRank;
            totalDefenseBonus += talent.defenseBonus * talent.currentRank;
            totalCritBonus += talent.critChanceBonus * talent.currentRank;
            totalDodgeBonus += talent.dodgeChanceBonus * talent.currentRank;
            totalSpeedBonus += talent.speedBonus * talent.currentRank;
            totalMagicBonus += talent.magicPowerBonus * talent.currentRank;
            totalResistBonus += talent.magicResistBonus * talent.currentRank;
        }
    }
    
    std::cout << "\nTalent Bonuses Applied:\n";
    std::cout << "  HP+" << totalHpBonus << " Mana+" << totalManaBonus 
              << " ATK+" << totalAttackBonus << " DEF+" << totalDefenseBonus
              << " CRIT+" << totalCritBonus << " DODGE+" << totalDodgeBonus
              << " SPD+" << totalSpeedBonus << " MAG+" << totalMagicBonus 
              << " RES+" << totalResistBonus << "\n\n";
}

void SkillTreeManager::recalculateStats() {
    // Recalculate bonuses - same as applyAllTalentBonuses
    applyAllTalentBonuses();
}

bool SkillTreeManager::interactiveTalentMenu() {
    bool inMenu = true;
    while (inMenu) {
        displayTalentTree();
        std::cout << "\n1. Unlock Talent\n";
        std::cout << "2. View Specializations\n";
        std::cout << "3. View Unlocked Talents\n";
        std::cout << "4. View Active Abilities\n";
        std::cout << "0. Exit Talent Tree\n\n";
        
        int choice = 0;
        std::cout << "Choice: ";
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: {
                system("clear");
                std::cout << "\nEnter Talent ID to unlock: ";
                int talentId = 0;
                std::cin >> talentId;
                std::cin.ignore();
                
                if (!canUnlockTalent(talentId)) {
                    std::cout << "\nCannot unlock this talent! Check requirements and available points.\n";
                } else {
                    if (unlockTalent(talentId)) {
                        std::cout << "\nTalent unlocked successfully!\n";
                    } else {
                        std::cout << "\nFailed to unlock talent.\n";
                    }
                }
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                system("clear");
                break;
            }
            case 2: {
                system("clear");
                std::cout << "\nSpecializations for " << playerRef.name << ":\n";
                std::cout << "Current: " << getCurrentSpecialization() << "\n\n";
                
                std::cout << "1. First Specialization Path\n";
                std::cout << "2. Second Specialization Path\n";
                std::cout << "3. Third Specialization Path\n";
                std::cout << "0. Cancel\n";
                std::cout << "Choice: ";
                int specChoice = 0;
                std::cin >> specChoice;
                std::cin.ignore();
                
                if (specChoice > 0 && specChoice <= 3) {
                    chooseSpecialization(specChoice - 1);
                    std::cout << "\nSpecialization updated!\n";
                }
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                system("clear");
                break;
            }
            case 3: {
                system("clear");
                auto unlocked = getUnlockedTalents();
                std::cout << "\n=== Unlocked Talents (" << unlocked.size() << ") ===\n";
                for (const auto& talent : unlocked) {
                    std::cout << "[" << talent.id << "] " << talent.name 
                              << " (Rank " << talent.currentRank << "/" << talent.maxRank << ")";
                    if (talent.type == TalentType::ACTIVE) std::cout << " [ACTIVE]";
                    else if (talent.type == TalentType::ULTIMATE) std::cout << " [ULTIMATE]";
                    std::cout << "\n  " << talent.description << "\n\n";
                }
                if (unlocked.empty()) {
                    std::cout << "You haven't unlocked any talents yet!\n";
                }
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                system("clear");
                break;
            }
            case 4: {
                system("clear");
                auto abilities = getActiveAbilities();
                std::cout << "\n=== Active Abilities (" << abilities.size() << ") ===\n";
                for (size_t i = 0; i < abilities.size(); ++i) {
                    const auto& ability = abilities[i];
                    std::cout << "[" << (i + 1) << "] " << ability.name 
                              << " (Mana: " << ability.manaCost << ", CD: " << ability.cooldown << ")\n";
                    std::cout << "  Damage: x" << ability.damageMultiplier;
                    if (ability.isMagical) std::cout << " [MAGICAL]";
                    if (ability.isHeal) std::cout << " [HEAL]";
                    std::cout << "\n";
                }
                if (abilities.empty()) {
                    std::cout << "You have no active abilities unlocked!\n";
                }
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                system("clear");
                break;
            }
            case 0:
                inMenu = false;
                break;
            default:
                system("clear");
                std::cout << "Invalid choice!\n";
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
        }
    }
    return true;
}

std::vector<Skill> SkillTreeManager::getActiveAbilities() const {
    std::vector<Skill> abilities;
    
    for (const auto& talent : talentTree) {
        if (talent.currentRank > 0 && 
            (talent.type == TalentType::ACTIVE || talent.type == TalentType::ULTIMATE)) {
            abilities.push_back(talent.ability);
        }
    }
    
    return abilities;
}

bool SkillTreeManager::useAbility(int abilityId, Enemy& target) {
    TalentNode* talent = findTalent(abilityId);
    if (talent == nullptr || talent->currentRank == 0) {
        return false;
    }
    
    if (talent->type != TalentType::ACTIVE && talent->type != TalentType::ULTIMATE) {
        return false;
    }
    
    // Check cooldown
    if (talent->ability.currentCooldown > 0) {
        std::cout << talent->ability.name << " is on cooldown!\n";
        return false;
    }
    
    // Check mana
    if (playerRef.stats.currentMana < talent->ability.manaCost) {
        std::cout << "Not enough mana!\n";
        return false;
    }
    
    // Use the ability
    playerRef.stats.currentMana -= talent->ability.manaCost;
    talent->ability.currentCooldown = talent->ability.cooldown;
    
    // Calculate damage
    int damage = static_cast<int>(playerRef.stats.attack * talent->ability.damageMultiplier);
    if (talent->ability.isMagical) {
        damage = static_cast<int>(playerRef.stats.magicPower * talent->ability.damageMultiplier);
    }
    
    // Apply damage to target
    target.stats.currentHP -= damage;
    
    std::cout << "Used " << talent->ability.name << " for " << damage << " damage!\n";
    
    // Track for combos
    registerSkillUse(abilityId);
    
    return true;
}

void SkillTreeManager::updateCooldowns() {
    for (auto& talent : talentTree) {
        if (talent.ability.currentCooldown > 0) {
            talent.ability.currentCooldown--;
        }
    }
}

void SkillTreeManager::registerSkillUse(int skillId) {
    currentCombo.skillSequence.push_back(skillId);
    currentCombo.currentStep++;
    
    if (checkComboCompletion()) {
        std::cout << "COMBO COMPLETE: " << currentCombo.name << "!\n";
        resetCombo();
    }
}

bool SkillTreeManager::checkComboCompletion() {
    // Check if current skill sequence matches any combo
    for (const auto& combo : combos) {
        if (currentCombo.skillSequence == combo.skillSequence) {
            return true;
        }
    }
    return false;
}

void SkillTreeManager::resetCombo() {
    currentCombo.skillSequence.clear();
    currentCombo.currentStep = 0;
}

std::string SkillTreeManager::getCurrentComboProgress() const {
    std::stringstream ss;
    ss << "Combo progress: " << currentCombo.currentStep << " hits";
    return ss.str();
}

bool SkillTreeManager::hasPassive(const std::string& passiveName) const {
    for (const auto& talent : talentTree) {
        if (talent.name == passiveName && talent.currentRank > 0) {
            return true;
        }
    }
    return false;
}

float SkillTreeManager::getPassiveValue(const std::string& passiveName) const {
    for (const auto& talent : talentTree) {
        if (talent.name == passiveName && talent.currentRank > 0) {
            return talent.damageMultiplier * talent.currentRank;
        }
    }
    return 0.0f;
}

std::string SkillTreeManager::serializeTalents() const {
    std::stringstream ss;
    ss << selectedSpecialization << ";";
    for (const auto& talent : talentTree) {
        ss << talent.id << ":" << talent.currentRank << ",";
    }
    return ss.str();
}

void SkillTreeManager::deserializeTalents(const std::string& data) {
    if (data.empty()) return;
    
    std::stringstream ss(data);
    std::string token;
    
    // First token is specialization
    if (std::getline(ss, token, ';')) {
        selectedSpecialization = std::stoi(token);
    }
    
    // Rest are talent ranks
    while (std::getline(ss, token, ',')) {
        if (token.empty()) continue;
        size_t colonPos = token.find(':');
        if (colonPos != std::string::npos) {
            int id = std::stoi(token.substr(0, colonPos));
            int rank = std::stoi(token.substr(colonPos + 1));
            TalentNode* talent = findTalent(id);
            if (talent) {
                talent->currentRank = rank;
            }
        }
    }
}

void SkillTreeManager::buildWarriorTree() {
    // Tier 1 - Basic talents
    TalentNode strength;
    strength.id = 101;
    strength.name = "Strength Training";
    strength.description = "Increase attack by 2 per rank";
    strength.type = TalentType::PASSIVE;
    strength.tier = 1;
    strength.maxRank = 5;
    strength.attackBonus = 2;
    talentTree.push_back(strength);
    
    TalentNode vitality;
    vitality.id = 102;
    vitality.name = "Vitality";
    vitality.description = "Increase max HP by 10 per rank";
    vitality.type = TalentType::PASSIVE;
    vitality.tier = 1;
    vitality.maxRank = 5;
    vitality.hpBonus = 10;
    talentTree.push_back(vitality);
    
    // Tier 2 - Active skill
    TalentNode powerStrike;
    powerStrike.id = 201;
    powerStrike.name = "Power Strike";
    powerStrike.description = "A powerful strike dealing 150% damage";
    powerStrike.type = TalentType::ACTIVE;
    powerStrike.tier = 2;
    powerStrike.maxRank = 3;
    powerStrike.prerequisites = {101};
    powerStrike.ability.name = "Power Strike";
    powerStrike.ability.manaCost = 15;
    powerStrike.ability.cooldown = 3;
    powerStrike.ability.damageMultiplier = 1.5f;
    talentTree.push_back(powerStrike);
    
    TalentNode ironSkin;
    ironSkin.id = 202;
    ironSkin.name = "Iron Skin";
    ironSkin.description = "Increase defense by 3 per rank";
    ironSkin.type = TalentType::PASSIVE;
    ironSkin.tier = 2;
    ironSkin.maxRank = 5;
    ironSkin.prerequisites = {102};
    ironSkin.defenseBonus = 3;
    talentTree.push_back(ironSkin);
    
    // Tier 3 - Specialization talents
    TalentNode berserkerRage;
    berserkerRage.id = 301;
    berserkerRage.name = "Berserker Rage";
    berserkerRage.description = "Deal 20% more damage when below 30% HP";
    berserkerRage.type = TalentType::PASSIVE;
    berserkerRage.tier = 3;
    berserkerRage.maxRank = 3;
    berserkerRage.prerequisites = {201};
    berserkerRage.damageMultiplier = 1.2f;
    talentTree.push_back(berserkerRage);
    
    // Tier 4 - Ultimate
    TalentNode devastatingBlow;
    devastatingBlow.id = 401;
    devastatingBlow.name = "Devastating Blow";
    devastatingBlow.description = "Ultimate: Deal 300% damage with guaranteed crit";
    devastatingBlow.type = TalentType::ULTIMATE;
    devastatingBlow.tier = 4;
    devastatingBlow.maxRank = 1;
    devastatingBlow.prerequisites = {301};
    devastatingBlow.ability.name = "Devastating Blow";
    devastatingBlow.ability.manaCost = 40;
    devastatingBlow.ability.cooldown = 10;
    devastatingBlow.ability.damageMultiplier = 3.0f;
    talentTree.push_back(devastatingBlow);
}

void SkillTreeManager::buildMageTree() {
    // Tier 1 - Basic talents
    TalentNode arcaneIntelligence;
    arcaneIntelligence.id = 101;
    arcaneIntelligence.name = "Arcane Intelligence";
    arcaneIntelligence.description = "Increase magic power by 3 per rank";
    arcaneIntelligence.type = TalentType::PASSIVE;
    arcaneIntelligence.tier = 1;
    arcaneIntelligence.maxRank = 5;
    arcaneIntelligence.magicPowerBonus = 3;
    talentTree.push_back(arcaneIntelligence);
    
    TalentNode manaPool;
    manaPool.id = 102;
    manaPool.name = "Expanded Mind";
    manaPool.description = "Increase max mana by 15 per rank";
    manaPool.type = TalentType::PASSIVE;
    manaPool.tier = 1;
    manaPool.maxRank = 5;
    manaPool.manaBonus = 15;
    talentTree.push_back(manaPool);
    
    // Tier 2 - Active skill
    TalentNode fireball;
    fireball.id = 201;
    fireball.name = "Fireball";
    fireball.description = "Launch a ball of fire dealing 200% magic damage";
    fireball.type = TalentType::ACTIVE;
    fireball.tier = 2;
    fireball.maxRank = 3;
    fireball.prerequisites = {101};
    fireball.ability.name = "Fireball";
    fireball.ability.manaCost = 20;
    fireball.ability.cooldown = 2;
    fireball.ability.damageMultiplier = 2.0f;
    fireball.ability.isMagical = true;
    fireball.element = DamageElement::FIRE;
    talentTree.push_back(fireball);
    
    TalentNode frostNova;
    frostNova.id = 202;
    frostNova.name = "Frost Nova";
    frostNova.description = "Ice explosion that slows enemies";
    frostNova.type = TalentType::ACTIVE;
    frostNova.tier = 2;
    frostNova.maxRank = 3;
    frostNova.prerequisites = {102};
    frostNova.ability.name = "Frost Nova";
    frostNova.ability.manaCost = 25;
    frostNova.ability.cooldown = 4;
    frostNova.ability.damageMultiplier = 1.5f;
    frostNova.ability.isMagical = true;
    frostNova.element = DamageElement::FROST;
    talentTree.push_back(frostNova);
    
    // Tier 3 - Specialization
    TalentNode pyroblast;
    pyroblast.id = 301;
    pyroblast.name = "Pyroblast";
    pyroblast.description = "Massive fire attack dealing 250% damage";
    pyroblast.type = TalentType::ACTIVE;
    pyroblast.tier = 3;
    pyroblast.maxRank = 3;
    pyroblast.prerequisites = {201};
    pyroblast.ability.name = "Pyroblast";
    pyroblast.ability.manaCost = 35;
    pyroblast.ability.cooldown = 5;
    pyroblast.ability.damageMultiplier = 2.5f;
    pyroblast.ability.isMagical = true;
    pyroblast.element = DamageElement::FIRE;
    talentTree.push_back(pyroblast);
    
    // Tier 4 - Ultimate
    TalentNode meteorStorm;
    meteorStorm.id = 401;
    meteorStorm.name = "Meteor Storm";
    meteorStorm.description = "Ultimate: Rain meteors dealing 400% AoE damage";
    meteorStorm.type = TalentType::ULTIMATE;
    meteorStorm.tier = 4;
    meteorStorm.maxRank = 1;
    meteorStorm.prerequisites = {301};
    meteorStorm.ability.name = "Meteor Storm";
    meteorStorm.ability.manaCost = 60;
    meteorStorm.ability.cooldown = 15;
    meteorStorm.ability.damageMultiplier = 4.0f;
    meteorStorm.ability.isMagical = true;
    meteorStorm.element = DamageElement::FIRE;
    talentTree.push_back(meteorStorm);
}

void SkillTreeManager::buildRogueTree() {
    // Tier 1
    TalentNode agility;
    agility.id = 101;
    agility.name = "Agility";
    agility.description = "Increase dodge chance by 2% per rank";
    agility.type = TalentType::PASSIVE;
    agility.tier = 1;
    agility.maxRank = 5;
    agility.dodgeChanceBonus = 2;
    talentTree.push_back(agility);
    
    TalentNode precision;
    precision.id = 102;
    precision.name = "Precision";
    precision.description = "Increase crit chance by 3% per rank";
    precision.type = TalentType::PASSIVE;
    precision.tier = 1;
    precision.maxRank = 5;
    precision.critChanceBonus = 3;
    talentTree.push_back(precision);
    
    // Tier 2
    TalentNode backstab;
    backstab.id = 201;
    backstab.name = "Backstab";
    backstab.description = "Strike from shadows for 250% damage";
    backstab.type = TalentType::ACTIVE;
    backstab.tier = 2;
    backstab.maxRank = 3;
    backstab.prerequisites = {102};
    backstab.ability.name = "Backstab";
    backstab.ability.manaCost = 25;
    backstab.ability.cooldown = 4;
    backstab.ability.damageMultiplier = 2.5f;
    talentTree.push_back(backstab);
    
    TalentNode evasion;
    evasion.id = 202;
    evasion.name = "Evasion";
    evasion.description = "Increase dodge by 5% per rank";
    evasion.type = TalentType::PASSIVE;
    evasion.tier = 2;
    evasion.maxRank = 3;
    evasion.prerequisites = {101};
    evasion.dodgeChanceBonus = 5;
    talentTree.push_back(evasion);
    
    // Tier 3
    TalentNode venomousBlade;
    venomousBlade.id = 301;
    venomousBlade.name = "Venomous Blade";
    venomousBlade.description = "Attacks apply poison damage over time";
    venomousBlade.type = TalentType::PASSIVE;
    venomousBlade.tier = 3;
    venomousBlade.maxRank = 3;
    venomousBlade.prerequisites = {201};
    venomousBlade.element = DamageElement::SHADOW;
    talentTree.push_back(venomousBlade);
    
    // Tier 4 - Ultimate
    TalentNode assassinate;
    assassinate.id = 401;
    assassinate.name = "Assassinate";
    assassinate.description = "Ultimate: Deal 500% damage with guaranteed crit";
    assassinate.type = TalentType::ULTIMATE;
    assassinate.tier = 4;
    assassinate.maxRank = 1;
    assassinate.prerequisites = {301};
    assassinate.ability.name = "Assassinate";
    assassinate.ability.manaCost = 50;
    assassinate.ability.cooldown = 12;
    assassinate.ability.damageMultiplier = 5.0f;
    talentTree.push_back(assassinate);
}

void SkillTreeManager::buildPaladinTree() {
    // Tier 1
    TalentNode divineStrength;
    divineStrength.id = 101;
    divineStrength.name = "Divine Strength";
    divineStrength.description = "Increase attack and defense by 2 per rank";
    divineStrength.type = TalentType::PASSIVE;
    divineStrength.tier = 1;
    divineStrength.maxRank = 5;
    divineStrength.attackBonus = 2;
    divineStrength.defenseBonus = 2;
    talentTree.push_back(divineStrength);
    
    TalentNode holyResilience;
    holyResilience.id = 102;
    holyResilience.name = "Holy Resilience";
    holyResilience.description = "Increase HP by 8 and magic resist by 2 per rank";
    holyResilience.type = TalentType::PASSIVE;
    holyResilience.tier = 1;
    holyResilience.maxRank = 5;
    holyResilience.hpBonus = 8;
    holyResilience.magicResistBonus = 2;
    talentTree.push_back(holyResilience);
    
    // Tier 2
    TalentNode holySmite;
    holySmite.id = 201;
    holySmite.name = "Holy Smite";
    holySmite.description = "Smite enemy with holy light for 180% damage";
    holySmite.type = TalentType::ACTIVE;
    holySmite.tier = 2;
    holySmite.maxRank = 3;
    holySmite.prerequisites = {101};
    holySmite.ability.name = "Holy Smite";
    holySmite.ability.manaCost = 20;
    holySmite.ability.cooldown = 3;
    holySmite.ability.damageMultiplier = 1.8f;
    holySmite.ability.isMagical = true;
    holySmite.element = DamageElement::HOLY;
    talentTree.push_back(holySmite);
    
    TalentNode layOnHands;
    layOnHands.id = 202;
    layOnHands.name = "Lay on Hands";
    layOnHands.description = "Heal yourself for a significant amount";
    layOnHands.type = TalentType::ACTIVE;
    layOnHands.tier = 2;
    layOnHands.maxRank = 3;
    layOnHands.prerequisites = {102};
    layOnHands.ability.name = "Lay on Hands";
    layOnHands.ability.manaCost = 30;
    layOnHands.ability.cooldown = 5;
    layOnHands.ability.damageMultiplier = 2.0f;
    layOnHands.ability.isHeal = true;
    layOnHands.ability.isMagical = true;
    talentTree.push_back(layOnHands);
    
    // Tier 3
    TalentNode divineShield;
    divineShield.id = 301;
    divineShield.name = "Divine Shield";
    divineShield.description = "Grant shield absorbing damage";
    divineShield.type = TalentType::ACTIVE;
    divineShield.tier = 3;
    divineShield.maxRank = 3;
    divineShield.prerequisites = {202};
    divineShield.ability.name = "Divine Shield";
    divineShield.ability.manaCost = 35;
    divineShield.ability.cooldown = 8;
    talentTree.push_back(divineShield);
    
    // Tier 4 - Ultimate
    TalentNode divineJudgment;
    divineJudgment.id = 401;
    divineJudgment.name = "Divine Judgment";
    divineJudgment.description = "Ultimate: Deal 250% holy damage and heal 50% of damage dealt";
    divineJudgment.type = TalentType::ULTIMATE;
    divineJudgment.tier = 4;
    divineJudgment.maxRank = 1;
    divineJudgment.prerequisites = {201, 301};
    divineJudgment.ability.name = "Divine Judgment";
    divineJudgment.ability.manaCost = 50;
    divineJudgment.ability.cooldown = 12;
    divineJudgment.ability.damageMultiplier = 2.5f;
    divineJudgment.ability.isMagical = true;
    divineJudgment.element = DamageElement::HOLY;
    divineJudgment.grantsVampirism = true;
    divineJudgment.vampirismPercent = 0.5f;
    talentTree.push_back(divineJudgment);
}

std::vector<TalentNode> SkillTreeManager::getUnlockedTalents() const {
    std::vector<TalentNode> unlocked;
    for (const auto& talent : talentTree) {
        if (talent.currentRank > 0) {
            unlocked.push_back(talent);
        }
    }
    return unlocked;
}

TalentNode* SkillTreeManager::findTalent(int talentId) {
    for (auto& talent : talentTree) {
        if (talent.id == talentId) {
            return &talent;
        }
    }
    return nullptr;
}

void SkillTreeManager::displaySkillLevelUpMenu() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "          SKILL POINT ALLOCATION\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    std::cout << "Available Talent Points: " << getAvailableTalentPoints() << "\n";
    std::cout << "Player Level: " << playerRef.level << "\n\n";
    
    std::cout << "You have earned new talent points!\n";
    std::cout << "\n1. Browse Talent Tree\n";
    std::cout << "2. Quick Allocate\n";
    std::cout << "0. Skip for now\n\n";
    
    int choice = 0;
    std::cout << "Choice: ";
    std::cin >> choice;
    std::cin.ignore();
    
    if (choice == 1) {
        interactiveTalentMenu();
    } else if (choice == 2) {
        std::cout << "\nQuick allocate: Enter Talent ID to add rank to: ";
        int talentId = 0;
        std::cin >> talentId;
        std::cin.ignore();
        
        if (canUnlockTalent(talentId)) {
            unlockTalent(talentId);
        } else {
            std::cout << "Cannot unlock that talent!\n";
        }
    }
}

}  // namespace AQE
