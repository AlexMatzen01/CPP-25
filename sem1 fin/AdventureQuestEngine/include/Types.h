//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Core data structures for the game engine

#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace AQE {  // Adventure Quest Engine namespace

// ---
// ENUMERATIONS
// ---

/**
// Item rarity tiers affecting drop rates and power
 */
enum class ItemRarity {
    COMMON,      // 50% drop rate
    UNCOMMON,    // 30% drop rate
    RARE,        // 15% drop rate
    EPIC,        // 4% drop rate
    LEGENDARY    // 1% drop rate
};

/**
// Types of items available in the game
 */
enum class ItemType {
    CONSUMABLE,   // Healing potions, food, etc.
    WEAPON,       // Swords, axes, staffs
    ARMOR,        // Defensive gear
    ACCESSORY,    // Rings, amulets with buffs
    KEY,          // Keys for locked doors/chests
    QUEST,        // Quest-specific items
    SPECIAL       // Unique items for special endings
};

/**
// Types of rooms in the dungeon
 */
enum class RoomType {
    NORMAL,       // Standard room with possible encounters
    TREASURE,     // Contains valuable loot
    BOSS,         // Boss enemy encounter
    TRAP,         // Dangerous trap room
    PUZZLE,       // Puzzle to solve for rewards
    SECRET,       // Hidden room with rare items
    SAFE,         // Rest area, no enemies
    SHOP,         // Merchant/vendor room
    ENTRANCE,     // Starting room
    EXIT,         // Final destination
    LIBRARY,      // Library with rare knowledge items
    FORGE,        // Forge for crafting weapons/armor
    SHRINE,       // Shrine with blessing/buff effects
    LABORATORY,   // Laboratory with alchemical items
    GRAVEYARD,    // Graveyard with undead enemies
    ARMORY,       // Armory with weapon/armor loot
    PRISON,       // Prison with hostile NPCs
    SHADOWLORD    // Unique final boss chamber
};

/**
// Types of events that can occur
 */
enum class EventType {
    COMBAT,       // Enemy encounter
    TREASURE,     // Find loot
    TRAP,         // Take damage or debuff
    RIDDLE,       // Solve a riddle
    MERCHANT,     // Buy/sell items
    HEAL,         // Restore HP
    BUFF,         // Temporary stat boost
    AMBUSH,       // Surprise attack
    STORY,        // Narrative event
    CHOICE        // Branching decision
};

/**
// Telegraphable intent for enemies (for fairness/readability)
 */
enum class EnemyIntent {
    UNKNOWN,
    ATTACK_LIGHT,
    ATTACK_HEAVY,
    DEFEND,
    DEBUFF,
    BUFF,
    HEAL,
    CONTROL,
    SUMMON,
    CHARGE
};

/**
// Quest status tracking
 */
enum class QuestStatus {
    NOT_STARTED,
    IN_PROGRESS,
    COMPLETED,
    FAILED
};

/**
// Character classes with different starting stats
 */
enum class CharacterClass {
    WARRIOR,      // High HP, high attack
    MAGE,         // Low HP, high magic damage
    ROGUE,        // Medium HP, high crit/dodge
    PALADIN,      // Balanced with healing
    RANGER,       // High speed, high crit, ranged focus
    NECROMANCER,  // Low HP, high magic, life drain
    BARBARIAN,    // Very high HP and attack, low defense
    MONK          // High dodge, high speed, balanced stats
};

/**
// Elemental damage types
 */
enum class DamageElement {
    PHYSICAL,     // Physical damage
    FIRE,         // Fire magic
    FROST,        // Ice/Frost magic
    LIGHTNING,    // Lightning magic
    ARCANE,       // Pure magical energy
    HOLY,         // Holy/divine magic
    SHADOW,       // Dark/shadow magic
    NATURE        // Nature/primal magic
};

/**
// Possible game endings
 */
enum class EndingType {
    VICTORY_NORMAL,      // Defeated final boss
    VICTORY_SECRET,      // Found secret ending
    VICTORY_SPEEDRUN,    // Won in under X turns
    DEFEAT_DEATH,        // Died in combat
    DEFEAT_TRAP,         // Killed by trap
    ENDING_EVIL,         // Made evil choices
    ENDING_HERO,         // Made heroic choices
    ENDING_NEUTRAL,      // Mixed choices
    ENDING_PACIFIST,     // Avoided combat
    ENDING_COMPLETIONIST // Completed all quests
};

// ---
// STAT STRUCTURES
// ---

/**
// Combat statistics for players and enemies
 */
struct CombatStats {
    int maxHP;           // Maximum hit points
    int currentHP;       // Current hit points
    int maxMana;         // Maximum mana points
    int currentMana;     // Current mana points
    int attack;          // Base attack damage
    int defense;         // Damage reduction
    int critChance;      // Percentage chance for critical hit (0-100)
    int dodgeChance;     // Percentage chance to dodge (0-100)
    int speed;           // Determines turn order
    int magicPower;      // Magic damage multiplier
    int magicResist;     // Magic damage reduction
    
    CombatStats() : maxHP(100), currentHP(100), maxMana(50), currentMana(50),
                    attack(10), defense(5), critChance(10), dodgeChance(5), 
                    speed(10), magicPower(10), magicResist(5) {}
};

/**
// Temporary buff/debuff effect
 */
struct StatusEffect {
    std::string name;
    std::string description;
    int attackMod;       // Modifier to attack
    int defenseMod;      // Modifier to defense
    int critMod;         // Modifier to crit chance
    int dodgeMod;        // Modifier to dodge chance
    int speedMod;        // Modifier to speed
    int turnsRemaining;  // How many turns until effect expires
    bool isDebuff;       // True if negative effect

    // Extended fields for richer combat interactions
    int dotDamage;       // Damage over time per tick
    bool dotIsMagic;     // If true, use magic scaling/resist
    bool appliesVulnerable; // If true, target takes extra damage
    int vulnerablePercent;  // Percent extra damage taken
    bool appliesStun;    // Stuns target for 1 turn
    bool appliesSilence; // Prevents abilities
    int shieldAmount;    // Temporary shield HP
    int stackCount;      // Current stacks of this effect
    int maxStacks;       // Cap for stacking
    bool ticksStartOfTurn; // If true, tick at start; else end
    
    StatusEffect() : attackMod(0), defenseMod(0), critMod(0), dodgeMod(0),
                     speedMod(0), turnsRemaining(0), isDebuff(false), dotDamage(0),
                     dotIsMagic(false), appliesVulnerable(false), vulnerablePercent(0),
                     appliesStun(false), appliesSilence(false), shieldAmount(0),
                     stackCount(0), maxStacks(1), ticksStartOfTurn(true) {}
};

// ---
// ITEM STRUCTURE
// ---

/**
// Represents an item in the game world
 */
struct Item {
    int id;                      // Unique identifier
    std::string name;            // Display name
    std::string description;     // Detailed description
    ItemType type;               // Category of item
    ItemRarity rarity;           // Rarity tier
    int value;                   // Gold value for buying/selling
    int quantity;                // Stack count
    
    // Effect values (interpretation depends on type)
    int healAmount;              // HP restored (consumables)
    int attackBonus;             // Attack modifier (weapons)
    int defenseBonus;            // Defense modifier (armor)
    int critBonus;               // Crit chance modifier
    int dodgeBonus;              // Dodge chance modifier
    int speedBonus;              // Speed modifier
    int duration;                // Effect duration in turns
    
    std::string questId;         // Associated quest ID (for quest items)
    bool isEquipped;             // Currently equipped?
    bool isUsable;               // Can be used from inventory?
    bool isStackable;            // Can stack with same items?
    
    // Durability and upgrade system
    int durability;              // Current durability points
    int maxDurability;           // Maximum durability points
    int upgradeLevel;            // Current upgrade level (0-10)

    // Replayability progression
    int itemLevel;               // Dynamic item level (iLvl)
    bool hasUniquePerk;          // High-tier roll with special bonus
    std::string uniquePerkName;  // Display name for rolled perk
    bool mawOnly;                // Only drops/spawns in The Maw
    
    // Enchantment system
    bool isEnchanted;            // Has enchantment applied?
    std::string enchantmentName; // Name of the enchantment
    
    Item() : id(0), type(ItemType::CONSUMABLE), rarity(ItemRarity::COMMON),
             value(0), quantity(1), healAmount(0), attackBonus(0),
             defenseBonus(0), critBonus(0), dodgeBonus(0), speedBonus(0),
             duration(0), isEquipped(false), isUsable(true), isStackable(true),
             durability(100), maxDurability(100), upgradeLevel(0),
             itemLevel(1), hasUniquePerk(false), mawOnly(false),
             isEnchanted(false) {}
};

// ---
// ENEMY STRUCTURE
// ---

/**
// Special ability for enemies
 */
struct EnemyAbility {
    std::string name;
    std::string description;
    int damage;              // Extra damage dealt
    int cooldown;            // Turns between uses
    int currentCooldown;     // Current cooldown counter
    float triggerChance;     // Chance to use (0.0 - 1.0)
    bool heals;              // Does this ability heal the enemy?
    int healAmount;          // HP restored if heals
    
    EnemyAbility() : damage(0), cooldown(3), currentCooldown(0),
                     triggerChance(0.25f), heals(false), healAmount(0) {}
};

/**
// Represents an enemy in the game
 */
struct Enemy {
    int id;                           // Unique identifier
    std::string name;                 // Display name
    std::string description;          // Flavor text
    std::string asciiArt;             // ASCII art representation
    CombatStats stats;                // Combat statistics
    std::vector<EnemyAbility> abilities; // Special abilities
    int experienceReward;             // XP given on defeat
    int goldReward;                   // Gold given on defeat
    std::vector<int> lootTable;       // IDs of possible item drops
    std::vector<int> lootChances;     // Drop chances (percentages)
    bool isBoss;                      // Is this a boss enemy?
    bool mawOnly;                     // Only appears in The Maw
    int trueDamage;                   // Unmitigable damage dealt per attack
    std::string dialogue;             // What the enemy says
    std::vector<StatusEffect> activeEffects; // Active status effects

    // Intent/AI fields
    EnemyIntent nextIntent;           // Next planned action type
    int intentStrength;               // Relative strength/weight
    
    Enemy() : id(0), experienceReward(10), goldReward(5), isBoss(false),
              mawOnly(false), trueDamage(0),
              nextIntent(EnemyIntent::UNKNOWN), intentStrength(0) {}
};

// ---
// ROOM STRUCTURE
// ---

/**
// Represents a room/location in the dungeon
 */
struct Room {
    int id;                          // Unique identifier
    int x, y;                        // Grid position
    std::string name;                // Room name
    std::string description;         // Detailed description
    std::string shortDesc;           // Brief description for revisits
    RoomType type;                   // Room category
    
    // Navigation
    bool hasNorth, hasSouth, hasEast, hasWest;  // Available exits
    int northRoomId, southRoomId, eastRoomId, westRoomId;  // Connected rooms
    bool isLocked;                   // Requires key to enter?
    int requiredKeyId;               // ID of key item needed
    
    // Contents
    std::vector<Item> items;         // Items in the room
    std::vector<Enemy> enemies;      // Enemies in the room
    std::vector<int> eventIds;       // Events that can trigger here
    
    // State
    bool isExplored;                 // Has player visited?
    bool isCleared;                  // Enemies defeated?
    bool isSecret;                   // Is this a secret room?
    bool isTrapActive;               // Is trap still active?
    int trapDamage;                  // Damage dealt by trap
    
    // Puzzle (if applicable)
    std::string puzzleQuestion;
    std::string puzzleAnswer;
    bool puzzleSolved;
    int puzzleRewardItemId;
    
    Room() : id(0), x(0), y(0), type(RoomType::NORMAL),
             hasNorth(false), hasSouth(false), hasEast(false), hasWest(false),
             northRoomId(-1), southRoomId(-1), eastRoomId(-1), westRoomId(-1),
             isLocked(false), requiredKeyId(-1), isExplored(false),
             isCleared(false), isSecret(false), isTrapActive(false),
             trapDamage(0), puzzleSolved(false), puzzleRewardItemId(-1) {}
};

// ---
// QUEST STRUCTURE
// ---

/**
// Represents a quest/mission
 */
struct Quest {
    int id;                          // Unique identifier
    std::string name;                // Quest name
    std::string description;         // Quest description
    std::string completionText;      // Text shown on completion
    QuestStatus status;              // Current status
    
    // Requirements
    int targetEnemyId;               // Enemy to defeat (if combat quest)
    int targetEnemyCount;            // Number to defeat
    int currentEnemyCount;           // Progress counter
    int targetItemId;                // Item to collect (if fetch quest)
    int targetRoomId;                // Room to reach (if exploration quest)
    bool requiresBoss;               // Must defeat a boss?
    
    // Rewards
    int goldReward;
    int experienceReward;
    std::vector<int> itemRewards;    // IDs of reward items
    
    // Flags
    bool isMainQuest;                // Part of main storyline?
    bool affectsMorality;            // Affects good/evil score?
    int moralityChange;              // +/- to morality score
    
    Quest() : id(0), status(QuestStatus::NOT_STARTED),
              targetEnemyId(-1), targetEnemyCount(0), currentEnemyCount(0),
              targetItemId(-1), targetRoomId(-1), requiresBoss(false),
              goldReward(0), experienceReward(0), isMainQuest(false),
              affectsMorality(false), moralityChange(0) {}
};

// ---
// EVENT STRUCTURE
// ---

/**
// Choice option for decision events
 */
struct EventChoice {
    std::string text;                // Choice display text
    std::string resultText;          // Text shown after choosing
    int hpChange;                    // HP modification
    int goldChange;                  // Gold modification
    int moralityChange;              // Morality modification
    int itemRewardId;                // Item given (-1 for none)
    int itemLostId;                  // Item taken (-1 for none)
    bool triggersEvent;              // Does this trigger another event?
    int triggeredEventId;            // ID of triggered event
    
    EventChoice() : hpChange(0), goldChange(0), moralityChange(0),
                    itemRewardId(-1), itemLostId(-1), triggersEvent(false),
                    triggeredEventId(-1) {}
};

/**
// Represents a game event (encounter, trap, choice, etc.)
 */
struct Event {
    int id;                          // Unique identifier
    std::string name;                // Event name
    std::string description;         // What happens
    EventType type;                  // Event category
    
    // Trigger conditions
    int triggerChance;               // Percentage chance to occur (0-100)
    bool requiresItem;               // Needs specific item to trigger?
    int requiredItemId;              // ID of required item
    bool oneTimeOnly;                // Can only occur once?
    bool hasOccurred;                // Has this already happened?
    
    // Effects
    int damageDealt;                 // Damage to player (traps)
    int healingGiven;                // HP restored
    int goldGiven;                   // Gold rewarded
    int goldTaken;                   // Gold removed
    int experienceGiven;             // XP rewarded
    std::vector<int> itemsGiven;     // Items rewarded
    std::vector<StatusEffect> buffs; // Buffs applied
    
    // Combat events
    int enemyId;                     // Enemy spawned (-1 for none)
    
    // Choice events
    std::vector<EventChoice> choices; // Available choices
    
    // Riddle events
    std::string riddleQuestion;
    std::string riddleAnswer;
    int riddleRewardId;
    int riddleFailDamage;
    
    Event() : id(0), type(EventType::STORY), triggerChance(100),
              requiresItem(false), requiredItemId(-1), oneTimeOnly(false),
              hasOccurred(false), damageDealt(0), healingGiven(0),
              goldGiven(0), goldTaken(0), experienceGiven(0), enemyId(-1),
              riddleRewardId(-1), riddleFailDamage(0) {}
};

// ---
// SKILL STRUCTURE
// ---

/**
// Represents a player skill/ability
 */
struct Skill {
    std::string name;
    std::string description;
    int manaCost;
    int cooldown;
    int currentCooldown;
    float damageMultiplier;
    bool isMagical;
    bool isHeal;
    StatusEffect effect;
    
    Skill() : manaCost(0), cooldown(0), currentCooldown(0), 
              damageMultiplier(1.0f), isMagical(false), isHeal(false) {}
};

/**
// Magic school categories for spells
 */
enum class MagicSchool {
    EVOCATION,      // Damage spells (fire, ice, lightning)
    ABJURATION,     // Defense/protection spells
    TRANSMUTATION,  // Buff/debuff spells
    NECROMANCY,     // Life drain, curses, undead summon
    CONJURATION,    // Summons, temporary effects
    DIVINATION,     // Utility, reveal hidden
    RESTORATION,    // Healing, purge status effects
    ENCHANTMENT     // Mind control, manipulation
};

/**
// Spell definition (magic ability for certain classes)
 */
struct Spell {
    int id;                          // Unique spell ID
    std::string name;                // Spell name
    std::string description;         // What does it do
    MagicSchool school;              // School of magic
    int manaCost;                    // Mana required
    int cooldown;                    // Turn cooldown
    int currentCooldown;             // Current cooldown
    int level;                       // Minimum level to learn
    float damageMultiplier;          // For offensive spells: damage * (magic_power * multiplier)
    float healMultiplier;            // For healing spells: heal * (magic_power * multiplier)
    int dotDamage;                   // Damage over time per turn
    int dotTurns;                    // How many turns DoT lasts
    bool isAOE;                      // Affects all enemies?
    bool isHeal;                     // Is healing spell?
    bool isDebuff;                   // Is debuff spell?
    StatusEffect appliedEffect;      // Effect to apply (if any)
    std::vector<CharacterClass> restrictedClasses;  // Classes that can learn this spell
    
    Spell() : id(0), school(MagicSchool::EVOCATION), manaCost(0), cooldown(0), 
              currentCooldown(0), level(1), damageMultiplier(1.0f), healMultiplier(1.0f), 
              dotDamage(0), dotTurns(0), isAOE(false), isHeal(false), isDebuff(false) {}
};

// ---
// PLAYER STRUCTURE
// ---

/**
// Represents the player character
 */
struct Player {
    std::string name;                // Character name
    CharacterClass charClass;        // Character class
    CombatStats stats;               // Combat statistics
    int level;                       // Current level
    int experience;                  // Current XP
    int experienceToLevel;           // XP needed for next level
    int gold;                        // Currency
    
    // Skills
    std::vector<Skill> skills;       // Available skills
    
    // Magic System (restricted to certain classes)
    // Note: Spells are stored dynamically; use caution with empty vector
    std::vector<Spell> learnedSpells;  // Known spells
    int spellsLearned;               // Total spells learned
    std::vector<struct Enemy> currentCombatEnemies;  // Enemies in current combat (for AOE spells)
    
    // Inventory
    std::vector<Item> inventory;     // All carried items
    int maxInventorySize;            // Maximum items (0 = unlimited)
    
    // Equipment slots
    int equippedWeaponId;            // ID of equipped weapon (-1 = none)
    int equippedArmorId;             // ID of equipped armor
    int equippedAccessoryId;         // ID of equipped accessory
    int averageItemLevel;            // Average iLvl of equipped gear
    
    // Status
    std::vector<StatusEffect> activeEffects;  // Active buffs/debuffs
    std::vector<Quest> activeQuests;          // Current quests
    std::vector<Quest> completedQuests;       // Finished quests
    std::vector<int> visitedRooms;            // IDs of visited rooms
    std::vector<std::string> gameLog;         // Log of events
    
    // Position
    int currentRoomId;               // Current room ID
    
    // Stats for endings
    int moralityScore;               // Good (+) / Evil (-)
    int totalKills;                  // Enemies defeated
    int explorationTurns;            // Turns spent exploring
    int combatTurns;                 // Turns spent in combat
    int totalTurns;                  // Derived: exploration + combat
    int secretsFound;                // Secret rooms discovered
    int puzzlesSolved;               // Puzzles completed
    int bossesDefeated;              // Boss enemies killed
    int deathCount;                  // Times died (for saves)
    
    // Flags
    bool hasWon;                     // Reached victory condition?
    bool isAlive;                    // Still alive?
    bool hasSecretEnding;            // Found secret ending?
    bool shadowLordDefeated;         // True after defeating final boss
    bool mawUnlocked;                // Portal Room / The Maw access unlocked
    int mawDepthReached;             // Deepest cleared Maw floor
    
    Player() : charClass(CharacterClass::WARRIOR), level(1), experience(0),
               experienceToLevel(100), gold(50), maxInventorySize(30),
               equippedWeaponId(-1), equippedArmorId(-1), equippedAccessoryId(-1),
               averageItemLevel(0),
               currentRoomId(0), moralityScore(0), totalKills(0), explorationTurns(0),
               combatTurns(0), totalTurns(0), secretsFound(0), puzzlesSolved(0),
               bossesDefeated(0), deathCount(0), hasWon(false), isAlive(true),
               hasSecretEnding(false), shadowLordDefeated(false), mawUnlocked(false),
               mawDepthReached(0) {}
};

// ---
// GAME STATE STRUCTURE
// ---

/**
// Main game state container
 */
struct GameState {
    Player player;                           // Player data
    std::vector<Room> dungeon;               // All rooms
    std::vector<Item> itemTemplates;         // Item database
    std::vector<Enemy> enemyTemplates;       // Enemy database
    std::vector<Event> eventTemplates;       // Event database
    std::vector<Quest> questTemplates;       // Quest database
    std::map<int, Room*> roomMap;            // Quick room lookup by ID
    
    // World generation params
    int mapWidth;
    int mapHeight;
    int currentDungeonLevel;
    unsigned int randomSeed;
    bool isInMawRealm;
    int currentMawStratum;
    
    // Game settings
    bool debugMode;
    bool showAsciiArt;
    int difficulty;                          // 1=Easy, 2=Normal, 3=Hard
    
    // Leaderboard
    std::vector<std::pair<std::string, int>> leaderboard;  // Name, Score
    
    GameState() : mapWidth(8), mapHeight(8), currentDungeonLevel(1),
                  randomSeed(0), isInMawRealm(false), currentMawStratum(0),
                  debugMode(false), showAsciiArt(true), difficulty(2) {}
};

// ---
// RIDDLE STRUCTURE
// ---

/**
// Represents a riddle/puzzle
 */
struct Riddle {
    std::string question;
    std::string answer;
    std::string hint;
    
    Riddle() {}
    Riddle(const std::string& q, const std::string& a, const std::string& h = "")
        : question(q), answer(a), hint(h) {}
};

}  // namespace AQE

#endif // TYPES_H
