/*******************************************************************************
 * C++ Adventure Quest Engine - ENHANCED EDITION
 * SkillTree.h - Advanced Skill Tree and Talent System
 * 
 * This file implements a comprehensive skill tree system with:
 * - Class-specific talent trees (3 branches per class)
 * - Active and passive abilities
 * - Synergy bonuses
 * - Specialization paths
 * 
 * Author: Adventure Quest Engine Team
 * Version: 2.0 - MASSIVE OVERHAUL
 ******************************************************************************/

#ifndef SKILLTREE_H
#define SKILLTREE_H

#include "Types.h"
#include <vector>
#include <string>
#include <map>

namespace AQE {

//==============================================================================
// TALENT NODE SYSTEM
//==============================================================================

enum class TalentType {
    PASSIVE,         // Always active bonus
    ACTIVE,          // Usable ability
    ULTIMATE,        // Powerful capstone ability
    SYNERGY          // Unlocks combo interactions
};

enum class DamageElement {
    PHYSICAL,        // Normal damage
    FIRE,            // Burn DoT
    ICE,             // Slow/freeze
    LIGHTNING,       // Chain to nearby enemies
    POISON,          // Strong DoT
    HOLY,            // Extra vs undead
    SHADOW,          // Lifesteal
    ARCANE           // Pure magic damage
};

struct TalentNode {
    int id;
    std::string name;
    std::string description;
    TalentType type;
    int tier;                        // 1-7 tiers
    std::vector<int> prerequisites;  // Talent IDs required
    int currentRank;                 // 0-5 ranks
    int maxRank;                     // Usually 5
    int pointCost;                   // Usually 1 per rank
    
    // Passive bonuses (per rank)
    int hpBonus;
    int manaBonus;
    int attackBonus;
    int defenseBonus;
    int critChanceBonus;
    int critDamageBonus;
    int dodgeChanceBonus;
    int speedBonus;
    int magicPowerBonus;
    int magicResistBonus;
    float damageMultiplier;          // 1.05 = 5% per rank
    float cooldownReduction;         // 0.05 = 5% per rank
    float resourceRegen;             // Mana/HP regen bonus
    
    // Active ability data
    Skill ability;                   // Full ability definition
    DamageElement element;           // Elemental type
    
    // Special effects
    bool grantsDoubleJump;           // Movement ability
    bool grantsShieldRecharge;       // Shield on kill
    bool grantsVampirism;            // Lifesteal
    float vampirismPercent;          // % of damage healed
    bool grantsThorns;               // Reflect damage
    int thornsPercent;               // % damage reflected
    bool grantsCounterattack;        // Free attack when dodging
    
    TalentNode() : id(0), type(TalentType::PASSIVE), tier(1), currentRank(0),
                   maxRank(5), pointCost(1), hpBonus(0), manaBonus(0),
                   attackBonus(0), defenseBonus(0), critChanceBonus(0),
                   critDamageBonus(0), dodgeChanceBonus(0), speedBonus(0),
                   magicPowerBonus(0), magicResistBonus(0), damageMultiplier(1.0f),
                   cooldownReduction(0.0f), resourceRegen(0.0f),
                   element(DamageElement::PHYSICAL), grantsDoubleJump(false),
                   grantsShieldRecharge(false), grantsVampirism(false),
                   vampirismPercent(0.0f), grantsThorns(false), thornsPercent(0),
                   grantsCounterattack(false) {}
};

//==============================================================================
// SPECIALIZATION PATHS
//==============================================================================

enum class WarriorSpec {
    BERSERKER,       // High damage, risky gameplay
    TANK,            // Defense and shields
    BLADESTORM       // AoE and movement
};

enum class MageSpec {
    PYROMANCER,      // Fire damage
    CRYOMANCER,      // Ice control
    ARCANIST         // Pure magic power
};

enum class RogueSpec {
    ASSASSIN,        // Single target burst
    SHADOWDANCER,    // Stealth and evasion
    POISONER         // DoTs and debuffs
};

enum class PaladinSpec {
    CRUSADER,        // Holy damage
    PROTECTOR,       // Healing and defense
    JUSTICAR         // Balanced offense/defense
};

//==============================================================================
// COMBO SYSTEM
//==============================================================================

struct ComboChain {
    std::string name;
    std::vector<int> skillSequence;  // Order of skill IDs
    int currentStep;                 // Progress in chain
    int bonusDamage;                 // Extra damage on completion
    StatusEffect bonusEffect;        // Effect applied on completion
    float damageMultiplier;          // Final hit multiplier
    
    ComboChain() : currentStep(0), bonusDamage(0), damageMultiplier(1.0f) {}
};

//==============================================================================
// SKILL TREE MANAGER
//==============================================================================

class SkillTreeManager {
public:
    SkillTreeManager(Player& player);
    
    // Tree management
    void initializeTalentTree();
    void displayTalentTree() const;
    bool unlockTalent(int talentId);
    bool canUnlockTalent(int talentId) const;
    int getAvailableTalentPoints() const;
    
    // Specialization
    void chooseSpecialization(int specIndex);
    std::string getCurrentSpecialization() const;
    
    // Stat calculation
    void applyAllTalentBonuses();
    void recalculateStats();
    
    // Active abilities
    std::vector<Skill> getActiveAbilities() const;
    bool useAbility(int abilityId, Enemy& target);
    void updateCooldowns();
    
    // Combo system
    void registerSkillUse(int skillId);
    bool checkComboCompletion();
    void resetCombo();
    std::string getCurrentComboProgress() const;
    
    // Passive effects
    bool hasPassive(const std::string& passiveName) const;
    float getPassiveValue(const std::string& passiveName) const;
    
    // Save/Load
    std::string serializeTalents() const;
    void deserializeTalents(const std::string& data);
    
private:
    Player& playerRef;
    std::vector<TalentNode> talentTree;
    std::map<CharacterClass, std::vector<TalentNode>> classTrees;
    std::vector<ComboChain> combos;
    ComboChain currentCombo;
    int selectedSpecialization;
    int totalTalentPoints;
    int spentTalentPoints;
    
    // Internal helpers
    void buildWarriorTree();
    void buildMageTree();
    void buildRogueTree();
    void buildPaladinTree();
    std::vector<TalentNode> getUnlockedTalents() const;
    TalentNode* findTalent(int talentId);
};

}  // namespace AQE

#endif // SKILLTREE_H
