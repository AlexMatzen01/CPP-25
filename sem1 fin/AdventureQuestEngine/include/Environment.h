/*******************************************************************************
 * C++ Adventure Quest Engine - ENHANCED EDITION
 * Environment.h - Dynamic Environment and Biome System
 * 
 * Features:
 * - Multiple biomes with unique characteristics
 * - Environmental hazards and interactive terrain
 * - Weather system affecting combat and exploration
 * - Dynamic events based on location
 * - Landmarks and points of interest
 * - Day/night cycle (if applicable)
 * 
 * Author: Adventure Quest Engine Team
 * Version: 2.0 - MASSIVE OVERHAUL
 ******************************************************************************/

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "Types.h"
#include "Combat.h"
#include "Crafting.h"
#include <vector>
#include <string>
#include <map>

namespace AQE {

//==============================================================================
// BIOME SYSTEM
//==============================================================================

enum class BiomeType {
    DUNGEON_DEPTHS,      // Classic dungeon
    VOLCANIC_CAVERN,     // Fire hazards
    ICY_HALLS,           // Ice/slow effects
    CORRUPTED_CRYPT,     // Undead, poison
    CRYSTAL_MINES,       // Valuable resources
    ANCIENT_RUINS,       // Puzzles, lore
    SHADOW_REALM,        // Dark magic
    ENCHANTED_FOREST,    // Nature, healing
    UNDERWATER_GROTTO,   // Water hazards
    SKY_CITADEL          // Wind/lightning
};

struct BiomeProperties {
    BiomeType type;
    std::string name;
    std::string description;
    std::vector<std::string> flavorTexts;    // Random descriptive text
    
    // Environmental effects
    int passiveDamagePerTurn;                // DoT in hazardous biomes
    DamageElement damageElement;             // Type of damage
    float movementSpeedModifier;             // 0.5 = half speed
    float visibilityRange;                   // How far you can "see"
    bool hasNaturalHealing;                  // Heal each turn
    int healingPerTurn;
    
    // Combat modifiers
    float playerDamageModifier;              // 1.1 = +10% damage
    float enemyDamageModifier;
    float critChanceModifier;
    float dodgeChanceModifier;
    DamageElement boostedElement;            // Fire does +50% in volcanic
    float elementalBoost;
    
    // Resource availability
    std::map<MaterialType, float> materialSpawnChance;
    std::vector<int> uniqueEnemyIds;         // Biome-specific enemies
    std::vector<int> uniqueItemIds;          // Biome-specific loot
    
    // Hazards
    bool hasLavaPools;
    bool hasIcePatches;                      // Chance to slip
    bool hasPoisonGas;
    bool hasElectricArcs;
    bool hasPitfalls;
    int hazardDamage;
    
    BiomeProperties() : type(BiomeType::DUNGEON_DEPTHS), passiveDamagePerTurn(0),
                       damageElement(DamageElement::PHYSICAL), movementSpeedModifier(1.0f),
                       visibilityRange(1.0f), hasNaturalHealing(false), healingPerTurn(0),
                       playerDamageModifier(1.0f), enemyDamageModifier(1.0f),
                       critChanceModifier(1.0f), dodgeChanceModifier(1.0f),
                       boostedElement(DamageElement::PHYSICAL), elementalBoost(1.0f),
                       hasLavaPools(false), hasIcePatches(false), hasPoisonGas(false),
                       hasElectricArcs(false), hasPitfalls(false), hazardDamage(0) {}
};

//==============================================================================
// WEATHER SYSTEM
//==============================================================================

enum class WeatherType {
    NONE,
    FOG,                 // Reduced accuracy
    STORM,               // Lightning damage chance
    BLIZZARD,            // Ice damage, slow
    HEAT_WAVE,           // Fire damage buff
    RAIN,                // Water/lightning boost
    DARKNESS,            // Reduced vision
    BLOOD_MOON,          // Enemies stronger
    AURORA               // Magic boost
};

struct WeatherEffect {
    WeatherType type;
    std::string name;
    std::string description;
    int duration;                            // Turns remaining
    
    // Combat effects
    int accuracyModifier;                    // -20 in fog
    int damageModifier;
    DamageElement associatedElement;
    bool boostedEnemies;
    float enemyStatMultiplier;
    
    // Exploration effects
    bool reducesVisibility;
    bool preventsResting;
    bool dealsDamagePerTurn;
    int damagePerTurn;
    
    WeatherEffect() : type(WeatherType::NONE), duration(0), accuracyModifier(0),
                     damageModifier(0), associatedElement(DamageElement::PHYSICAL),
                     boostedEnemies(false), enemyStatMultiplier(1.0f),
                     reducesVisibility(false), preventsResting(false),
                     dealsDamagePerTurn(false), damagePerTurn(0) {}
};

//==============================================================================
// LANDMARKS AND POINTS OF INTEREST
//==============================================================================

enum class LandmarkType {
    SHRINE,              // Heal/buff
    ALTAR,               // Sacrifice for power
    FOUNTAIN,            // Restore mana
    STATUE,              // Lore/story
    PORTAL,              // Fast travel
    CAMPFIRE,            // Rest and save
    LIBRARY,             // Learn recipes
    BLACKSMITH,          // Repair/upgrade
    WELL,                // Mystery effects
    THRONE               // Boss area marker
};

struct Landmark {
    int id;
    LandmarkType type;
    std::string name;
    std::string description;
    std::string interactionText;
    bool isInteractive;
    bool hasBeenUsed;
    int cooldownTurns;                       // Before can use again
    int currentCooldown;
    
    // Effects
    int healingAmount;
    int manaRestoration;
    StatusEffect grantedBuff;
    int goldCost;                            // Some landmarks charge
    std::vector<int> grantedRecipes;         // Learn crafting
    
    // Requirements
    bool requiresOffering;
    int offeringItemId;
    int offeringGold;
    
    Landmark() : id(0), type(LandmarkType::SHRINE), isInteractive(true),
                hasBeenUsed(false), cooldownTurns(0), currentCooldown(0),
                healingAmount(0), manaRestoration(0), goldCost(0),
                requiresOffering(false), offeringItemId(-1), offeringGold(0) {}
};

//==============================================================================
// INTERACTIVE TERRAIN
//==============================================================================

enum class TerrainFeature {
    WALL,                // Blocks movement
    DOOR,                // Can be opened/locked
    CHEST,               // Contains loot
    BARREL,              // Breakable, minor loot
    LEVER,               // Activates mechanism
    PRESSURE_PLATE,      // Triggers trap/door
    PIT,                 // Can fall in
    LADDER,              // Vertical movement
    BRIDGE,              // Can be destroyed
    BUSH,                // Can hide in
    TORCH,               // Light source
    GRATE                // Drain or vent
};

struct InteractiveTerrain {
    TerrainFeature type;
    bool isActive;
    bool isLocked;
    int requiredKeyId;
    std::vector<Item> containedItems;
    int trapDamage;
    bool triggersEvent;
    int eventId;
    
    InteractiveTerrain() : type(TerrainFeature::WALL), isActive(false),
                          isLocked(false), requiredKeyId(-1), trapDamage(0),
                          triggersEvent(false), eventId(-1) {}
};

//==============================================================================
// ENVIRONMENTAL HAZARDS
//==============================================================================

struct EnvironmentalHazard {
    std::string name;
    std::string description;
    int damage;
    DamageElement element;
    StatusEffect appliedEffect;
    float activationChance;                  // Chance to trigger per turn
    bool isAvoidable;                        // Can dodge?
    int dodgeThreshold;                      // Dodge chance needed
    
    // Visual warning
    std::string warningMessage;
    int warningTurns;                        // Turns before triggering
    
    EnvironmentalHazard() : damage(0), element(DamageElement::PHYSICAL),
                           activationChance(0.0f), isAvoidable(false),
                           dodgeThreshold(50), warningTurns(0) {}
};

//==============================================================================
// ENVIRONMENT MANAGER
//==============================================================================

class EnvironmentManager {
public:
    EnvironmentManager(GameState& gameState);
    
    // Biome management
    void setBiome(int roomId, BiomeType biome);
    BiomeType getCurrentBiome() const;
    void generateBiomeDistribution();        // Assign biomes to regions
    void displayBiomeInfo() const;
    
    // Biome effects
    void applyBiomeEffects(Player& player);
    void processBiomeHazard(Player& player);
    std::string getBiomeFlavorText() const;
    
    // Weather
    void setWeather(WeatherType weather, int duration);
    void updateWeather();                    // Tick down, random changes
    WeatherType getCurrentWeather() const;
    void applyWeatherEffects(Player& player, CombatStats& enemyStats);
    void displayWeatherInfo() const;
    
    // Landmarks
    void addLandmark(int roomId, Landmark landmark);
    bool interactWithLandmark(int landmarkId, Player& player);
    void displayLandmarksInRoom(int roomId) const;
    std::vector<Landmark> getLandmarksInRoom(int roomId) const;
    
    // Hazards
    void checkEnvironmentalHazards(Player& player, std::vector<std::string>& log);
    void spawnHazard(int roomId, const EnvironmentalHazard& hazard);
    void displayActiveHazards() const;
    
    // Interactive terrain
    void addTerrainFeature(int roomId, const InteractiveTerrain& terrain);
    bool interactWithTerrain(int roomId, TerrainFeature type, Player& player);
    std::vector<InteractiveTerrain> getTerrainInRoom(int roomId) const;
    
    // Resource gathering
    std::vector<CraftingMaterial> gatherMaterials(int roomId);
    bool canGatherInBiome(BiomeType biome) const;
    
    // Save/Load
    std::string serializeEnvironment() const;
    void deserializeEnvironment(const std::string& data);
    
private:
    GameState& gameStateRef;
    std::map<int, BiomeType> roomBiomes;             // RoomID -> Biome
    std::map<BiomeType, BiomeProperties> biomeData;
    WeatherEffect currentWeather;
    std::map<int, std::vector<Landmark>> roomLandmarks;
    std::map<int, std::vector<InteractiveTerrain>> roomTerrain;
    std::map<int, std::vector<EnvironmentalHazard>> activeHazards;
    
    void initializeBiomes();
    void generateRandomWeather();
    BiomeProperties& getBiomeProperties(BiomeType biome);
};

//==============================================================================
// ENHANCED MAP GENERATION
//==============================================================================

class BiomeMapGenerator {
public:
    BiomeMapGenerator(int width, int height);
    
    // Generation
    void generateBiomeMap(GameState& state);
    void createBiomeZones(int numZones);
    void placeTransitionRooms();             // Smooth biome transitions
    void placeLandmarks();
    void placeResources();
    
    // Connectivity
    void ensurePathToBoss();
    void createSecretPaths();
    void placeShortcuts();                   // Unlockable fast routes
    
    // Special rooms
    void placeArenaChamber();                // Wave combat
    void placeVaultRoom();                   // High risk, high reward
    void placeLoreRoom();                    // Story elements
    
private:
    int mapWidth;
    int mapHeight;
    std::vector<std::vector<BiomeType>> biomeGrid;
    
    BiomeType selectBiomeForZone(int zoneIndex);
    void floodFillBiome(int x, int y, BiomeType biome, int maxSpread);
};

}  // namespace AQE

#endif // ENVIRONMENT_H
