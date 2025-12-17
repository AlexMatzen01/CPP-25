/*******************************************************************************
 * Environment.cpp - Full implementation for Environment System
 ******************************************************************************/

#include "../include/Environment.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

namespace AQE {

//==============================================================================
// EnvironmentManager Implementation
//==============================================================================

EnvironmentManager::EnvironmentManager(GameState& gameState) : gameStateRef(gameState) {
    initializeBiomes();
}

void EnvironmentManager::setBiome(int roomId, BiomeType biome) {
    roomBiomes[roomId] = biome;
}

BiomeType EnvironmentManager::getCurrentBiome() const {
    int currentRoom = gameStateRef.player.currentRoomId;
    auto it = roomBiomes.find(currentRoom);
    if (it != roomBiomes.end()) {
        return it->second;
    }
    return BiomeType::DUNGEON_DEPTHS;
}

void EnvironmentManager::generateBiomeDistribution() {
    // Assign biomes to rooms in the dungeon
    if (gameStateRef.dungeon.empty()) {
        return;  // No rooms to assign biomes to
    }
    
    int roomCount = static_cast<int>(gameStateRef.dungeon.size());
    
    for (int i = 0; i < roomCount; ++i) {
        int roomId = gameStateRef.dungeon[i].id;
        
        // Distribute biomes based on depth/position
        float depth = static_cast<float>(i) / roomCount;
        
        if (depth < 0.2f) {
            setBiome(roomId, BiomeType::DUNGEON_DEPTHS);
        } else if (depth < 0.35f) {
            setBiome(roomId, static_cast<BiomeType>(rand() % 3));  // Early biomes
        } else if (depth < 0.5f) {
            setBiome(roomId, static_cast<BiomeType>(rand() % 5 + 1));  // Mid biomes
        } else if (depth < 0.75f) {
            setBiome(roomId, static_cast<BiomeType>(rand() % 3 + 5));  // Late biomes
        } else {
            setBiome(roomId, BiomeType::SHADOW_REALM);  // End game
        }
    }
}

void EnvironmentManager::displayBiomeInfo() const {
    BiomeType currentBiome = getCurrentBiome();
    auto it = biomeData.find(currentBiome);
    
    if (it == biomeData.end()) return;
    
    const BiomeProperties& props = it->second;
    
    std::cout << "\n--- " << props.name << " ---\n";
    std::cout << props.description << "\n";
    
    // Show any active effects
    if (props.passiveDamagePerTurn > 0) {
        std::cout << "WARNING: This area deals " << props.passiveDamagePerTurn << " damage per turn!\n";
    }
    
    if (props.hasNaturalHealing) {
        std::cout << "This area naturally heals " << props.healingPerTurn << " HP per turn.\n";
    }
    
    if (props.elementalBoost > 1.0f) {
        std::cout << "Elemental damage is boosted here.\n";
    }
}

void EnvironmentManager::applyBiomeEffects(Player& player) {
    BiomeType currentBiome = getCurrentBiome();
    auto it = biomeData.find(currentBiome);
    
    if (it == biomeData.end()) return;
    
    const BiomeProperties& props = it->second;
    
    // Apply passive damage
    if (props.passiveDamagePerTurn > 0) {
        player.stats.currentHP -= props.passiveDamagePerTurn;
        std::cout << "The environment deals " << props.passiveDamagePerTurn << " damage!\n";
        
        if (player.stats.currentHP <= 0) {
            player.isAlive = false;
        }
    }
    
    // Apply natural healing
    if (props.hasNaturalHealing && props.healingPerTurn > 0) {
        player.stats.currentHP += props.healingPerTurn;
        if (player.stats.currentHP > player.stats.maxHP) {
            player.stats.currentHP = player.stats.maxHP;
        }
        std::cout << "The environment heals you for " << props.healingPerTurn << " HP.\n";
    }
}

void EnvironmentManager::processBiomeHazard(Player& player) {
    BiomeType currentBiome = getCurrentBiome();
    auto it = biomeData.find(currentBiome);
    
    if (it == biomeData.end()) return;
    
    const BiomeProperties& props = it->second;
    
    // Check for environmental hazards
    if (props.hasLavaPools && rand() % 100 < 20) {
        std::cout << "You step near a lava pool! ";
        int damage = props.hazardDamage > 0 ? props.hazardDamage : 15;
        player.stats.currentHP -= damage;
        std::cout << "You take " << damage << " fire damage!\n";
    }
    
    if (props.hasIcePatches && rand() % 100 < 30) {
        std::cout << "You slip on an ice patch! ";
        // Could affect movement or cause stun
        std::cout << "You lose your balance briefly.\n";
    }
    
    if (props.hasPoisonGas && rand() % 100 < 25) {
        std::cout << "Poison gas fills the area! ";
        int damage = props.hazardDamage > 0 ? props.hazardDamage : 10;
        player.stats.currentHP -= damage;
        std::cout << "You take " << damage << " poison damage!\n";
    }
    
    if (props.hasElectricArcs && rand() % 100 < 15) {
        std::cout << "Lightning arcs across the room! ";
        int damage = props.hazardDamage > 0 ? props.hazardDamage : 20;
        player.stats.currentHP -= damage;
        std::cout << "You take " << damage << " lightning damage!\n";
    }
    
    if (props.hasPitfalls && rand() % 100 < 10) {
        std::cout << "Watch out! A pitfall nearly catches you!\n";
    }
    
    if (player.stats.currentHP <= 0) {
        player.isAlive = false;
    }
}

std::string EnvironmentManager::getBiomeFlavorText() const {
    BiomeType currentBiome = getCurrentBiome();
    auto it = biomeData.find(currentBiome);
    
    if (it == biomeData.end() || it->second.flavorTexts.empty()) {
        return "You continue through the dungeon.";
    }
    
    const auto& texts = it->second.flavorTexts;
    return texts[rand() % texts.size()];
}

void EnvironmentManager::setWeather(WeatherType weather, int duration) {
    currentWeather.type = weather;
    currentWeather.duration = duration;
    
    // Set properties based on weather type
    switch (weather) {
        case WeatherType::FOG:
            currentWeather.name = "Dense Fog";
            currentWeather.description = "A thick fog reduces visibility.";
            currentWeather.accuracyModifier = -20;
            currentWeather.reducesVisibility = true;
            break;
        case WeatherType::STORM:
            currentWeather.name = "Thunderstorm";
            currentWeather.description = "Lightning crackles through the air.";
            currentWeather.associatedElement = DamageElement::LIGHTNING;
            currentWeather.dealsDamagePerTurn = true;
            currentWeather.damagePerTurn = 5;
            break;
        case WeatherType::BLIZZARD:
            currentWeather.name = "Blizzard";
            currentWeather.description = "Freezing winds cut through you.";
            currentWeather.associatedElement = DamageElement::ICE;
            currentWeather.damageModifier = -10;
            currentWeather.dealsDamagePerTurn = true;
            currentWeather.damagePerTurn = 8;
            break;
        case WeatherType::HEAT_WAVE:
            currentWeather.name = "Heat Wave";
            currentWeather.description = "Scorching heat fills the area.";
            currentWeather.associatedElement = DamageElement::FIRE;
            currentWeather.damageModifier = 10;  // Fire damage bonus
            break;
        case WeatherType::RAIN:
            currentWeather.name = "Heavy Rain";
            currentWeather.description = "Rain pours down constantly.";
            currentWeather.accuracyModifier = -5;
            break;
        case WeatherType::DARKNESS:
            currentWeather.name = "Unnatural Darkness";
            currentWeather.description = "An oppressive darkness fills the area.";
            currentWeather.reducesVisibility = true;
            currentWeather.accuracyModifier = -15;
            break;
        case WeatherType::BLOOD_MOON:
            currentWeather.name = "Blood Moon";
            currentWeather.description = "A crimson moon empowers enemies.";
            currentWeather.boostedEnemies = true;
            currentWeather.enemyStatMultiplier = 1.3f;
            break;
        case WeatherType::AURORA:
            currentWeather.name = "Magical Aurora";
            currentWeather.description = "Mystical lights enhance magic.";
            currentWeather.damageModifier = 15;  // Magic damage bonus
            break;
        default:
            currentWeather = WeatherEffect();  // Clear weather
            break;
    }
    
    if (weather != WeatherType::NONE) {
        std::cout << "\nWeather changed: " << currentWeather.name << "\n";
        std::cout << currentWeather.description << "\n";
    }
}

void EnvironmentManager::updateWeather() {
    if (currentWeather.type == WeatherType::NONE) {
        // Chance to generate random weather
        if (rand() % 100 < 5) {  // 5% chance
            generateRandomWeather();
        }
        return;
    }
    
    // Tick down duration
    currentWeather.duration--;
    
    if (currentWeather.duration <= 0) {
        std::cout << "The " << currentWeather.name << " has cleared.\n";
        currentWeather = WeatherEffect();
    }
}

WeatherType EnvironmentManager::getCurrentWeather() const {
    return currentWeather.type;
}

void EnvironmentManager::applyWeatherEffects(Player& player, CombatStats& enemyStats) {
    if (currentWeather.type == WeatherType::NONE) return;
    
    // Apply accuracy modifier
    if (currentWeather.accuracyModifier != 0) {
        // This would affect hit chance calculations
    }
    
    // Apply damage per turn
    if (currentWeather.dealsDamagePerTurn) {
        player.stats.currentHP -= currentWeather.damagePerTurn;
        std::cout << "Weather deals " << currentWeather.damagePerTurn << " damage!\n";
    }
    
    // Boost enemies during blood moon
    if (currentWeather.boostedEnemies) {
        enemyStats.attack = static_cast<int>(enemyStats.attack * currentWeather.enemyStatMultiplier);
        enemyStats.defense = static_cast<int>(enemyStats.defense * currentWeather.enemyStatMultiplier);
    }
}

void EnvironmentManager::displayWeatherInfo() const {
    if (currentWeather.type == WeatherType::NONE) {
        std::cout << "Weather: Clear\n";
        return;
    }
    
    std::cout << "Weather: " << currentWeather.name;
    std::cout << " (" << currentWeather.duration << " turns remaining)\n";
    std::cout << currentWeather.description << "\n";
}

void EnvironmentManager::addLandmark(int roomId, Landmark landmark) {
    roomLandmarks[roomId].push_back(landmark);
}

bool EnvironmentManager::interactWithLandmark(int landmarkId, Player& player) {
    int currentRoom = gameStateRef.player.currentRoomId;
    auto it = roomLandmarks.find(currentRoom);
    
    if (it == roomLandmarks.end()) return false;
    
    for (auto& landmark : it->second) {
        if (landmark.id == landmarkId) {
            if (!landmark.isInteractive) {
                std::cout << "This cannot be interacted with.\n";
                return false;
            }
            
            if (landmark.currentCooldown > 0) {
                std::cout << "This is still on cooldown. (" << landmark.currentCooldown << " turns)\n";
                return false;
            }
            
            // Check requirements
            if (landmark.requiresOffering) {
                if (landmark.offeringGold > 0 && player.gold < landmark.offeringGold) {
                    std::cout << "You need " << landmark.offeringGold << " gold to use this.\n";
                    return false;
                }
            }
            
            // Process interaction
            std::cout << "\n" << landmark.interactionText << "\n\n";
            
            // Apply effects
            if (landmark.healingAmount > 0) {
                player.stats.currentHP += landmark.healingAmount;
                if (player.stats.currentHP > player.stats.maxHP) {
                    player.stats.currentHP = player.stats.maxHP;
                }
                std::cout << "You are healed for " << landmark.healingAmount << " HP!\n";
            }
            
            if (landmark.manaRestoration > 0) {
                player.stats.currentMana += landmark.manaRestoration;
                if (player.stats.currentMana > player.stats.maxMana) {
                    player.stats.currentMana = player.stats.maxMana;
                }
                std::cout << "You restore " << landmark.manaRestoration << " mana!\n";
            }
            
            // Pay gold cost if any
            if (landmark.goldCost > 0) {
                player.gold -= landmark.goldCost;
                std::cout << "You pay " << landmark.goldCost << " gold.\n";
            }
            
            // Grant recipes if any
            for (int recipeId : landmark.grantedRecipes) {
                std::cout << "You learn a new recipe!\n";
                (void)recipeId;  // Would integrate with crafting system
            }
            
            landmark.hasBeenUsed = true;
            landmark.currentCooldown = landmark.cooldownTurns;
            
            return true;
        }
    }
    
    return false;
}

void EnvironmentManager::displayLandmarksInRoom(int roomId) const {
    auto it = roomLandmarks.find(roomId);
    
    if (it == roomLandmarks.end() || it->second.empty()) {
        return;
    }
    
    std::cout << "\nLandmarks in this area:\n";
    for (const auto& landmark : it->second) {
        std::cout << "  [" << landmark.id << "] " << landmark.name;
        if (landmark.isInteractive) {
            std::cout << " (Interactive)";
        }
        if (landmark.currentCooldown > 0) {
            std::cout << " [Cooldown: " << landmark.currentCooldown << "]";
        }
        std::cout << "\n    " << landmark.description << "\n";
    }
}

std::vector<Landmark> EnvironmentManager::getLandmarksInRoom(int roomId) const {
    auto it = roomLandmarks.find(roomId);
    if (it != roomLandmarks.end()) {
        return it->second;
    }
    return std::vector<Landmark>();
}

void EnvironmentManager::checkEnvironmentalHazards(Player& player, std::vector<std::string>& log) {
    int currentRoom = gameStateRef.player.currentRoomId;
    auto it = activeHazards.find(currentRoom);
    
    if (it == activeHazards.end()) return;
    
    for (auto& hazard : it->second) {
        // Check activation chance
        if (static_cast<float>(rand() % 100) / 100.0f < hazard.activationChance) {
            // Check if avoidable
            bool avoided = false;
            if (hazard.isAvoidable) {
                int dodgeRoll = rand() % 100;
                if (dodgeRoll < player.stats.dodgeChance) {
                    avoided = true;
                    std::stringstream ss;
                    ss << "You dodge the " << hazard.name << "!";
                    log.push_back(ss.str());
                }
            }
            
            if (!avoided) {
                player.stats.currentHP -= hazard.damage;
                std::stringstream ss;
                ss << hazard.name << " deals " << hazard.damage << " damage!";
                log.push_back(ss.str());
            }
        }
    }
}

void EnvironmentManager::spawnHazard(int roomId, const EnvironmentalHazard& hazard) {
    activeHazards[roomId].push_back(hazard);
}

void EnvironmentManager::displayActiveHazards() const {
    int currentRoom = gameStateRef.player.currentRoomId;
    auto it = activeHazards.find(currentRoom);
    
    if (it == activeHazards.end() || it->second.empty()) {
        return;
    }
    
    std::cout << "\nHazards in this area:\n";
    for (const auto& hazard : it->second) {
        std::cout << "  - " << hazard.name << ": " << hazard.description << "\n";
    }
}

void EnvironmentManager::addTerrainFeature(int roomId, const InteractiveTerrain& terrain) {
    roomTerrain[roomId].push_back(terrain);
}

bool EnvironmentManager::interactWithTerrain(int roomId, TerrainFeature type, Player& player) {
    auto it = roomTerrain.find(roomId);
    if (it == roomTerrain.end()) return false;
    
    for (auto& terrain : it->second) {
        if (terrain.type == type) {
            if (terrain.isLocked) {
                // Check for key
                bool hasKey = false;
                for (const auto& item : player.inventory) {
                    if (item.id == terrain.requiredKeyId) {
                        hasKey = true;
                        break;
                    }
                }
                
                if (!hasKey) {
                    std::cout << "This is locked. You need a key.\n";
                    return false;
                }
                
                terrain.isLocked = false;
                std::cout << "You unlock it!\n";
            }
            
            // Handle different terrain types
            switch (type) {
                case TerrainFeature::CHEST:
                    if (!terrain.containedItems.empty()) {
                        std::cout << "You open the chest!\n";
                        for (const auto& item : terrain.containedItems) {
                            player.inventory.push_back(item);
                            std::cout << "Found: " << item.name << "\n";
                        }
                        terrain.containedItems.clear();
                    } else {
                        std::cout << "The chest is empty.\n";
                    }
                    break;
                    
                case TerrainFeature::LEVER:
                    terrain.isActive = !terrain.isActive;
                    std::cout << "You pull the lever. " << (terrain.isActive ? "It activates." : "It deactivates.") << "\n";
                    if (terrain.triggersEvent) {
                        std::cout << "Something happens...\n";
                    }
                    break;
                    
                case TerrainFeature::DOOR:
                    if (!terrain.isActive) {
                        terrain.isActive = true;
                        std::cout << "The door opens.\n";
                    } else {
                        terrain.isActive = false;
                        std::cout << "The door closes.\n";
                    }
                    break;
                    
                case TerrainFeature::BARREL:
                    std::cout << "You break open the barrel.\n";
                    if (!terrain.containedItems.empty()) {
                        for (const auto& item : terrain.containedItems) {
                            player.inventory.push_back(item);
                            std::cout << "Found: " << item.name << "\n";
                        }
                        terrain.containedItems.clear();
                    }
                    break;
                    
                default:
                    std::cout << "You examine it closely.\n";
                    break;
            }
            
            return true;
        }
    }
    
    return false;
}

std::vector<InteractiveTerrain> EnvironmentManager::getTerrainInRoom(int roomId) const {
    auto it = roomTerrain.find(roomId);
    if (it != roomTerrain.end()) {
        return it->second;
    }
    return std::vector<InteractiveTerrain>();
}

std::vector<CraftingMaterial> EnvironmentManager::gatherMaterials(int roomId) {
    std::vector<CraftingMaterial> gathered;
    
    BiomeType biome = BiomeType::DUNGEON_DEPTHS;
    auto biomeIt = roomBiomes.find(roomId);
    if (biomeIt != roomBiomes.end()) {
        biome = biomeIt->second;
    }
    
    if (!canGatherInBiome(biome)) {
        std::cout << "Nothing to gather here.\n";
        return gathered;
    }
    
    // Get biome-specific materials
    auto propIt = biomeData.find(biome);
    if (propIt == biomeData.end()) return gathered;
    
    const BiomeProperties& props = propIt->second;
    
    for (const auto& pair : props.materialSpawnChance) {
        float roll = static_cast<float>(rand() % 100) / 100.0f;
        if (roll < pair.second) {
            CraftingMaterial mat;
            mat.type = pair.first;
            mat.quantity = 1 + rand() % 3;
            
            switch (pair.first) {
                case MaterialType::WOOD: mat.name = "Wood"; break;
                case MaterialType::METAL: mat.name = "Iron Ore"; break;
                case MaterialType::LEATHER: mat.name = "Leather Scraps"; break;
                case MaterialType::HERB: mat.name = "Herb"; break;
                case MaterialType::CRYSTAL: mat.name = "Crystal"; break;
                case MaterialType::GEM: mat.name = "Gem"; break;
                default: mat.name = "Resource"; break;
            }
            
            gathered.push_back(mat);
            std::cout << "Gathered " << mat.quantity << " " << mat.name << "!\n";
        }
    }
    
    if (gathered.empty()) {
        std::cout << "You found nothing this time.\n";
    }
    
    return gathered;
}

bool EnvironmentManager::canGatherInBiome(BiomeType biome) const {
    // Most biomes allow gathering
    switch (biome) {
        case BiomeType::SHADOW_REALM:
            return false;  // Too dangerous to gather
        default:
            return true;
    }
}

std::string EnvironmentManager::serializeEnvironment() const {
    std::stringstream ss;
    
    // Serialize current weather
    ss << static_cast<int>(currentWeather.type) << "," << currentWeather.duration << "|";
    
    // Serialize room biomes
    for (const auto& pair : roomBiomes) {
        ss << pair.first << ":" << static_cast<int>(pair.second) << ";";
    }
    
    return ss.str();
}

void EnvironmentManager::deserializeEnvironment(const std::string& data) {
    if (data.empty()) return;
    
    std::stringstream ss(data);
    std::string segment;
    
    // Weather
    if (std::getline(ss, segment, '|')) {
        size_t comma = segment.find(',');
        if (comma != std::string::npos) {
            int weatherType = std::stoi(segment.substr(0, comma));
            int duration = std::stoi(segment.substr(comma + 1));
            setWeather(static_cast<WeatherType>(weatherType), duration);
        }
    }
    
    // Room biomes
    if (std::getline(ss, segment, '|')) {
        std::stringstream biomeSs(segment);
        std::string entry;
        while (std::getline(biomeSs, entry, ';')) {
            if (entry.empty()) continue;
            size_t colon = entry.find(':');
            if (colon != std::string::npos) {
                int roomId = std::stoi(entry.substr(0, colon));
                int biome = std::stoi(entry.substr(colon + 1));
                setBiome(roomId, static_cast<BiomeType>(biome));
            }
        }
    }
}

void EnvironmentManager::initializeBiomes() {
    // Dungeon Depths - Starting area
    BiomeProperties dungeonDepths;
    dungeonDepths.type = BiomeType::DUNGEON_DEPTHS;
    dungeonDepths.name = "Dungeon Depths";
    dungeonDepths.description = "Dark stone corridors stretch endlessly.";
    dungeonDepths.flavorTexts = {
        "Dripping water echoes in the distance.",
        "Cobwebs cling to ancient stone pillars.",
        "The air is damp and musty."
    };
    dungeonDepths.materialSpawnChance[MaterialType::METAL] = 0.2f;
    dungeonDepths.materialSpawnChance[MaterialType::BONE] = 0.15f;
    biomeData[BiomeType::DUNGEON_DEPTHS] = dungeonDepths;
    
    // Volcanic Cavern
    BiomeProperties volcanic;
    volcanic.type = BiomeType::VOLCANIC_CAVERN;
    volcanic.name = "Volcanic Caverns";
    volcanic.description = "Molten rivers of lava illuminate the cavern.";
    volcanic.flavorTexts = {
        "The heat is almost unbearable.",
        "Lava bubbles and hisses nearby.",
        "Volcanic ash covers everything."
    };
    volcanic.passiveDamagePerTurn = 5;
    volcanic.damageElement = DamageElement::FIRE;
    volcanic.hasLavaPools = true;
    volcanic.hazardDamage = 15;
    volcanic.boostedElement = DamageElement::FIRE;
    volcanic.elementalBoost = 1.5f;
    volcanic.materialSpawnChance[MaterialType::CRYSTAL] = 0.25f;
    biomeData[BiomeType::VOLCANIC_CAVERN] = volcanic;
    
    // Icy Halls
    BiomeProperties icyHalls;
    icyHalls.type = BiomeType::ICY_HALLS;
    icyHalls.name = "Icy Halls";
    icyHalls.description = "Frozen corridors covered in frost and ice.";
    icyHalls.flavorTexts = {
        "Your breath forms clouds in the frigid air.",
        "Ice crystals glitter on the walls.",
        "The cold seeps into your bones."
    };
    icyHalls.movementSpeedModifier = 0.8f;
    icyHalls.hasIcePatches = true;
    icyHalls.boostedElement = DamageElement::ICE;
    icyHalls.elementalBoost = 1.5f;
    icyHalls.materialSpawnChance[MaterialType::CRYSTAL] = 0.3f;
    biomeData[BiomeType::ICY_HALLS] = icyHalls;
    
    // Corrupted Crypt
    BiomeProperties corruptedCrypt;
    corruptedCrypt.type = BiomeType::CORRUPTED_CRYPT;
    corruptedCrypt.name = "Corrupted Crypts";
    corruptedCrypt.description = "Ancient tombs tainted by dark magic.";
    corruptedCrypt.flavorTexts = {
        "An oppressive evil hangs in the air.",
        "Skeletal remains litter the ground.",
        "Green mist seeps from the walls."
    };
    corruptedCrypt.hasPoisonGas = true;
    corruptedCrypt.hazardDamage = 10;
    corruptedCrypt.boostedElement = DamageElement::SHADOW;
    corruptedCrypt.elementalBoost = 1.3f;
    corruptedCrypt.materialSpawnChance[MaterialType::BONE] = 0.4f;
    corruptedCrypt.materialSpawnChance[MaterialType::ESSENCE] = 0.15f;
    biomeData[BiomeType::CORRUPTED_CRYPT] = corruptedCrypt;
    
    // Crystal Mines
    BiomeProperties crystalMines;
    crystalMines.type = BiomeType::CRYSTAL_MINES;
    crystalMines.name = "Crystal Mines";
    crystalMines.description = "Sparkling crystals of all colors line the walls.";
    crystalMines.flavorTexts = {
        "Brilliant light reflects off countless crystals.",
        "Gems of every hue shimmer around you.",
        "The crystal formations are breathtaking."
    };
    crystalMines.materialSpawnChance[MaterialType::GEM] = 0.35f;
    crystalMines.materialSpawnChance[MaterialType::CRYSTAL] = 0.4f;
    biomeData[BiomeType::CRYSTAL_MINES] = crystalMines;
    
    // Enchanted Forest
    BiomeProperties enchantedForest;
    enchantedForest.type = BiomeType::ENCHANTED_FOREST;
    enchantedForest.name = "Enchanted Grove";
    enchantedForest.description = "An underground forest of magical plants.";
    enchantedForest.flavorTexts = {
        "Bioluminescent fungi light your path.",
        "Strange plants pulse with inner light.",
        "The air itself feels magical."
    };
    enchantedForest.hasNaturalHealing = true;
    enchantedForest.healingPerTurn = 5;
    enchantedForest.materialSpawnChance[MaterialType::HERB] = 0.5f;
    enchantedForest.materialSpawnChance[MaterialType::WOOD] = 0.3f;
    biomeData[BiomeType::ENCHANTED_FOREST] = enchantedForest;
    
    // Shadow Realm
    BiomeProperties shadowRealm;
    shadowRealm.type = BiomeType::SHADOW_REALM;
    shadowRealm.name = "Shadow Realm";
    shadowRealm.description = "A dimension of pure darkness and evil.";
    shadowRealm.flavorTexts = {
        "Darkness itself seems alive here.",
        "Whispers of the damned echo around you.",
        "Reality bends in this twisted place."
    };
    shadowRealm.visibilityRange = 0.5f;
    shadowRealm.enemyDamageModifier = 1.25f;
    shadowRealm.boostedElement = DamageElement::SHADOW;
    shadowRealm.elementalBoost = 2.0f;
    shadowRealm.materialSpawnChance[MaterialType::ESSENCE] = 0.3f;
    biomeData[BiomeType::SHADOW_REALM] = shadowRealm;
    
    // Sky Citadel
    BiomeProperties skyCitadel;
    skyCitadel.type = BiomeType::SKY_CITADEL;
    skyCitadel.name = "Sky Citadel";
    skyCitadel.description = "A floating fortress crackling with lightning.";
    skyCitadel.flavorTexts = {
        "Lightning arcs between spires.",
        "The wind howls through the halls.",
        "You can see clouds through the windows."
    };
    skyCitadel.hasElectricArcs = true;
    skyCitadel.hazardDamage = 20;
    skyCitadel.boostedElement = DamageElement::LIGHTNING;
    skyCitadel.elementalBoost = 1.5f;
    skyCitadel.materialSpawnChance[MaterialType::CRYSTAL] = 0.25f;
    biomeData[BiomeType::SKY_CITADEL] = skyCitadel;
}

void EnvironmentManager::generateRandomWeather() {
    int roll = rand() % 100;
    
    if (roll < 20) {
        setWeather(WeatherType::FOG, 5 + rand() % 5);
    } else if (roll < 35) {
        setWeather(WeatherType::RAIN, 3 + rand() % 4);
    } else if (roll < 45) {
        setWeather(WeatherType::STORM, 3 + rand() % 3);
    } else if (roll < 55) {
        setWeather(WeatherType::DARKNESS, 4 + rand() % 4);
    } else if (roll < 60) {
        setWeather(WeatherType::BLOOD_MOON, 5);
    } else if (roll < 65) {
        setWeather(WeatherType::AURORA, 3 + rand() % 3);
    }
    // Otherwise, no weather change
}

BiomeProperties& EnvironmentManager::getBiomeProperties(BiomeType biome) {
    static BiomeProperties dummy;
    auto it = biomeData.find(biome);
    if (it != biomeData.end()) {
        return it->second;
    }
    return dummy;
}

//==============================================================================
// BiomeMapGenerator Implementation
//==============================================================================

BiomeMapGenerator::BiomeMapGenerator(int width, int height) 
    : mapWidth(width), mapHeight(height) {
    biomeGrid.resize(height, std::vector<BiomeType>(width, BiomeType::DUNGEON_DEPTHS));
}

void BiomeMapGenerator::generateBiomeMap(GameState& state) {
    int numZones = 4 + rand() % 3;  // 4-6 biome zones
    createBiomeZones(numZones);
    placeTransitionRooms();
    placeLandmarks();
    placeResources();
    
    // Apply biomes to rooms
    for (auto& room : state.dungeon) {
        // Calculate room position in grid
        int x = room.id % mapWidth;
        int y = room.id / mapWidth;
        
        if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
            // Room would get the biome from the grid
            (void)x;
            (void)y;
        }
    }
}

void BiomeMapGenerator::createBiomeZones(int numZones) {
    for (int zone = 0; zone < numZones; ++zone) {
        int centerX = rand() % mapWidth;
        int centerY = rand() % mapHeight;
        BiomeType biome = selectBiomeForZone(zone);
        
        int maxSpread = 3 + rand() % 4;
        floodFillBiome(centerX, centerY, biome, maxSpread);
    }
}

void BiomeMapGenerator::placeTransitionRooms() {
    // Smooth biome transitions
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            // Check neighbors for transition zones
        }
    }
}

void BiomeMapGenerator::placeLandmarks() {
    // Place shrines, altars, etc. at strategic locations
}

void BiomeMapGenerator::placeResources() {
    // Place resource nodes for gathering
}

void BiomeMapGenerator::ensurePathToBoss() {
    // Make sure there's a valid path to the boss room
}

void BiomeMapGenerator::createSecretPaths() {
    // Add hidden passages
}

void BiomeMapGenerator::placeShortcuts() {
    // Place unlockable shortcuts between areas
}

void BiomeMapGenerator::placeArenaChamber() {
    // Place a wave combat arena
}

void BiomeMapGenerator::placeVaultRoom() {
    // Place a high-risk treasure room
}

void BiomeMapGenerator::placeLoreRoom() {
    // Place story/lore rooms
}

BiomeType BiomeMapGenerator::selectBiomeForZone(int zoneIndex) {
    // Select biome based on zone position
    // Earlier zones are safer, later zones are more dangerous
    
    std::vector<BiomeType> earlyBiomes = {
        BiomeType::DUNGEON_DEPTHS,
        BiomeType::CRYSTAL_MINES,
        BiomeType::ENCHANTED_FOREST
    };
    
    std::vector<BiomeType> midBiomes = {
        BiomeType::CORRUPTED_CRYPT,
        BiomeType::ICY_HALLS,
        BiomeType::VOLCANIC_CAVERN
    };
    
    std::vector<BiomeType> lateBiomes = {
        BiomeType::SHADOW_REALM,
        BiomeType::SKY_CITADEL
    };
    
    if (zoneIndex < 2) {
        return earlyBiomes[rand() % earlyBiomes.size()];
    } else if (zoneIndex < 4) {
        return midBiomes[rand() % midBiomes.size()];
    } else {
        return lateBiomes[rand() % lateBiomes.size()];
    }
}

void BiomeMapGenerator::floodFillBiome(int x, int y, BiomeType biome, int maxSpread) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return;
    if (maxSpread <= 0) return;
    
    biomeGrid[y][x] = biome;
    
    // Spread to neighbors with decreasing probability
    int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    for (auto& dir : directions) {
        if (rand() % 100 < 70) {  // 70% chance to spread
            floodFillBiome(x + dir[0], y + dir[1], biome, maxSpread - 1);
        }
    }
}

}  // namespace AQE
