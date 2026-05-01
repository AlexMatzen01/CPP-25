





#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace AQE {  






enum class ItemRarity {
    COMMON,      
    UNCOMMON,    
    RARE,        
    EPIC,        
    LEGENDARY    
};


enum class ItemType {
    CONSUMABLE,   
    WEAPON,       
    ARMOR,        
    ACCESSORY,    
    KEY,          
    QUEST,        
    SPECIAL       
};


enum class RoomType {
    NORMAL,       
    TREASURE,     
    BOSS,         
    TRAP,         
    PUZZLE,       
    SECRET,       
    SAFE,         
    SHOP,         
    ENTRANCE,     
    EXIT,         
    LIBRARY,      
    FORGE,        
    SHRINE,       
    LABORATORY,   
    GRAVEYARD,    
    ARMORY,       
    PRISON,       
    SHADOWLORD    
};


enum class EventType {
    COMBAT,       
    TREASURE,     
    TRAP,         
    RIDDLE,       
    MERCHANT,     
    HEAL,         
    BUFF,         
    AMBUSH,       
    STORY,        
    CHOICE        
};


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


enum class QuestStatus {
    NOT_STARTED,
    IN_PROGRESS,
    COMPLETED,
    FAILED
};


enum class CharacterClass {
    WARRIOR,      
    MAGE,         
    ROGUE,        
    PALADIN,      
    RANGER,       
    NECROMANCER,  
    BARBARIAN,    
    MONK          
};


enum class DamageElement {
    PHYSICAL,     
    FIRE,         
    FROST,        
    LIGHTNING,    
    ARCANE,       
    HOLY,         
    SHADOW,       
    NATURE        
};


enum class EndingType {
    VICTORY_NORMAL,      
    VICTORY_SECRET,      
    VICTORY_SPEEDRUN,    
    DEFEAT_DEATH,        
    DEFEAT_TRAP,         
    ENDING_EVIL,         
    ENDING_HERO,         
    ENDING_NEUTRAL,      
    ENDING_PACIFIST,     
    ENDING_COMPLETIONIST 
};






struct CombatStats {
    int maxHP;           
    int currentHP;       
    int maxMana;         
    int currentMana;     
    int attack;          
    int defense;         
    int critChance;      
    int dodgeChance;     
    int speed;           
    int magicPower;      
    int magicResist;     
    
    CombatStats() : maxHP(100), currentHP(100), maxMana(50), currentMana(50),
                    attack(10), defense(5), critChance(10), dodgeChance(5), 
                    speed(10), magicPower(10), magicResist(5) {}
};


struct StatusEffect {
    std::string name;
    std::string description;
    int attackMod;       
    int defenseMod;      
    int critMod;         
    int dodgeMod;        
    int speedMod;        
    int turnsRemaining;  
    bool isDebuff;       

    
    int dotDamage;       
    bool dotIsMagic;     
    bool appliesVulnerable; 
    int vulnerablePercent;  
    bool appliesStun;    
    bool appliesSilence; 
    int shieldAmount;    
    int stackCount;      
    int maxStacks;       
    bool ticksStartOfTurn; 
    
    StatusEffect() : attackMod(0), defenseMod(0), critMod(0), dodgeMod(0),
                     speedMod(0), turnsRemaining(0), isDebuff(false), dotDamage(0),
                     dotIsMagic(false), appliesVulnerable(false), vulnerablePercent(0),
                     appliesStun(false), appliesSilence(false), shieldAmount(0),
                     stackCount(0), maxStacks(1), ticksStartOfTurn(true) {}
};






struct Item {
    int id;                      
    std::string name;            
    std::string description;     
    ItemType type;               
    ItemRarity rarity;           
    int value;                   
    int quantity;                
    
    
    int healAmount;              
    int attackBonus;             
    int defenseBonus;            
    int critBonus;               
    int dodgeBonus;              
    int speedBonus;              
    int duration;                
    
    std::string questId;         
    bool isEquipped;             
    bool isUsable;               
    bool isStackable;            
    
    
    int durability;              
    int maxDurability;           
    int upgradeLevel;            

    
    int itemLevel;               
    bool hasUniquePerk;          
    std::string uniquePerkName;  
    bool mawOnly;                
    
    
    bool isEnchanted;            
    std::string enchantmentName; 
    
    Item() : id(0), type(ItemType::CONSUMABLE), rarity(ItemRarity::COMMON),
             value(0), quantity(1), healAmount(0), attackBonus(0),
             defenseBonus(0), critBonus(0), dodgeBonus(0), speedBonus(0),
             duration(0), isEquipped(false), isUsable(true), isStackable(true),
             durability(100), maxDurability(100), upgradeLevel(0),
             itemLevel(1), hasUniquePerk(false), mawOnly(false),
             isEnchanted(false) {}
};






struct EnemyAbility {
    std::string name;
    std::string description;
    int damage;              
    int cooldown;            
    int currentCooldown;     
    float triggerChance;     
    bool heals;              
    int healAmount;          
    
    EnemyAbility() : damage(0), cooldown(3), currentCooldown(0),
                     triggerChance(0.25f), heals(false), healAmount(0) {}
};


struct Enemy {
    int id;                           
    std::string name;                 
    std::string description;          
    std::string asciiArt;             
    CombatStats stats;                
    std::vector<EnemyAbility> abilities; 
    int experienceReward;             
    int goldReward;                   
    std::vector<int> lootTable;       
    std::vector<int> lootChances;     
    bool isBoss;                      
    bool mawOnly;                     
    int trueDamage;                   
    std::string dialogue;             
    std::vector<StatusEffect> activeEffects; 

    
    EnemyIntent nextIntent;           
    int intentStrength;               
    
    Enemy() : id(0), experienceReward(10), goldReward(5), isBoss(false),
              mawOnly(false), trueDamage(0),
              nextIntent(EnemyIntent::UNKNOWN), intentStrength(0) {}
};






struct Room {
    int id;                          
    int x, y;                        
    std::string name;                
    std::string description;         
    std::string shortDesc;           
    RoomType type;                   
    
    
    bool hasNorth, hasSouth, hasEast, hasWest;  
    int northRoomId, southRoomId, eastRoomId, westRoomId;  
    bool isLocked;                   
    int requiredKeyId;               
    
    
    std::vector<Item> items;         
    std::vector<Enemy> enemies;      
    std::vector<int> eventIds;       
    
    
    bool isExplored;                 
    bool isCleared;                  
    bool isSecret;                   
    bool isTrapActive;               
    int trapDamage;                  
    
    
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






struct Quest {
    int id;                          
    std::string name;                
    std::string description;         
    std::string completionText;      
    QuestStatus status;              
    
    
    int targetEnemyId;               
    int targetEnemyCount;            
    int currentEnemyCount;           
    int targetItemId;                
    int targetRoomId;                
    bool requiresBoss;               
    
    
    int goldReward;
    int experienceReward;
    std::vector<int> itemRewards;    
    
    
    bool isMainQuest;                
    bool affectsMorality;            
    int moralityChange;              
    
    Quest() : id(0), status(QuestStatus::NOT_STARTED),
              targetEnemyId(-1), targetEnemyCount(0), currentEnemyCount(0),
              targetItemId(-1), targetRoomId(-1), requiresBoss(false),
              goldReward(0), experienceReward(0), isMainQuest(false),
              affectsMorality(false), moralityChange(0) {}
};






struct EventChoice {
    std::string text;                
    std::string resultText;          
    int hpChange;                    
    int goldChange;                  
    int moralityChange;              
    int itemRewardId;                
    int itemLostId;                  
    bool triggersEvent;              
    int triggeredEventId;            
    
    EventChoice() : hpChange(0), goldChange(0), moralityChange(0),
                    itemRewardId(-1), itemLostId(-1), triggersEvent(false),
                    triggeredEventId(-1) {}
};


struct Event {
    int id;                          
    std::string name;                
    std::string description;         
    EventType type;                  
    
    
    int triggerChance;               
    bool requiresItem;               
    int requiredItemId;              
    bool oneTimeOnly;                
    bool hasOccurred;                
    
    
    int damageDealt;                 
    int healingGiven;                
    int goldGiven;                   
    int goldTaken;                   
    int experienceGiven;             
    std::vector<int> itemsGiven;     
    std::vector<StatusEffect> buffs; 
    
    
    int enemyId;                     
    
    
    std::vector<EventChoice> choices; 
    
    
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


enum class MagicSchool {
    EVOCATION,      
    ABJURATION,     
    TRANSMUTATION,  
    NECROMANCY,     
    CONJURATION,    
    DIVINATION,     
    RESTORATION,    
    ENCHANTMENT     
};


struct Spell {
    int id;                          
    std::string name;                
    std::string description;         
    MagicSchool school;              
    int manaCost;                    
    int cooldown;                    
    int currentCooldown;             
    int level;                       
    float damageMultiplier;          
    float healMultiplier;            
    int dotDamage;                   
    int dotTurns;                    
    bool isAOE;                      
    bool isHeal;                     
    bool isDebuff;                   
    StatusEffect appliedEffect;      
    std::vector<CharacterClass> restrictedClasses;  
    
    Spell() : id(0), school(MagicSchool::EVOCATION), manaCost(0), cooldown(0), 
              currentCooldown(0), level(1), damageMultiplier(1.0f), healMultiplier(1.0f), 
              dotDamage(0), dotTurns(0), isAOE(false), isHeal(false), isDebuff(false) {}
};






struct Player {
    std::string name;                
    CharacterClass charClass;        
    CombatStats stats;               
    int level;                       
    int experience;                  
    int experienceToLevel;           
    int gold;                        
    
    
    std::vector<Skill> skills;       
    
    
    
    std::vector<Spell> learnedSpells;  
    int spellsLearned;               
    std::vector<struct Enemy> currentCombatEnemies;  
    
    
    std::vector<Item> inventory;     
    int maxInventorySize;            
    
    
    int equippedWeaponId;            
    int equippedArmorId;             
    int equippedAccessoryId;         
    int averageItemLevel;            
    
    
    std::vector<StatusEffect> activeEffects;  
    std::vector<Quest> activeQuests;          
    std::vector<Quest> completedQuests;       
    std::vector<int> visitedRooms;            
    std::vector<std::string> gameLog;         
    
    
    int currentRoomId;               
    
    
    int moralityScore;               
    int totalKills;                  
    int explorationTurns;            
    int combatTurns;                 
    int totalTurns;                  
    int secretsFound;                
    int puzzlesSolved;               
    int bossesDefeated;              
    int deathCount;                  
    
    
    bool hasWon;                     
    bool isAlive;                    
    bool hasSecretEnding;            
    bool shadowLordDefeated;         
    bool mawUnlocked;                
    int mawDepthReached;             
    
    
    std::vector<int> unlockedTalentIds;     
    int selectedSpecialization;             
    int talentPoints;                       
    int spentTalentPoints;                  
    
    Player() : charClass(CharacterClass::WARRIOR), level(1), experience(0),
               experienceToLevel(100), gold(50), maxInventorySize(30),
               equippedWeaponId(-1), equippedArmorId(-1), equippedAccessoryId(-1),
               averageItemLevel(0),
               currentRoomId(0), moralityScore(0), totalKills(0), explorationTurns(0),
               combatTurns(0), totalTurns(0), secretsFound(0), puzzlesSolved(0),
               bossesDefeated(0), deathCount(0), hasWon(false), isAlive(true),
               hasSecretEnding(false), shadowLordDefeated(false), mawUnlocked(false),
               mawDepthReached(0), selectedSpecialization(0), talentPoints(1), spentTalentPoints(0) {}
};






struct GameState {
    Player player;                           
    std::vector<Room> dungeon;               
    std::vector<Item> itemTemplates;         
    std::vector<Enemy> enemyTemplates;       
    std::vector<Event> eventTemplates;       
    std::vector<Quest> questTemplates;       
    std::map<int, Room*> roomMap;            
    
    
    int mapWidth;
    int mapHeight;
    int currentDungeonLevel;
    unsigned int randomSeed;
    bool isInMawRealm;
    int currentMawStratum;
    
    
    bool debugMode;
    bool showAsciiArt;
    int difficulty;                          
    
    
    std::vector<std::pair<std::string, int>> leaderboard;  
    
    GameState() : mapWidth(8), mapHeight(8), currentDungeonLevel(1),
                  randomSeed(0), isInMawRealm(false), currentMawStratum(0),
                  debugMode(false), showAsciiArt(true), difficulty(2) {}
};






struct Riddle {
    std::string question;
    std::string answer;
    std::string hint;
    
    Riddle() {}
    Riddle(const std::string& q, const std::string& a, const std::string& h = "")
        : question(q), answer(a), hint(h) {}
};

}  

#endif 
