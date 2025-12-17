/*******************************************************************************
 * C++ Adventure Quest Engine - ENHANCED EDITION
 * Combat.h - Advanced Combat System Header
 * 
 * MASSIVE OVERHAUL - Version 2.0
 * This file contains declarations for the enhanced turn-based combat system:
 * - Elemental damage types and resistances
 * - Combo attack chains
 * - Positioning and tactical movement
 * - Enhanced AI with predictive intent
 * - Counterattacks and reaction abilities
 * - Multi-target and AoE attacks
 * 
 * Author: Adventure Quest Engine Team
 * Version: 2.0 - MASSIVE OVERHAUL
 ******************************************************************************/

#ifndef COMBAT_H
#define COMBAT_H

#include "Types.h"
#include "SkillTree.h"
#include <vector>
#include <string>
#include <queue>

namespace AQE {

//==============================================================================
// ENHANCED COMBAT ENUMS AND STRUCTURES
//==============================================================================

/**
 * @brief Combat positioning affects damage and abilities
 */
enum class CombatPosition {
    FRONT_LINE,          // High damage, takes more hits
    MID_LINE,            // Balanced
    BACK_LINE            // Safe, ranged attacks only
};

/**
 * @brief Advanced combat actions
 */
enum class CombatAction {
    BASIC_ATTACK,
    HEAVY_ATTACK,        // +50% damage, -20% accuracy
    QUICK_ATTACK,        // -30% damage, +20% accuracy, +10% dodge next turn
    USE_SKILL,
    USE_ITEM,
    DEFEND,              // +50% defense, +30% dodge this turn
    CHANGE_POSITION,
    FLEE,
    COUNTERATTACK,       // Reaction ability
    COMBO_FINISHER
};

/**
 * @brief Resistance and vulnerability to elements
 */
struct ElementalResistance {
    DamageElement element;
    float resistancePercent;         // 0.5 = 50% reduction, -0.5 = +50% damage taken
    
    ElementalResistance(DamageElement e = DamageElement::PHYSICAL, float r = 0.0f) 
        : element(e), resistancePercent(r) {}
};

/**
 * @brief Enhanced attack result with positioning and elements
 */
struct EnhancedAttackResult {
    bool hit;
    bool critical;
    bool dodged;
    bool blocked;                    // NEW: Full block
    bool grazing;                    // NEW: Partial hit (50% damage)
    int damageDealt;
    int rawDamage;
    int shieldDamage;                // Damage absorbed by shields
    DamageElement element;
    bool triggeredEffect;            // Burn, freeze, etc.
    StatusEffect triggeredStatusEffect;
    int comboPoints;                 // Combo meter gain
    bool isCounterattack;
    std::string message;
    
    EnhancedAttackResult() : hit(false), critical(false), dodged(false),
                            blocked(false), grazing(false), damageDealt(0),
                            rawDamage(0), shieldDamage(0), element(DamageElement::PHYSICAL),
                            triggeredEffect(false), comboPoints(1), isCounterattack(false) {}
};

/**
 * @brief Combo attack chain
 */
struct CombatCombo {
    std::string name;
    int currentChain;                // Current hits in chain
    int maxChain;                    // Max chain length
    float damageMultiplier;          // 1.1 = +10% per hit
    int comboMeter;                  // 0-100, builds with attacks
    bool finisherAvailable;          // Can use special finisher?
    DamageElement comboElement;      // Element of the combo
    
    CombatCombo() : currentChain(0), maxChain(5), damageMultiplier(1.1f),
                   comboMeter(0), finisherAvailable(false),
                   comboElement(DamageElement::PHYSICAL) {}
};

/**
 * @brief Combat participant (player or companion)
 */
struct Combatant {
    std::string name;
    CombatStats stats;
    CombatPosition position;
    std::vector<StatusEffect> activeEffects;
    bool isPlayer;
    bool isCompanion;
    int companionId;                 // If companion
    CombatCombo combo;
    
    Combatant() : position(CombatPosition::FRONT_LINE), isPlayer(false),
                 isCompanion(false), companionId(-1) {}
};

//==============================================================================
// COMBAT RESULT STRUCTURE (ENHANCED)
//==============================================================================

/**
 * @brief Result of a single attack action
 */
struct AttackResult {
    bool hit;            // Did the attack land?
    bool critical;       // Was it a critical hit?
    bool dodged;         // Did target dodge?
    int damageDealt;     // Final damage after calculations
    int rawDamage;       // Damage before defense
    std::string message; // Combat message to display
    
    AttackResult() : hit(false), critical(false), dodged(false),
                     damageDealt(0), rawDamage(0) {}
};

/**
 * @brief Result of using an ability
 */
struct AbilityResult {
    bool used;           // Was ability used?
    bool success;        // Did it succeed?
    int damageDealt;     // Damage (if offensive)
    int healingDone;     // Healing (if healing ability)
    std::string message; // Result message
    StatusEffect appliedEffect;  // Any status effect applied
    
    AbilityResult() : used(false), success(false), damageDealt(0), healingDone(0) {}
};

/**
 * @brief Overall combat encounter result
 */
struct CombatResult {
    bool playerWon;          // Did player win?
    bool playerFled;         // Did player escape?
    bool playerDied;         // Did player die?
    int experienceGained;    // Total XP earned
    int goldGained;          // Total gold earned
    std::vector<Item> loot;  // Items dropped
    std::vector<std::string> combatLog;  // Full combat log
    
    // NEW: Enhanced stats
    int totalDamageDealt;
    int totalDamageTaken;
    int criticalHitsLanded;
    int dodgesPerformed;
    int combosCompleted;
    int abilitiesUsed;
    int turnsElapsed;
    bool flawlessVictory;    // No damage taken
    
    CombatResult() : playerWon(false), playerFled(false), playerDied(false),
                     experienceGained(0), goldGained(0), totalDamageDealt(0),
                     totalDamageTaken(0), criticalHitsLanded(0), dodgesPerformed(0),
                     combosCompleted(0), abilitiesUsed(0), turnsElapsed(0),
                     flawlessVictory(false) {}
};

//==============================================================================
// COMBAT SYSTEM CLASS
//==============================================================================

/**
 * @brief Main combat system handler
 * 
 * Manages turn-based combat between player and enemies, including
 * attack calculations, abilities, fleeing, and loot distribution.
 */
class CombatSystem {
public:
    /**
     * @brief Constructor
     * @param player Reference to player
     * @param difficulty Game difficulty (1-3)
     */
    CombatSystem(Player& player, int difficulty = 2);
    
    /**
     * @brief Start combat with a group of enemies
     * @param enemies Vector of enemies to fight
     * @return Combat result
     */
    CombatResult startCombat(std::vector<Enemy>& enemies);
    
    /**
     * @brief Start combat with a single enemy
     * @param enemy The enemy to fight
     * @return Combat result
     */
    CombatResult startCombat(Enemy& enemy);
    
    /**
     * @brief Calculate player's effective stats (with equipment/buffs)
     * @return Effective combat stats
     */
    CombatStats getEffectivePlayerStats() const;
    
    /**
     * @brief Calculate enemy's effective stats (with buffs/debuffs)
     * @param enemy The enemy
     * @return Effective combat stats
     */
    CombatStats getEffectiveEnemyStats(const Enemy& enemy) const;

private:
    //==========================================================================
    // PRIVATE MEMBER VARIABLES
    //==========================================================================
    
    Player& playerRef;
    int gameDifficulty;
    std::vector<std::string> combatLog;
    bool combatEnded;
    bool playerVictory;
    
    //==========================================================================
    // PRIVATE METHODS - Combat Flow
    //==========================================================================
    
    /**
     * @brief Main combat loop
     * @param enemies Enemies in combat
     * @return True if player won
     */
    bool combatLoop(std::vector<Enemy>& enemies);
    
    /**
     * @brief Display combat status
     * @param enemies Current enemies
     */
    void displayCombatStatus(const std::vector<Enemy>& enemies) const;

    /**
     * @brief Determine and set upcoming intents for enemies
     * @param enemies Current enemies (mutable for intent fields)
     */
    void rollEnemyIntents(std::vector<Enemy>& enemies);
    
    /**
     * @brief Get player's combat action choice
     * @return Action code (1=attack, 2=ability, 3=item, 4=flee)
     */
    int getPlayerAction() const;
    
    /**
     * @brief Select target from enemy list
     * @param enemies Available targets
     * @return Index of selected enemy
     */
    int selectTarget(const std::vector<Enemy>& enemies) const;
    
    /**
     * @brief Determine turn order based on speed
     * @param enemies Enemies in combat
     * @return Vector of (isPlayer, index) pairs
     */
    std::vector<std::pair<bool, int>> determineTurnOrder(
        const std::vector<Enemy>& enemies) const;
    
    //==========================================================================
    // PRIVATE METHODS - Attack Calculations
    //==========================================================================
    
    /**
     * @brief Calculate and execute player attack
     * @param target Target enemy
     * @return Attack result
     */
    AttackResult playerAttack(Enemy& target);
    
    /**
     * @brief Calculate and execute enemy attack
     * @param attacker The attacking enemy
     * @return Attack result
     */
    AttackResult enemyAttack(const Enemy& attacker);
    
    /**
     * @brief Roll for hit/miss
     * @param attackerSpeed Attacker's speed stat
     * @param targetDodge Target's dodge chance
     * @return True if hit
     */
    bool rollToHit(int attackerSpeed, int targetDodge) const;
    
    /**
     * @brief Roll for critical hit
     * @param critChance Attacker's crit chance
     * @return True if critical
     */
    bool rollCritical(int critChance) const;
    
    /**
     * @brief Calculate final damage
     * @param baseDamage Raw damage
     * @param defense Target's defense
     * @param isCrit Is this a critical hit?
     * @return Final damage value
     */
    int calculateFinalDamage(int baseDamage, int defense, bool isCrit) const;

    /**
     * @brief Apply shield effects before HP damage and return net damage dealt
     */
    int applyShieldThenDamage(int incoming, std::vector<StatusEffect>& effects, int& hpPool) const;
    
    //==========================================================================
    // PRIVATE METHODS - Abilities
    //==========================================================================
    
    /**
     * @brief Use player special ability
     * @param abilityIndex Index of ability to use
     * @param target Target enemy (if applicable)
     * @return Ability result
     */
    AbilityResult usePlayerAbility(int abilityIndex, Enemy* target);
    
    /**
     * @brief Have enemy use its ability
     * @param enemy The enemy using ability
     * @return Ability result
     */
    AbilityResult useEnemyAbility(Enemy& enemy);
    
    /**
     * @brief Check if enemy should use ability
     * @param enemy The enemy to check
     * @return True if ability should be used
     */
    bool shouldEnemyUseAbility(const Enemy& enemy) const;
    
    //==========================================================================
    // PRIVATE METHODS - Status Effects
    //==========================================================================
    
    /**
     * @brief Apply status effect to player
     * @param effect The effect to apply
     */
    void applyStatusToPlayer(const StatusEffect& effect);
    
    /**
     * @brief Process player's active status effects
     */
    void processPlayerStatuses();
    
    /**
     * @brief Process enemy's active status effects
     * @param enemy The enemy to process
     */
    void processEnemyStatuses(Enemy& enemy);

    /**
     * @brief Apply tick effects (DoT/HoT/vulnerability expiration) to a target
     */
    void tickStatuses(std::vector<StatusEffect>& effects, CombatStats& stats, bool isPlayer);

    /**
     * @brief Remove expired statuses and recompute derived shields
     */
    void cleanupStatuses(std::vector<StatusEffect>& effects);
    
    //==========================================================================
    // PRIVATE METHODS - Flee & Items
    //==========================================================================
    
    /**
     * @brief Attempt to flee from combat
     * @param enemies Enemies in combat
     * @return True if escape successful
     */
    bool attemptFlee(const std::vector<Enemy>& enemies);
    
    /**
     * @brief Use item during combat
     * @param itemIndex Index of item in inventory
     * @return True if item was used
     */
    bool useItemInCombat(int itemIndex);
    
    //==========================================================================
    // PRIVATE METHODS - Loot
    //==========================================================================
    
    /**
     * @brief Generate loot from defeated enemies
     * @param enemies Defeated enemies
     * @return Vector of dropped items
     */
    std::vector<Item> generateLoot(const std::vector<Enemy>& enemies);
    
    /**
     * @brief Roll loot table for an enemy
     * @param enemy The defeated enemy
     * @param templates Available item templates
     * @return Dropped items
     */
    std::vector<Item> rollLootTable(const Enemy& enemy);
    
    /**
     * @brief Calculate total experience from enemies
     * @param enemies Defeated enemies
     * @return Total experience points
     */
    int calculateExperience(const std::vector<Enemy>& enemies) const;
    
    /**
     * @brief Calculate total gold from enemies
     * @param enemies Defeated enemies
     * @return Total gold
     */
    int calculateGold(const std::vector<Enemy>& enemies) const;
    
    //==========================================================================
    // PRIVATE METHODS - Logging
    //==========================================================================
    
    /**
     * @brief Add message to combat log
     * @param message Message to log
     */
    void log(const std::string& message);
    
    /**
     * @brief Display combat message with delay
     * @param message Message to display
     * @param delayMs Delay in milliseconds
     */
    void displayMessage(const std::string& message, int delayMs = 500) const;
};

//==============================================================================
// STANDALONE COMBAT FUNCTIONS
//==============================================================================

/**
 * @brief Quick combat encounter (single enemy, no UI)
 * @param player Player reference
 * @param enemy Enemy to fight
 * @return True if player won
 */
bool quickCombat(Player& player, Enemy& enemy);

/**
 * @brief Check if player has combat items
 * @param player Player to check
 * @return True if usable combat items exist
 */
bool hasCombatItems(const Player& player);

/**
 * @brief Get list of usable combat items
 * @param player Player to check
 * @return Vector of item indices
 */
std::vector<int> getCombatItemIndices(const Player& player);

/**
 * @brief Apply difficulty modifier to damage
 * @param damage Base damage
 * @param difficulty Game difficulty (1-3)
 * @param isPlayerDamage True if player is dealing damage
 * @return Modified damage
 */
int applyDifficultyModifier(int damage, int difficulty, bool isPlayerDamage);

/**
 * @brief Generate random combat message
 * @param attackerName Name of attacker
 * @param defenderName Name of defender
 * @param damage Damage dealt
 * @param isCrit Was it a critical hit?
 * @param isMiss Did it miss?
 * @return Formatted combat message
 */
std::string generateCombatMessage(const std::string& attackerName,
                                  const std::string& defenderName,
                                  int damage, bool isCrit, bool isMiss);

}  // namespace AQE

#endif // COMBAT_H
