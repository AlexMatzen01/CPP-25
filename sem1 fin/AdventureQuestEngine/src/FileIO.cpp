//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// File I/O implementation

#include "../include/FileIO.h"
#include "../include/Types.h"
#include "../include/json.hpp"


using json = nlohmann::json;

namespace AQE {
    NLOHMANN_JSON_SERIALIZE_ENUM(ItemType, {
        {ItemType::CONSUMABLE, "CONSUMABLE"},
        {ItemType::WEAPON, "WEAPON"},
        {ItemType::ARMOR, "ARMOR"},
        {ItemType::ACCESSORY, "ACCESSORY"},
        {ItemType::KEY, "KEY"},
        {ItemType::QUEST, "QUEST"},
        {ItemType::SPECIAL, "SPECIAL"}
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(ItemRarity, {
        {ItemRarity::COMMON, "COMMON"},
        {ItemRarity::UNCOMMON, "UNCOMMON"},
        {ItemRarity::RARE, "RARE"},
        {ItemRarity::EPIC, "EPIC"},
        {ItemRarity::LEGENDARY, "LEGENDARY"}
    })

    void from_json(const json& j, Item& i) {
        j.at("id").get_to(i.id);
        j.at("name").get_to(i.name);
        j.at("description").get_to(i.description);
        j.at("type").get_to(i.type);
        j.at("rarity").get_to(i.rarity);
        j.at("value").get_to(i.value);
        if (j.contains("quantity")) j.at("quantity").get_to(i.quantity);
        if (j.contains("healAmount")) j.at("healAmount").get_to(i.healAmount);
        if (j.contains("attackBonus")) j.at("attackBonus").get_to(i.attackBonus);
        if (j.contains("defenseBonus")) j.at("defenseBonus").get_to(i.defenseBonus);
        if (j.contains("critBonus")) j.at("critBonus").get_to(i.critBonus);
        if (j.contains("dodgeBonus")) j.at("dodgeBonus").get_to(i.dodgeBonus);
        if (j.contains("speedBonus")) j.at("speedBonus").get_to(i.speedBonus);
        if (j.contains("duration")) j.at("duration").get_to(i.duration);
        if (j.contains("questId")) j.at("questId").get_to(i.questId);
        if (j.contains("isEquipped")) j.at("isEquipped").get_to(i.isEquipped);
        if (j.contains("isUsable")) j.at("isUsable").get_to(i.isUsable);
        if (j.contains("isStackable")) j.at("isStackable").get_to(i.isStackable);
        if (j.contains("durability")) j.at("durability").get_to(i.durability);
        if (j.contains("maxDurability")) j.at("maxDurability").get_to(i.maxDurability);
        if (j.contains("upgradeLevel")) j.at("upgradeLevel").get_to(i.upgradeLevel);
        if (j.contains("isEnchanted")) j.at("isEnchanted").get_to(i.isEnchanted);
        if (j.contains("enchantmentName")) j.at("enchantmentName").get_to(i.enchantmentName);
    }
    
    void from_json(const json& j, StatusEffect& s) {
        j.at("name").get_to(s.name);
        j.at("description").get_to(s.description);
        j.at("attackMod").get_to(s.attackMod);
        j.at("defenseMod").get_to(s.defenseMod);
        j.at("critMod").get_to(s.critMod);
        j.at("dodgeMod").get_to(s.dodgeMod);
        j.at("speedMod").get_to(s.speedMod);
        j.at("turnsRemaining").get_to(s.turnsRemaining);
        j.at("isDebuff").get_to(s.isDebuff);
    }
    
    NLOHMANN_JSON_SERIALIZE_ENUM(EnemyIntent, {
        {EnemyIntent::UNKNOWN, "UNKNOWN"},
        {EnemyIntent::ATTACK_LIGHT, "ATTACK_LIGHT"},
        {EnemyIntent::ATTACK_HEAVY, "ATTACK_HEAVY"},
        {EnemyIntent::DEFEND, "DEFEND"},
        {EnemyIntent::DEBUFF, "DEBUFF"},
        {EnemyIntent::BUFF, "BUFF"},
        {EnemyIntent::HEAL, "HEAL"},
        {EnemyIntent::CONTROL, "CONTROL"},
        {EnemyIntent::SUMMON, "SUMMON"},
        {EnemyIntent::CHARGE, "CHARGE"}
    })

    void from_json(const json& j, CombatStats& c) {
        j.at("maxHP").get_to(c.maxHP);
        j.at("currentHP").get_to(c.currentHP);
        j.at("maxMana").get_to(c.maxMana);
        j.at("currentMana").get_to(c.currentMana);
        j.at("attack").get_to(c.attack);
        j.at("defense").get_to(c.defense);
        j.at("magicPower").get_to(c.magicPower);
        j.at("speed").get_to(c.speed);
        j.at("critChance").get_to(c.critChance);
        j.at("dodgeChance").get_to(c.dodgeChance);
    }

    void from_json(const json& j, EnemyAbility& a) {
        j.at("name").get_to(a.name);
        j.at("description").get_to(a.description);
        j.at("damage").get_to(a.damage);
        j.at("cooldown").get_to(a.cooldown);
        if (j.contains("currentCooldown")) j.at("currentCooldown").get_to(a.currentCooldown);
        j.at("triggerChance").get_to(a.triggerChance);
        j.at("heals").get_to(a.heals);
        j.at("healAmount").get_to(a.healAmount);
    }

    void from_json(const json& j, Enemy& e) {
        j.at("id").get_to(e.id);
        j.at("name").get_to(e.name);
        j.at("description").get_to(e.description);
        if (j.contains("asciiArt")) j.at("asciiArt").get_to(e.asciiArt);
        j.at("stats").get_to(e.stats);
        if (j.contains("abilities")) j.at("abilities").get_to(e.abilities);
        if (j.contains("experienceReward")) j.at("experienceReward").get_to(e.experienceReward);
        if (j.contains("goldReward")) j.at("goldReward").get_to(e.goldReward);
        if (j.contains("lootTable")) j.at("lootTable").get_to(e.lootTable);
        if (j.contains("lootChances")) j.at("lootChances").get_to(e.lootChances);
        if (j.contains("isBoss")) j.at("isBoss").get_to(e.isBoss);
        if (j.contains("dialogue")) j.at("dialogue").get_to(e.dialogue);
        if (j.contains("activeEffects")) j.at("activeEffects").get_to(e.activeEffects);
        if (j.contains("nextIntent")) j.at("nextIntent").get_to(e.nextIntent);
        if (j.contains("intentStrength")) j.at("intentStrength").get_to(e.intentStrength);
    }
}
#include "../include/MapGenerator.h"
#include "../include/Utils.h"
#include "../include/Constants.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <filesystem>

// Platform-specific includes for directory creation
#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
#else
    #include <sys/stat.h>
    #define MKDIR(path) mkdir(path, 0755)
#endif

namespace AQE {

// ---
// CONSTRUCTOR
// ---

FileIO::FileIO(const std::string& dataFolder) : dataPath(dataFolder) {
    ensureDataDirectory();
}

// ---
// DATA LOADING
// ---

bool FileIO::loadAllGameData(GameState& gameState) {
    bool success = true;
    
    // Load items
    gameState.itemTemplates = loadItems(dataPath + "items.json");
    if (gameState.itemTemplates.empty()) {
        gameState.itemTemplates = createDefaultItems();
    }
    
    // Load enemies
    gameState.enemyTemplates = loadEnemies(dataPath + "enemies.json");
    if (gameState.enemyTemplates.empty()) {
        gameState.enemyTemplates = createDefaultEnemies();
    }
    
    // Load events
    gameState.eventTemplates = loadEvents(dataPath + "events.txt");
    
    // Load quests
    gameState.questTemplates = loadQuests(dataPath + "quests.txt");
    
    return success;
}

std::vector<Room> FileIO::loadRooms(const std::string& filename) {
    std::vector<Room> rooms;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cout << "Note: Could not open " << filename << ", using defaults.\n";
        return createDefaultRooms();
    }
    
    std::string line;
    Room current;
    bool inRoom = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') continue;
        
        if (line == "[ROOM]") {
            if (inRoom) {
                rooms.push_back(current);
            }
            current = Room();
            inRoom = true;
            continue;
        }
        
        if (!inRoom) continue;
        
        std::string key, value;
        if (parseKeyValue(line, key, value)) {
            if (key == "name") current.name = value;
            else if (key == "description") current.description = value;
            else if (key == "shortdesc") current.shortDesc = value;
            else if (key == "type") {
                if (value == "normal") current.type = RoomType::NORMAL;
                else if (value == "treasure") current.type = RoomType::TREASURE;
                else if (value == "boss") current.type = RoomType::BOSS;
                else if (value == "trap") current.type = RoomType::TRAP;
                else if (value == "puzzle") current.type = RoomType::PUZZLE;
                else if (value == "secret") current.type = RoomType::SECRET;
                else if (value == "safe") current.type = RoomType::SAFE;
            }
            else if (key == "puzzle_question") current.puzzleQuestion = value;
            else if (key == "puzzle_answer") current.puzzleAnswer = value;
            else if (key == "trap_damage") current.trapDamage = std::stoi(value);
        }
    }
    
    if (inRoom) {
        rooms.push_back(current);
    }
    
    file.close();
    return rooms;
}

std::vector<Item> FileIO::loadItems(const std::string& filename) {
    std::vector<Item> items;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Could not open " << filename << ". Using default items.\n";
        return createDefaultItems();
    }
    
    try {
        nlohmann::json j;
        file >> j;
        items = j.get<std::vector<Item>>();
    } catch (const std::exception& e) {
        std::cerr << "Error parsing items JSON: " << e.what() << "\nUsing defaults instead.\n";
        return createDefaultItems();
    }
    return items;
}

std::vector<Enemy> FileIO::loadEnemies(const std::string& filename) {
    std::vector<Enemy> enemies;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Could not open " << filename << ". Using default enemies.\n";
        return createDefaultEnemies();
    }
    
    try {
        nlohmann::json j;
        file >> j;
        enemies = j.get<std::vector<Enemy>>();
    } catch (const std::exception& e) {
        std::cerr << "Error parsing enemies JSON: " << e.what() << "\nUsing defaults instead.\n";
        return createDefaultEnemies();
    }
    return enemies;
}

std::vector<Riddle> FileIO::loadPuzzles(const std::string& filename) {
    std::vector<Riddle> puzzles;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        // Return default riddles from Constants.h
        return RIDDLE_DATABASE;
    }
    
    std::string line;
    Riddle current;
    bool inPuzzle = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') continue;
        
        if (line == "[PUZZLE]") {
            if (inPuzzle && !current.question.empty()) {
                puzzles.push_back(current);
            }
            current = Riddle();
            inPuzzle = true;
            continue;
        }
        
        if (!inPuzzle) continue;
        
        std::string key, value;
        if (parseKeyValue(line, key, value)) {
            if (key == "question") current.question = value;
            else if (key == "answer") current.answer = value;
            else if (key == "hint") current.hint = value;
        }
    }
    
    if (inPuzzle && !current.question.empty()) {
        puzzles.push_back(current);
    }
    
    file.close();
    
    if (puzzles.empty()) {
        return RIDDLE_DATABASE;
    }
    
    return puzzles;
}

std::vector<Event> FileIO::loadEvents(const std::string& filename) {
    std::vector<Event> events;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        // Return empty vector - events are optional
        return events;
    }
    
    // Simple stub implementation
    // In a full implementation, this would parse event data from file
    file.close();
    return events;
}

std::vector<Quest> FileIO::loadQuests(const std::string& filename) {
    std::vector<Quest> quests;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        // Return empty vector - quests are optional
        return quests;
    }
    
    // Simple stub implementation
    // In a full implementation, this would parse quest data from file
    file.close();
    return quests;
}

// ---
// GAME SAVING/LOADING
// ---

bool FileIO::saveGame(const GameState& gameState, const std::string& filename) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not create save file.\n";
        return false;
    }
    
    const Player& p = gameState.player;
    
    file << "# Adventure Quest Engine Save File\n";
    file << "# DO NOT EDIT MANUALLY\n\n";
    
    file << "[PLAYER]\n";
    file << "name=" << p.name << "\n";
    file << "class=" << static_cast<int>(p.charClass) << "\n";
    file << "level=" << p.level << "\n";
    file << "experience=" << p.experience << "\n";
    file << "gold=" << p.gold << "\n";
    file << "currentHP=" << p.stats.currentHP << "\n";
    file << "maxHP=" << p.stats.maxHP << "\n";
    file << "attack=" << p.stats.attack << "\n";
    file << "defense=" << p.stats.defense << "\n";
    file << "critChance=" << p.stats.critChance << "\n";
    file << "dodgeChance=" << p.stats.dodgeChance << "\n";
    file << "speed=" << p.stats.speed << "\n";
    file << "currentRoom=" << p.currentRoomId << "\n";
    file << "morality=" << p.moralityScore << "\n";
    file << "kills=" << p.totalKills << "\n";
    file << "turns=" << p.totalTurns << "\n";
    file << "secrets=" << p.secretsFound << "\n";
    file << "puzzles=" << p.puzzlesSolved << "\n";
    file << "bosses=" << p.bossesDefeated << "\n";
    
    // Save inventory
    file << "\n[INVENTORY]\n";
    file << "count=" << p.inventory.size() << "\n";
    for (const Item& item : p.inventory) {
        file << "item=" << item.id << "," << item.name << "," 
             << static_cast<int>(item.type) << ","
             << static_cast<int>(item.rarity) << ","
             << item.quantity << "," << item.isEquipped << "\n";
    }
    
    // Save visited rooms
    file << "\n[VISITED]\n";
    for (int roomId : p.visitedRooms) {
        file << "room=" << roomId << "\n";
    }
    
    // Save game state
    file << "\n[GAMESTATE]\n";
    file << "mapWidth=" << gameState.mapWidth << "\n";
    file << "mapHeight=" << gameState.mapHeight << "\n";
    file << "dungeonLevel=" << gameState.currentDungeonLevel << "\n";
    file << "seed=" << gameState.randomSeed << "\n";
    file << "difficulty=" << gameState.difficulty << "\n";
    
    // Save dungeon rooms (simplified - just cleared status)
    file << "\n[DUNGEON]\n";
    for (const Room& room : gameState.dungeon) {
        file << "room=" << room.id << "," << room.isExplored << "," 
             << room.isCleared << "," << room.isTrapActive << ","
             << room.puzzleSolved << "\n";
    }
    
    file.close();
    
    std::cout << "Game saved successfully!\n";
    return true;
}

bool FileIO::loadGame(GameState& gameState, const std::string& filename) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Save file not found.\n";
        return false;
    }
    
    Player& p = gameState.player;
    struct LoadedRoomState {
        int roomId;
        bool isExplored;
        bool isCleared;
        bool isTrapActive;
        bool puzzleSolved;
    };

    std::vector<LoadedRoomState> loadedRoomStates;
    std::string line;
    std::string section;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') continue;
        
        if (line[0] == '[') {
            section = line;
            continue;
        }
        
        std::string key, value;
        if (!parseKeyValue(line, key, value)) continue;
        
        if (section == "[PLAYER]") {
            if (key == "name") p.name = value;
            else if (key == "class") p.charClass = static_cast<CharacterClass>(std::stoi(value));
            else if (key == "level") p.level = std::stoi(value);
            else if (key == "experience") p.experience = std::stoi(value);
            else if (key == "gold") p.gold = std::stoi(value);
            else if (key == "currentHP") p.stats.currentHP = std::stoi(value);
            else if (key == "maxHP") p.stats.maxHP = std::stoi(value);
            else if (key == "attack") p.stats.attack = std::stoi(value);
            else if (key == "defense") p.stats.defense = std::stoi(value);
            else if (key == "critChance") p.stats.critChance = std::stoi(value);
            else if (key == "dodgeChance") p.stats.dodgeChance = std::stoi(value);
            else if (key == "speed") p.stats.speed = std::stoi(value);
            else if (key == "currentRoom") p.currentRoomId = std::stoi(value);
            else if (key == "morality") p.moralityScore = std::stoi(value);
            else if (key == "kills") p.totalKills = std::stoi(value);
            else if (key == "turns") p.totalTurns = std::stoi(value);
            else if (key == "secrets") p.secretsFound = std::stoi(value);
            else if (key == "puzzles") p.puzzlesSolved = std::stoi(value);
            else if (key == "bosses") p.bossesDefeated = std::stoi(value);
        }
        else if (section == "[INVENTORY]") {
            if (key == "item") {
                // Parse item: id,name,type,rarity,quantity,equipped
                std::vector<std::string> parts = split(value, ',');
                if (parts.size() >= 6) {
                    Item item;
                    item.id = std::stoi(parts[0]);
                    item.name = parts[1];
                    item.type = static_cast<ItemType>(std::stoi(parts[2]));
                    item.rarity = static_cast<ItemRarity>(std::stoi(parts[3]));
                    item.quantity = std::stoi(parts[4]);
                    item.isEquipped = (parts[5] == "1");
                    p.inventory.push_back(item);
                }
            }
        }
        else if (section == "[VISITED]") {
            if (key == "room") {
                p.visitedRooms.push_back(std::stoi(value));
            }
        }
        else if (section == "[GAMESTATE]") {
            if (key == "mapWidth") gameState.mapWidth = std::stoi(value);
            else if (key == "mapHeight") gameState.mapHeight = std::stoi(value);
            else if (key == "dungeonLevel") gameState.currentDungeonLevel = std::stoi(value);
            else if (key == "seed") gameState.randomSeed = std::stoul(value);
            else if (key == "difficulty") gameState.difficulty = std::stoi(value);
        }
        else if (section == "[DUNGEON]") {
            if (key == "room") {
                std::vector<std::string> parts = split(value, ',');
                if (parts.size() >= 5) {
                    loadedRoomStates.push_back({
                        std::stoi(parts[0]),
                        parts[1] == "1",
                        parts[2] == "1",
                        parts[3] == "1",
                        parts[4] == "1"
                    });
                }
            }
        }
    }
    
    file.close();

    if (gameState.randomSeed == 0) {
        gameState.randomSeed = static_cast<unsigned int>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count());
        if (gameState.randomSeed == 0) {
            gameState.randomSeed = 1;
        }
    }

    if (gameState.dungeon.empty()) {
        const int width = (gameState.mapWidth > 0) ? gameState.mapWidth : 8;
        const int height = (gameState.mapHeight > 0) ? gameState.mapHeight : 8;

        initRandom(gameState.randomSeed);

        MapGenerator generator(width, height);
        if (!generator.generateMap(gameState)) {
            std::cerr << "Error: Failed to rebuild dungeon while loading save.\n";
            return false;
        }
    }

    if (!loadedRoomStates.empty()) {
        for (Room& room : gameState.dungeon) {
            for (const LoadedRoomState& savedRoom : loadedRoomStates) {
                if (room.id == savedRoom.roomId) {
                    room.isExplored = savedRoom.isExplored;
                    room.isCleared = savedRoom.isCleared;
                    room.isTrapActive = savedRoom.isTrapActive;
                    room.puzzleSolved = savedRoom.puzzleSolved;
                    break;
                }
            }
        }
    }

    gameState.roomMap.clear();
    for (Room& room : gameState.dungeon) {
        gameState.roomMap[room.id] = &room;
    }
    
    p.isAlive = (p.stats.currentHP > 0);
    
    std::cout << "Game loaded successfully!\n";
    return true;
}

bool FileIO::saveExists(const std::string& filename) const {
    std::ifstream file(filename);
    return file.good();
}

bool FileIO::deleteSave(const std::string& filename) {
    return std::remove(filename.c_str()) == 0;
}

std::vector<std::string> FileIO::listSaveFiles() const {
    std::vector<std::string> saveFiles;

    // Legacy root save support.
    if (saveExists("savegame.txt")) {
        saveFiles.push_back("savegame.txt");
    }

    try {
        namespace fs = std::filesystem;
        fs::path dataDir(dataPath);

        if (fs::exists(dataDir) && fs::is_directory(dataDir)) {
            for (const fs::directory_entry& entry : fs::directory_iterator(dataDir)) {
                if (!entry.is_regular_file()) {
                    continue;
                }

                const std::string name = entry.path().filename().string();
                const std::string lowered = toLower(name);
                const bool isLegacySave = (lowered == "savegame.txt");
                const bool isSlotSave = (lowered.rfind("save_", 0) == 0 &&
                    lowered.size() > 9 &&
                    lowered.substr(lowered.size() - 4) == ".txt");

                if (isLegacySave || isSlotSave) {
                    saveFiles.push_back(entry.path().string());
                }
            }
        }
    } catch (...) {
        // If directory iteration fails, continue with whatever was discovered.
    }

    std::sort(saveFiles.begin(), saveFiles.end());
    saveFiles.erase(std::unique(saveFiles.begin(), saveFiles.end()), saveFiles.end());
    
    return saveFiles;
}

// ---
// GAME LOG
// ---

bool FileIO::saveGameLog(const Player& player, const std::string& filename,
                         EndingType ending) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not create log file.\n";
        return false;
    }
    
    file << "========================================\n";
    file << "   ADVENTURE QUEST ENGINE - GAME LOG\n";
    file << "========================================\n\n";
    
    file << "Date: " << getCurrentDateTime() << "\n\n";
    
    file << "--- PLAYER SUMMARY ---\n";
    file << "Name: " << player.name << "\n";
    file << "Class: " << classToString(player.charClass) << "\n";
    file << "Level: " << player.level << "\n";
    file << "Final HP: " << player.stats.currentHP << "/" << player.stats.maxHP << "\n";
    file << "Gold: " << player.gold << "\n\n";
    
    file << "--- STATISTICS ---\n";
    file << "Total Turns: " << player.totalTurns << "\n";
    file << "Enemies Defeated: " << player.totalKills << "\n";
    file << "Bosses Defeated: " << player.bossesDefeated << "\n";
    file << "Secrets Found: " << player.secretsFound << "\n";
    file << "Puzzles Solved: " << player.puzzlesSolved << "\n";
    file << "Morality Score: " << player.moralityScore << "\n";
    file << "Quests Completed: " << player.completedQuests.size() << "\n\n";
    
    file << "--- ENDING ---\n";
    file << endingToString(ending) << "\n\n";
    
    file << "--- GAME LOG ---\n";
    for (const std::string& entry : player.gameLog) {
        file << "- " << entry << "\n";
    }
    
    file << "\n--- COMPLETED QUESTS ---\n";
    for (const Quest& quest : player.completedQuests) {
        file << "- " << quest.name << "\n";
    }
    
    file << "\n--- FINAL INVENTORY ---\n";
    for (const Item& item : player.inventory) {
        file << "- " << item.name << " x" << item.quantity;
        if (item.isEquipped) file << " [Equipped]";
        file << "\n";
    }
    
    file << "\n========================================\n";
    file << "        FINAL SCORE: " << calculateScore(player) << "\n";
    file << "========================================\n";
    
    file.close();
    
    std::cout << "Game log saved to " << filename << "\n";
    return true;
}

bool FileIO::appendToLog(const std::string& message, const std::string& filename) {
    std::ofstream file(filename, std::ios::app);
    
    if (!file.is_open()) {
        return false;
    }
    
    file << "[" << getCurrentDateTime() << "] " << message << "\n";
    file.close();
    
    return true;
}

// ---
// LEADERBOARD
// ---

std::vector<LeaderboardEntry> FileIO::loadLeaderboard(const std::string& filename) {
    std::vector<LeaderboardEntry> entries;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return entries;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        std::vector<std::string> parts = split(line, '|');
        if (parts.size() >= 5) {
            LeaderboardEntry entry;
            entry.playerName = parts[0];
            entry.score = std::stoi(parts[1]);
            entry.turns = std::stoi(parts[2]);
            entry.characterClass = parts[3];
            entry.ending = parts[4];
            if (parts.size() >= 6) {
                entry.date = parts[5];
            }
            entries.push_back(entry);
        }
    }
    
    file.close();
    return entries;
}

bool FileIO::saveLeaderboard(const std::vector<LeaderboardEntry>& entries,
                             const std::string& filename) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        return false;
    }
    
    file << "# Adventure Quest Engine Leaderboard\n";
    file << "# Format: name|score|turns|class|ending|date\n\n";
    
    for (const LeaderboardEntry& entry : entries) {
        file << entry.playerName << "|"
             << entry.score << "|"
             << entry.turns << "|"
             << entry.characterClass << "|"
             << entry.ending << "|"
             << entry.date << "\n";
    }
    
    file.close();
    return true;
}

int FileIO::addToLeaderboard(const LeaderboardEntry& entry,
                             const std::string& filename,
                             int maxEntries) {
    std::vector<LeaderboardEntry> entries = loadLeaderboard(filename);
    
    // Add new entry
    entries.push_back(entry);
    
    // Sort by score (descending)
    sortLeaderboardByScore(entries);
    
    // Find position
    int position = -1;
    for (size_t i = 0; i < entries.size(); ++i) {
        if (entries[i].playerName == entry.playerName && 
            entries[i].score == entry.score) {
            position = static_cast<int>(i);
            break;
        }
    }
    
    // Trim to max entries
    if (static_cast<int>(entries.size()) > maxEntries) {
        entries.resize(maxEntries);
        
        // Check if our entry was cut
        if (position >= maxEntries) {
            position = -1;
        }
    }
    
    // Save
    saveLeaderboard(entries, filename);
    
    return position;
}

void FileIO::displayLeaderboard(const std::string& filename, int count) {
    std::vector<LeaderboardEntry> entries = loadLeaderboard(filename);
    
    std::cout << "\n";
    displaySeparator(70, '=');
    std::cout << centerString("LEADERBOARD", 70) << "\n";
    displaySeparator(70, '=');
    
    if (entries.empty()) {
        std::cout << "\n  No entries yet. Be the first!\n";
    } else {
        std::cout << "\n";
        std::cout << std::left << std::setw(5) << "Rank"
                  << std::setw(20) << "Name"
                  << std::setw(10) << "Score"
                  << std::setw(10) << "Turns"
                  << std::setw(12) << "Class"
                  << std::setw(15) << "Ending"
                  << "\n";
        displaySeparator(70, '-');
        
        int displayed = 0;
        for (const LeaderboardEntry& entry : entries) {
            if (displayed >= count) break;
            
            std::cout << std::left << std::setw(5) << (displayed + 1)
                      << std::setw(20) << entry.playerName.substr(0, 18)
                      << std::setw(10) << entry.score
                      << std::setw(10) << entry.turns
                      << std::setw(12) << entry.characterClass.substr(0, 10)
                      << std::setw(15) << entry.ending.substr(0, 13)
                      << "\n";
            displayed++;
        }
    }
    
    displaySeparator(70, '=');
}

void FileIO::sortLeaderboardByScore(std::vector<LeaderboardEntry>& entries) {
    // Using selection sort for educational purposes
    int n = static_cast<int>(entries.size());
    
    for (int i = 0; i < n - 1; ++i) {
        int maxIdx = i;
        
        for (int j = i + 1; j < n; ++j) {
            if (entries[j].score > entries[maxIdx].score) {
                maxIdx = j;
            }
        }
        
        if (maxIdx != i) {
            std::swap(entries[i], entries[maxIdx]);
        }
    }
}

void FileIO::sortLeaderboardByTurns(std::vector<LeaderboardEntry>& entries) {
    // Using insertion sort for educational purposes (fewer turns is better)
    int n = static_cast<int>(entries.size());
    
    for (int i = 1; i < n; ++i) {
        LeaderboardEntry key = entries[i];
        int j = i - 1;
        
        while (j >= 0 && entries[j].turns > key.turns) {
            entries[j + 1] = entries[j];
            j--;
        }
        
        entries[j + 1] = key;
    }
}

// ---
// PRIVATE METHODS
// ---

bool FileIO::ensureDataDirectory() {
    // Try to create directory
    MKDIR(dataPath.c_str());
    return true;  // Simplified - assume success
}

bool FileIO::parseKeyValue(const std::string& line, std::string& key, std::string& value) {
    size_t eqPos = line.find('=');
    if (eqPos == std::string::npos) {
        return false;
    }
    
    key = trim(line.substr(0, eqPos));
    value = trim(line.substr(eqPos + 1));
    
    // Remove quotes if present
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        value = value.substr(1, value.size() - 2);
    }
    
    return true;
}

std::string FileIO::getCurrentDateTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// ---
// DEFAULT DATA CREATION
// ---

std::vector<Item> createDefaultItems() {
    std::vector<Item> items;
    
    // Health potions
    Item smallPotion;
    smallPotion.id = 1;
    smallPotion.name = "Minor Health Potion";
    smallPotion.description = "Restores 25 HP.";
    smallPotion.type = ItemType::CONSUMABLE;
    smallPotion.rarity = ItemRarity::COMMON;
    smallPotion.healAmount = 25;
    smallPotion.value = 10;
    smallPotion.isUsable = true;
    smallPotion.isStackable = true;
    items.push_back(smallPotion);
    
    Item medPotion;
    medPotion.id = 2;
    medPotion.name = "Health Potion";
    medPotion.description = "Restores 50 HP.";
    medPotion.type = ItemType::CONSUMABLE;
    medPotion.rarity = ItemRarity::UNCOMMON;
    medPotion.healAmount = 50;
    medPotion.value = 25;
    medPotion.isUsable = true;
    medPotion.isStackable = true;
    items.push_back(medPotion);
    
    Item largePotion;
    largePotion.id = 3;
    largePotion.name = "Greater Health Potion";
    largePotion.description = "Restores 100 HP.";
    largePotion.type = ItemType::CONSUMABLE;
    largePotion.rarity = ItemRarity::RARE;
    largePotion.healAmount = 100;
    largePotion.value = 50;
    largePotion.isUsable = true;
    largePotion.isStackable = true;
    items.push_back(largePotion);
    
    // Weapons
    Item sword;
    sword.id = 10;
    sword.name = "Iron Sword";
    sword.description = "A sturdy iron sword.";
    sword.type = ItemType::WEAPON;
    sword.rarity = ItemRarity::COMMON;
    sword.attackBonus = 2;
    sword.value = 20;
    items.push_back(sword);
    
    Item axe;
    axe.id = 11;
    axe.name = "Battle Axe";
    axe.description = "A heavy battle axe.";
    axe.type = ItemType::WEAPON;
    axe.rarity = ItemRarity::UNCOMMON;
    axe.attackBonus = 12;
    axe.critBonus = 6;
    axe.value = 85;
    items.push_back(axe);
    
    Item staff;
    staff.id = 12;
    staff.name = "Arcane Staff";
    staff.description = "A staff imbued with magic.";
    staff.type = ItemType::WEAPON;
    staff.rarity = ItemRarity::RARE;
    staff.attackBonus = 11;
    staff.critBonus = 14;
    staff.value = 180;
    items.push_back(staff);
    
    Item legendSword;
    legendSword.id = 13;
    legendSword.name = "Dragonslayer";
    legendSword.description = "A legendary blade forged to slay dragons.";
    legendSword.type = ItemType::WEAPON;
    legendSword.rarity = ItemRarity::LEGENDARY;
    legendSword.attackBonus = 22;
    legendSword.critBonus = 18;
    legendSword.value = 800;
    items.push_back(legendSword);

    Item frostmourne;
    frostmourne.id = 14;
    frostmourne.name = "Frostmourne";
    frostmourne.description = "A cursed blade that steals the souls of its victims.";
    frostmourne.type = ItemType::WEAPON;
    frostmourne.rarity = ItemRarity::LEGENDARY;
    frostmourne.attackBonus = 25;
    frostmourne.critBonus = 12;
    frostmourne.value = 1200;
    items.push_back(frostmourne);
    
    // Armor
    Item leather;
    leather.id = 20;
    leather.name = "Leather Armor";
    leather.description = "Basic protective leather armor.";
    leather.type = ItemType::ARMOR;
    leather.rarity = ItemRarity::COMMON;
    leather.defenseBonus = 2;
    leather.value = 20;
    items.push_back(leather);
    
    Item chainmail;
    chainmail.id = 21;
    chainmail.name = "Chainmail";
    chainmail.description = "Interlocking metal rings provide good protection.";
    chainmail.type = ItemType::ARMOR;
    chainmail.rarity = ItemRarity::UNCOMMON;
    chainmail.defenseBonus = 7;
    chainmail.speedBonus = -1;
    chainmail.value = 95;
    items.push_back(chainmail);
    
    Item plate;
    plate.id = 22;
    plate.name = "Plate Armor";
    plate.description = "Heavy plate armor with excellent protection.";
    plate.type = ItemType::ARMOR;
    plate.rarity = ItemRarity::RARE;
    plate.defenseBonus = 13;
    plate.speedBonus = -3;
    plate.value = 240;
    items.push_back(plate);

    Item demonsteel;
    demonsteel.id = 23;
    demonsteel.name = "Demonsteel Armor";
    demonsteel.description = "Armor forged in the bowels of Hell, offering divine defense.";
    demonsteel.type = ItemType::ARMOR;
    demonsteel.rarity = ItemRarity::LEGENDARY;
    demonsteel.defenseBonus = 16;
    demonsteel.speedBonus = -2;
    demonsteel.value = 700;
    items.push_back(demonsteel);

    Item dominationhelm;
    dominationhelm.id = 24;
    dominationhelm.name = "Helm of Domination";
    dominationhelm.description = "A helm that grants control over death itself.";
    dominationhelm.type = ItemType::ARMOR;
    dominationhelm.rarity = ItemRarity::LEGENDARY;
    dominationhelm.defenseBonus = 18;
    dominationhelm.critBonus = 6;
    dominationhelm.value = 1800;
    items.push_back(dominationhelm);
    
    // Accessories
    Item ring;
    ring.id = 30;
    ring.name = "Ring of Swiftness";
    ring.description = "Enchanted with wind magic, grants increased movement speed and reflexes. Particularly favors quick-footed rogues and rangers.";
    ring.type = ItemType::ACCESSORY;
    ring.rarity = ItemRarity::UNCOMMON;
    ring.speedBonus = 6;
    ring.dodgeBonus = 6;
    ring.value = 90;
    items.push_back(ring);
    
    Item amulet;
    amulet.id = 31;
    amulet.name = "Amulet of Fortune";
    amulet.description = "Blessed by the gods of luck, this amulet dramatically increases the likelihood of critical strikes. Worn by legendary assassins.";
    amulet.type = ItemType::ACCESSORY;
    amulet.rarity = ItemRarity::RARE;
    amulet.critBonus = 18;
    amulet.attackBonus = 2;
    amulet.value = 180;
    items.push_back(amulet);
    
    // Keys
    Item bossKey;
    bossKey.id = 40;
    bossKey.name = "Boss Key";
    bossKey.description = "Opens the door to the boss chamber.";
    bossKey.type = ItemType::KEY;
    bossKey.rarity = ItemRarity::RARE;
    bossKey.value = 0;
    bossKey.isStackable = false;
    items.push_back(bossKey);
    
    // Special items
    Item secretItem;
    secretItem.id = 50;
    secretItem.name = "Ancient Artifact";
    secretItem.description = "A mysterious artifact that may lead to a secret ending.";
    secretItem.type = ItemType::SPECIAL;
    secretItem.rarity = ItemRarity::LEGENDARY;
    secretItem.value = 1000;
    secretItem.isStackable = false;
    items.push_back(secretItem);
    
    // Buff items
    Item strengthElixir;
    strengthElixir.id = 60;
    strengthElixir.name = "Elixir of Strength";
    strengthElixir.description = "Temporarily increases attack power.";
    strengthElixir.type = ItemType::CONSUMABLE;
    strengthElixir.rarity = ItemRarity::UNCOMMON;
    strengthElixir.attackBonus = 10;
    strengthElixir.duration = 5;
    strengthElixir.value = 40;
    strengthElixir.isUsable = true;
    items.push_back(strengthElixir);
    
    Item defenseElixir;
    defenseElixir.id = 61;
    defenseElixir.name = "Elixir of Defense";
    defenseElixir.description = "Temporarily increases defense.";
    defenseElixir.type = ItemType::CONSUMABLE;
    defenseElixir.rarity = ItemRarity::UNCOMMON;
    defenseElixir.defenseBonus = 8;
    defenseElixir.duration = 5;
    defenseElixir.value = 40;
    defenseElixir.isUsable = true;
    items.push_back(defenseElixir);
    
    Item speedElixir;
    speedElixir.id = 62;
    speedElixir.name = "Elixir of Agility";
    speedElixir.description = "Temporarily increases speed and dodge chance.";
    speedElixir.type = ItemType::CONSUMABLE;
    speedElixir.rarity = ItemRarity::RARE;
    speedElixir.speedBonus = 7;
    speedElixir.dodgeBonus = 10;
    speedElixir.duration = 5;
    speedElixir.value = 60;
    speedElixir.isUsable = true;
    items.push_back(speedElixir);
    
    // More Weapons - Swords
    Item bronzeSword;
    bronzeSword.id = 15;
    bronzeSword.name = "Bronze Sword";
    bronzeSword.description = "A basic bronze blade for beginners.";
    bronzeSword.type = ItemType::WEAPON;
    bronzeSword.rarity = ItemRarity::COMMON;
    bronzeSword.attackBonus = 1;
    bronzeSword.value = 10;
    items.push_back(bronzeSword);
    
    Item steelSword;
    steelSword.id = 16;
    steelSword.name = "Steel Sword";
    steelSword.description = "A well-crafted steel blade.";
    steelSword.type = ItemType::WEAPON;
    steelSword.rarity = ItemRarity::UNCOMMON;
    steelSword.attackBonus = 12;
    steelSword.critBonus = 4;
    steelSword.value = 110;
    items.push_back(steelSword);
    
    Item mithrilSword;
    mithrilSword.id = 17;
    mithrilSword.name = "Mithril Sword";
    mithrilSword.description = "A lightweight sword of superior craftsmanship.";
    mithrilSword.type = ItemType::WEAPON;
    mithrilSword.rarity = ItemRarity::RARE;
    mithrilSword.attackBonus = 17;
    mithrilSword.critBonus = 10;
    mithrilSword.speedBonus = 3;
    mithrilSword.value = 320;
    items.push_back(mithrilSword);
    
    Item flamingSword;
    flamingSword.id = 18;
    flamingSword.name = "Flaming Longsword";
    flamingSword.description = "A blade wreathed in eternal flames.";
    flamingSword.type = ItemType::WEAPON;
    flamingSword.rarity = ItemRarity::EPIC;
    flamingSword.attackBonus = 20;
    flamingSword.critBonus = 14;
    flamingSword.value = 520;
    items.push_back(flamingSword);
    
    Item vorpalBlade;
    vorpalBlade.id = 19;
    vorpalBlade.name = "Vorpal Blade";
    vorpalBlade.description = "One, two! One, two! And through and through...";
    vorpalBlade.type = ItemType::WEAPON;
    vorpalBlade.rarity = ItemRarity::LEGENDARY;
    vorpalBlade.attackBonus = 23;
    vorpalBlade.critBonus = 22;
    vorpalBlade.value = 1500;
    items.push_back(vorpalBlade);
    
    // Daggers
    Item rustyDagger;
    rustyDagger.id = 70;
    rustyDagger.name = "Rusty Dagger";
    rustyDagger.description = "A worn dagger, still sharp enough.";
    rustyDagger.type = ItemType::WEAPON;
    rustyDagger.rarity = ItemRarity::COMMON;
    rustyDagger.attackBonus = 1;
    rustyDagger.critBonus = 6;
    rustyDagger.speedBonus = 2;
    rustyDagger.value = 5;
    items.push_back(rustyDagger);
    
    Item poisonDagger;
    poisonDagger.id = 71;
    poisonDagger.name = "Poison Dagger";
    poisonDagger.description = "Coated with deadly venom.";
    poisonDagger.type = ItemType::WEAPON;
    poisonDagger.rarity = ItemRarity::UNCOMMON;
    poisonDagger.attackBonus = 6;
    poisonDagger.critBonus = 16;
    poisonDagger.speedBonus = 5;
    poisonDagger.value = 105;
    items.push_back(poisonDagger);
    
    Item shadowDagger;
    shadowDagger.id = 72;
    shadowDagger.name = "Shadow Fang";
    shadowDagger.description = "Forged in darkness, strikes from nowhere.";
    shadowDagger.type = ItemType::WEAPON;
    shadowDagger.rarity = ItemRarity::RARE;
    shadowDagger.attackBonus = 10;
    shadowDagger.critBonus = 28;
    shadowDagger.speedBonus = 7;
    shadowDagger.dodgeBonus = 6;
    shadowDagger.value = 280;
    items.push_back(shadowDagger);
    
    // Axes
    Item woodcutterAxe;
    woodcutterAxe.id = 80;
    woodcutterAxe.name = "Woodcutter's Axe";
    woodcutterAxe.description = "Better for trees than combat, but it'll do.";
    woodcutterAxe.type = ItemType::WEAPON;
    woodcutterAxe.rarity = ItemRarity::COMMON;
    woodcutterAxe.attackBonus = 3;
    woodcutterAxe.critBonus = 2;
    woodcutterAxe.value = 15;
    items.push_back(woodcutterAxe);
    
    Item warAxe;
    warAxe.id = 81;
    warAxe.name = "War Axe";
    warAxe.description = "A brutal weapon designed for war.";
    warAxe.type = ItemType::WEAPON;
    warAxe.rarity = ItemRarity::UNCOMMON;
    warAxe.attackBonus = 13;
    warAxe.critBonus = 7;
    warAxe.speedBonus = -1;
    warAxe.value = 115;
    items.push_back(warAxe);
    
    Item greatAxe;
    greatAxe.id = 82;
    greatAxe.name = "Executioner's Greataxe";
    greatAxe.description = "A massive axe that cleaves through armor.";
    greatAxe.type = ItemType::WEAPON;
    greatAxe.rarity = ItemRarity::RARE;
    greatAxe.attackBonus = 21;
    greatAxe.critBonus = 16;
    greatAxe.speedBonus = -3;
    greatAxe.value = 380;
    items.push_back(greatAxe);
    
    // Spears
    Item woodenSpear;
    woodenSpear.id = 85;
    woodenSpear.name = "Wooden Spear";
    woodenSpear.description = "A simple spear with reach advantage.";
    woodenSpear.type = ItemType::WEAPON;
    woodenSpear.rarity = ItemRarity::COMMON;
    woodenSpear.attackBonus = 3;
    woodenSpear.value = 10;
    items.push_back(woodenSpear);
    
    Item pike;
    pike.id = 86;
    pike.name = "Steel Pike";
    pike.description = "A long pike for keeping enemies at bay.";
    pike.type = ItemType::WEAPON;
    pike.rarity = ItemRarity::UNCOMMON;
    pike.attackBonus = 11;
    pike.defenseBonus = 3;
    pike.value = 95;
    items.push_back(pike);
    
    Item halberd;
    halberd.id = 87;
    halberd.name = "Halberd";
    halberd.description = "A versatile polearm weapon.";
    halberd.type = ItemType::WEAPON;
    halberd.rarity = ItemRarity::RARE;
    halberd.attackBonus = 16;
    halberd.defenseBonus = 5;
    halberd.critBonus = 6;
    halberd.value = 280;
    items.push_back(halberd);
    
    // Staves and Wands
    Item apprenticeStaff;
    apprenticeStaff.id = 90;
    apprenticeStaff.name = "Apprentice's Staff";
    apprenticeStaff.description = "A basic staff for novice mages.";
    apprenticeStaff.type = ItemType::WEAPON;
    apprenticeStaff.rarity = ItemRarity::COMMON;
    apprenticeStaff.attackBonus = 2;
    apprenticeStaff.critBonus = 8;
    apprenticeStaff.value = 20;
    items.push_back(apprenticeStaff);
    
    Item fireStaff;
    fireStaff.id = 91;
    fireStaff.name = "Staff of Flames";
    fireStaff.description = "Channels the power of fire.";
    fireStaff.type = ItemType::WEAPON;
    fireStaff.rarity = ItemRarity::UNCOMMON;
    fireStaff.attackBonus = 9;
    fireStaff.critBonus = 13;
    fireStaff.value = 135;
    items.push_back(fireStaff);
    
    Item frostStaff;
    frostStaff.id = 92;
    frostStaff.name = "Staff of Winter";
    frostStaff.description = "Freezes enemies with icy magic.";
    frostStaff.type = ItemType::WEAPON;
    frostStaff.rarity = ItemRarity::RARE;
    frostStaff.attackBonus = 12;
    frostStaff.critBonus = 16;
    frostStaff.speedBonus = 2;
    frostStaff.value = 300;
    items.push_back(frostStaff);
    
    Item archmagusStaff;
    archmagusStaff.id = 93;
    archmagusStaff.name = "Archmagus Staff";
    archmagusStaff.description = "The staff of a grand wizard.";
    archmagusStaff.type = ItemType::WEAPON;
    archmagusStaff.rarity = ItemRarity::EPIC;
    archmagusStaff.attackBonus = 14;
    archmagusStaff.critBonus = 20;
    archmagusStaff.defenseBonus = 4;
    archmagusStaff.value = 600;
    items.push_back(archmagusStaff);
    
    // Armor Sets - Light
    Item clothRobe;
    clothRobe.id = 100;
    clothRobe.name = "Cloth Robe";
    clothRobe.description = "Simple cloth offering minimal protection.";
    clothRobe.type = ItemType::ARMOR;
    clothRobe.rarity = ItemRarity::COMMON;
    clothRobe.defenseBonus = 1;
    clothRobe.value = 10;
    items.push_back(clothRobe);
    
    Item paddedArmor;
    paddedArmor.id = 101;
    paddedArmor.name = "Padded Armor";
    paddedArmor.description = "Quilted layers provide basic protection.";
    paddedArmor.type = ItemType::ARMOR;
    paddedArmor.rarity = ItemRarity::COMMON;
    paddedArmor.defenseBonus = 2;
    paddedArmor.value = 18;
    items.push_back(paddedArmor);
    
    Item hideArmor;
    hideArmor.id = 102;
    hideArmor.name = "Hide Armor";
    hideArmor.description = "Tough animal hide offers good flexibility.";
    hideArmor.type = ItemType::ARMOR;
    hideArmor.rarity = ItemRarity::COMMON;
    hideArmor.defenseBonus = 3;
    hideArmor.value = 28;
    items.push_back(hideArmor);
    
    Item studdedLeather;
    studdedLeather.id = 103;
    studdedLeather.name = "Studded Leather Armor";
    studdedLeather.description = "Leather reinforced with metal studs.";
    studdedLeather.type = ItemType::ARMOR;
    studdedLeather.rarity = ItemRarity::UNCOMMON;
    studdedLeather.defenseBonus = 6;
    studdedLeather.dodgeBonus = 4;
    studdedLeather.value = 85;
    items.push_back(studdedLeather);
    
    // Armor Sets - Medium
    Item scalemail;
    scalemail.id = 104;
    scalemail.name = "Scale Mail";
    scalemail.description = "Overlapping metal scales.";
    scalemail.type = ItemType::ARMOR;
    scalemail.rarity = ItemRarity::UNCOMMON;
    scalemail.defenseBonus = 8;
    scalemail.speedBonus = -1;
    scalemail.value = 120;
    items.push_back(scalemail);
    
    Item breastplate;
    breastplate.id = 105;
    breastplate.name = "Breastplate";
    breastplate.description = "Solid chest protection.";
    breastplate.type = ItemType::ARMOR;
    breastplate.rarity = ItemRarity::UNCOMMON;
    breastplate.defenseBonus = 9;
    breastplate.speedBonus = -1;
    breastplate.value = 135;
    items.push_back(breastplate);
    
    Item halfPlate;
    halfPlate.id = 106;
    halfPlate.name = "Half Plate";
    halfPlate.description = "Partial plate armor with good mobility.";
    halfPlate.type = ItemType::ARMOR;
    halfPlate.rarity = ItemRarity::RARE;
    halfPlate.defenseBonus = 11;
    halfPlate.speedBonus = -2;
    halfPlate.value = 220;
    items.push_back(halfPlate);
    
    // Armor Sets - Heavy
    Item bandedArmor;
    bandedArmor.id = 107;
    bandedArmor.name = "Banded Mail";
    bandedArmor.description = "Strips of metal bound together.";
    bandedArmor.type = ItemType::ARMOR;
    bandedArmor.rarity = ItemRarity::UNCOMMON;
    bandedArmor.defenseBonus = 10;
    bandedArmor.speedBonus = -2;
    bandedArmor.value = 150;
    items.push_back(bandedArmor);
    
    Item splintArmor;
    splintArmor.id = 108;
    splintArmor.name = "Splint Armor";
    splintArmor.description = "Vertical metal strips over leather.";
    splintArmor.type = ItemType::ARMOR;
    splintArmor.rarity = ItemRarity::RARE;
    splintArmor.defenseBonus = 12;
    splintArmor.speedBonus = -2;
    splintArmor.value = 260;
    items.push_back(splintArmor);
    
    Item fullPlate;
    fullPlate.id = 109;
    fullPlate.name = "Full Plate Armor";
    fullPlate.description = "Complete heavy plate protection.";
    fullPlate.type = ItemType::ARMOR;
    fullPlate.rarity = ItemRarity::RARE;
    fullPlate.defenseBonus = 14;
    fullPlate.speedBonus = -4;
    fullPlate.value = 420;
    items.push_back(fullPlate);
    
    Item dragonscale;
    dragonscale.id = 110;
    dragonscale.name = "Dragonscale Armor";
    dragonscale.description = "Crafted from genuine dragon scales.";
    dragonscale.type = ItemType::ARMOR;
    dragonscale.rarity = ItemRarity::EPIC;
    dragonscale.defenseBonus = 17;
    dragonscale.dodgeBonus = 6;
    dragonscale.speedBonus = -1;
    dragonscale.value = 800;
    items.push_back(dragonscale);
    
    Item celestialArmor;
    celestialArmor.id = 111;
    celestialArmor.name = "Celestial Armor";
    celestialArmor.description = "Blessed armor from the heavens.";
    celestialArmor.type = ItemType::ARMOR;
    celestialArmor.rarity = ItemRarity::LEGENDARY;
    celestialArmor.defenseBonus = 20;
    celestialArmor.dodgeBonus = 10;
    celestialArmor.speedBonus = 2;
    celestialArmor.value = 2000;
    items.push_back(celestialArmor);
    
    // Shields
    Item woodenShield;
    woodenShield.id = 120;
    woodenShield.name = "Wooden Shield";
    woodenShield.description = "A basic wooden shield.";
    woodenShield.type = ItemType::ARMOR;
    woodenShield.rarity = ItemRarity::COMMON;
    woodenShield.defenseBonus = 1;
    woodenShield.value = 12;
    items.push_back(woodenShield);
    
    Item steelShield;
    steelShield.id = 121;
    steelShield.name = "Steel Shield";
    steelShield.description = "Sturdy metal shield.";
    steelShield.type = ItemType::ARMOR;
    steelShield.rarity = ItemRarity::UNCOMMON;
    steelShield.defenseBonus = 5;
    steelShield.value = 75;
    items.push_back(steelShield);
    
    Item towerShield;
    towerShield.id = 122;
    towerShield.name = "Tower Shield";
    towerShield.description = "Massive shield offering great protection.";
    towerShield.type = ItemType::ARMOR;
    towerShield.rarity = ItemRarity::RARE;
    towerShield.defenseBonus = 8;
    towerShield.speedBonus = -2;
    towerShield.value = 200;
    items.push_back(towerShield);
    
    // More Accessories
    Item strengthRing;
    strengthRing.id = 130;
    strengthRing.name = "Ring of Might";
    strengthRing.description = "Forged in the furnaces of mountain dwarves, this ring channels raw physical strength into every strike. Warriors favor this precious artifact.";
    strengthRing.type = ItemType::ACCESSORY;
    strengthRing.rarity = ItemRarity::UNCOMMON;
    strengthRing.attackBonus = 8;
    strengthRing.critBonus = 3;
    strengthRing.value = 110;
    items.push_back(strengthRing);
    
    Item defenseRing;
    defenseRing.id = 131;
    defenseRing.name = "Ring of Protection";
    defenseRing.description = "A protective charm that shields the wearer from harm. Crafted by ancient mages to withstand the fiercest blows.";
    defenseRing.type = ItemType::ACCESSORY;
    defenseRing.rarity = ItemRarity::UNCOMMON;
    defenseRing.defenseBonus = 6;
    defenseRing.dodgeBonus = 2;
    defenseRing.value = 100;
    items.push_back(defenseRing);
    
    Item vitalityAmulet;
    vitalityAmulet.id = 132;
    vitalityAmulet.name = "Amulet of Vitality";
    vitalityAmulet.description = "Pulses with life energy, enhancing the wearer's constitution and resilience. Grants both defensive prowess and regenerative vitality.";
    vitalityAmulet.type = ItemType::ACCESSORY;
    vitalityAmulet.rarity = ItemRarity::RARE;
    vitalityAmulet.defenseBonus = 5;
    vitalityAmulet.dodgeBonus = 3;
    vitalityAmulet.value = 170;
    items.push_back(vitalityAmulet);
    
    Item shadowCloak;
    shadowCloak.id = 133;
    shadowCloak.name = "Cloak of Shadows";
    shadowCloak.description = "Woven from the essence of twilight itself, this cloak allows the wearer to slip between shadows. Masters of stealth seek this treasure.";
    shadowCloak.type = ItemType::ACCESSORY;
    shadowCloak.rarity = ItemRarity::RARE;
    shadowCloak.dodgeBonus = 14;
    shadowCloak.speedBonus = 4;
    shadowCloak.defenseBonus = 2;
    shadowCloak.value = 220;
    items.push_back(shadowCloak);
    
    Item dragonHeart;
    dragonHeart.id = 134;
    dragonHeart.name = "Heart of the Dragon";
    dragonHeart.description = "The crystallized heart of an ancient dragon, containing immense power. Grants both offensive fury and defensive scales to the wielder.";
    dragonHeart.type = ItemType::ACCESSORY;
    dragonHeart.rarity = ItemRarity::EPIC;
    dragonHeart.attackBonus = 12;
    dragonHeart.defenseBonus = 8;
    dragonHeart.critBonus = 12;
    dragonHeart.speedBonus = -1;
    dragonHeart.value = 650;
    items.push_back(dragonHeart);
    
    Item phoenixFeather;
    phoenixFeather.id = 135;
    phoenixFeather.name = "Phoenix Feather";
    phoenixFeather.description = "A legendary feather from the eternal phoenix, granting near-immortal evasion and rapid regeneration. The ultimate accessory of champions.";
    phoenixFeather.type = ItemType::ACCESSORY;
    phoenixFeather.rarity = ItemRarity::LEGENDARY;
    phoenixFeather.defenseBonus = 8;
    phoenixFeather.dodgeBonus = 20;
    phoenixFeather.speedBonus = 3;
    phoenixFeather.value = 1200;
    items.push_back(phoenixFeather);
    
    // More specialized accessories
    Item ironSkinRing;
    ironSkinRing.id = 136;
    ironSkinRing.name = "Ring of Iron Skin";
    ironSkinRing.description = "Hardens the wearer's flesh to the strength of iron, providing exceptional defense at the cost of minor agility.";
    ironSkinRing.type = ItemType::ACCESSORY;
    ironSkinRing.rarity = ItemRarity::UNCOMMON;
    ironSkinRing.defenseBonus = 7;
    ironSkinRing.speedBonus = -2;
    ironSkinRing.value = 95;
    items.push_back(ironSkinRing);
    
    Item assassinRing;
    assassinRing.id = 137;
    assassinRing.name = "Assassin's Ring";
    assassinRing.description = "The hidden weapon of shadow guilds. Amplifies critical damage and speed for the perfect strike.";
    assassinRing.type = ItemType::ACCESSORY;
    assassinRing.rarity = ItemRarity::RARE;
    assassinRing.critBonus = 12;
    assassinRing.speedBonus = 5;
    assassinRing.attackBonus = 4;
    assassinRing.value = 200;
    items.push_back(assassinRing);
    
    Item wisdomAmulet;
    wisdomAmulet.id = 138;
    wisdomAmulet.name = "Amulet of Wisdom";
    wisdomAmulet.description = "Blessed by ancient scholars, this amulet provides balanced enhancements to both offense and defense through tactical insight.";
    wisdomAmulet.type = ItemType::ACCESSORY;
    wisdomAmulet.rarity = ItemRarity::RARE;
    wisdomAmulet.attackBonus = 5;
    wisdomAmulet.defenseBonus = 5;
    wisdomAmulet.critBonus = 5;
    wisdomAmulet.value = 210;
    items.push_back(wisdomAmulet);
    
    Item vileBlackHand;
    vileBlackHand.id = 139;
    vileBlackHand.name = "Amulet of the Vile Black Hand";
    vileBlackHand.description = "An artifact of dark power, granting tremendous attack at the cost of reduced defense. For the bold and aggressive.";
    vileBlackHand.type = ItemType::ACCESSORY;
    vileBlackHand.rarity = ItemRarity::EPIC;
    vileBlackHand.attackBonus = 15;
    vileBlackHand.critBonus = 8;
    vileBlackHand.defenseBonus = -3;
    vileBlackHand.value = 550;
    items.push_back(vileBlackHand);
    
    Item silverPendant;
    silverPendant.id = 145;
    silverPendant.name = "Silver Pendant of Healing";
    silverPendant.description = "Infused with holy silver, this pendant provides steady defensive protection and minor healing properties.";
    silverPendant.type = ItemType::ACCESSORY;
    silverPendant.rarity = ItemRarity::UNCOMMON;
    silverPendant.defenseBonus = 4;
    silverPendant.dodgeBonus = 4;
    silverPendant.value = 105;
    items.push_back(silverPendant);
    
    // Advanced Consumables
    Item supremePotion;
    supremePotion.id = 4;
    supremePotion.name = "Supreme Health Potion";
    supremePotion.description = "Restores 200 HP.";
    supremePotion.type = ItemType::CONSUMABLE;
    supremePotion.rarity = ItemRarity::EPIC;
    supremePotion.healAmount = 200;
    supremePotion.value = 100;
    supremePotion.isUsable = true;
    supremePotion.isStackable = true;
    items.push_back(supremePotion);
    
    Item ultimatePotion;
    ultimatePotion.id = 5;
    ultimatePotion.name = "Ultimate Healing Elixir";
    ultimatePotion.description = "Fully restores HP.";
    ultimatePotion.type = ItemType::CONSUMABLE;
    ultimatePotion.rarity = ItemRarity::LEGENDARY;
    // Make ultimate potion powerful but not trivialize the game
    ultimatePotion.healAmount = 999;
    ultimatePotion.value = 250;
    ultimatePotion.isUsable = true;
    ultimatePotion.isStackable = true;
    items.push_back(ultimatePotion);
    
    Item resurrectionScroll;
    resurrectionScroll.id = 140;
    resurrectionScroll.name = "Scroll of Resurrection";
    resurrectionScroll.description = "Brings you back from death once.";
    resurrectionScroll.type = ItemType::CONSUMABLE;
    resurrectionScroll.rarity = ItemRarity::LEGENDARY;
    resurrectionScroll.value = 500;
    resurrectionScroll.isUsable = true;
    resurrectionScroll.isStackable = false;
    items.push_back(resurrectionScroll);
    
    Item berserkPotion;
    berserkPotion.id = 141;
    berserkPotion.name = "Potion of Berserker Rage";
    berserkPotion.description = "Massive attack boost, reduced defense.";
    berserkPotion.type = ItemType::CONSUMABLE;
    berserkPotion.rarity = ItemRarity::RARE;
    berserkPotion.attackBonus = 20;
    berserkPotion.defenseBonus = -5;
    berserkPotion.duration = 3;
    berserkPotion.value = 80;
    berserkPotion.isUsable = true;
    items.push_back(berserkPotion);
    
    Item invisPotion;
    invisPotion.id = 142;
    invisPotion.name = "Potion of Invisibility";
    invisPotion.description = "Become invisible to enemies.";
    invisPotion.type = ItemType::CONSUMABLE;
    invisPotion.rarity = ItemRarity::EPIC;
    invisPotion.dodgeBonus = 50;
    invisPotion.duration = 2;
    invisPotion.value = 150;
    invisPotion.isUsable = true;
    items.push_back(invisPotion);

    for (Item& item : items) {
        if (item.type == ItemType::WEAPON ||
            item.type == ItemType::ARMOR ||
            item.type == ItemType::ACCESSORY) {
            item.isUsable = false;
            item.isStackable = false;
        }
    }
    
    return items;
}

std::vector<Enemy> createDefaultEnemies() {
    std::vector<Enemy> enemies;
    
    // Basic enemies
    Enemy skeleton;
    skeleton.id = 1;
    skeleton.name = "Skeleton";
    skeleton.description = "A reanimated skeleton warrior.";
    skeleton.dialogue = "Clatter clatter...";
    skeleton.stats.maxHP = 25;
    skeleton.stats.currentHP = 25;
    skeleton.stats.attack = 6;
    skeleton.stats.defense = 2;
    skeleton.stats.critChance = 5;
    skeleton.stats.dodgeChance = 8;
    skeleton.stats.speed = 8;
    skeleton.experienceReward = 25;
    skeleton.goldReward = 10;
    skeleton.lootTable = {1, 10};  // Potion, Sword
    skeleton.lootChances = {30, 10};
    enemies.push_back(skeleton);
    
    Enemy goblin;
    goblin.id = 2;
    goblin.name = "Goblin";
    goblin.description = "A sneaky green-skinned goblin.";
    goblin.dialogue = "Gimme your shinies!";
    goblin.stats.maxHP = 20;
    goblin.stats.currentHP = 20;
    goblin.stats.attack = 5;
    goblin.stats.defense = 1;
    goblin.stats.critChance = 12;
    goblin.stats.dodgeChance = 18;
    goblin.stats.speed = 12;
    goblin.experienceReward = 20;
    goblin.goldReward = 15;
    goblin.lootTable = {1};
    goblin.lootChances = {40};
    enemies.push_back(goblin);
    
    Enemy orc;
    orc.id = 3;
    orc.name = "Orc Warrior";
    orc.description = "A brutish orc with a massive club.";
    orc.dialogue = "ORC SMASH!";
    orc.stats.maxHP = 45;
    orc.stats.currentHP = 45;
    orc.stats.attack = 9;
    orc.stats.defense = 4;
    orc.stats.critChance = 8;
    orc.stats.dodgeChance = 4;
    orc.stats.speed = 6;
    orc.experienceReward = 40;
    orc.goldReward = 20;
    orc.lootTable = {2, 11};
    orc.lootChances = {25, 15};
    enemies.push_back(orc);

    // New basic enemies
    Enemy slime;
    slime.id = 6;
    slime.name = "Green Slime";
    slime.description = "A gelatinous blob that wobbles menacingly.";
    slime.dialogue = "*Squish squish*";
    slime.stats.maxHP = 15;
    slime.stats.currentHP = 15;
    slime.stats.attack = 4;
    slime.stats.defense = 0;
    slime.stats.critChance = 2;
    slime.stats.dodgeChance = 30;
    slime.stats.speed = 5;
    slime.experienceReward = 15;
    slime.goldReward = 5;
    slime.lootTable = {1};
    slime.lootChances = {20};
    enemies.push_back(slime);

    Enemy spider;
    spider.id = 7;
    spider.name = "Giant Spider";
    spider.description = "A multi-eyed horror with dripping fangs.";
    spider.dialogue = "*Chittering sounds*";
    spider.stats.maxHP = 30;
    spider.stats.currentHP = 30;
    spider.stats.attack = 7;
    spider.stats.defense = 2;
    spider.stats.critChance = 10;
    spider.stats.dodgeChance = 15;
    spider.stats.speed = 16;
    spider.experienceReward = 35;
    spider.goldReward = 12;
    
    EnemyAbility venom;
    venom.name = "Venomous Bite";
    venom.description = "A bite that deals extra damage.";
    venom.damage = 15;
    venom.cooldown = 3;
    venom.triggerChance = 0.3f;
    spider.abilities.push_back(venom);
    enemies.push_back(spider);

    Enemy darkKnight;
    darkKnight.id = 8;
    darkKnight.name = "Dark Knight";
    darkKnight.description = "A fallen warrior in black plate armor.";
    darkKnight.dialogue = "None shall pass!";
    darkKnight.stats.maxHP = 60;
    darkKnight.stats.currentHP = 60;
    darkKnight.stats.attack = 12;
    darkKnight.stats.defense = 8;
    darkKnight.stats.critChance = 10;
    darkKnight.stats.dodgeChance = 2;
    darkKnight.stats.speed = 7;
    darkKnight.experienceReward = 75;
    darkKnight.goldReward = 40;
    darkKnight.lootTable = {16, 21}; // Steel Sword, Chainmail
    darkKnight.lootChances = {20, 15};
    enemies.push_back(darkKnight);

    Enemy mimic;
    mimic.id = 9;
    mimic.name = "Mimic";
    mimic.description = "A treasure chest that grew teeth.";
    mimic.dialogue = "Surprise!";
    mimic.stats.maxHP = 40;
    mimic.stats.currentHP = 40;
    mimic.stats.attack = 10;
    mimic.stats.defense = 5;
    mimic.stats.critChance = 20;
    mimic.stats.dodgeChance = 5;
    mimic.stats.speed = 10;
    mimic.experienceReward = 100;
    mimic.goldReward = 150;
    mimic.lootTable = {3, 130}; // Greater Potion, Ring of Might
    mimic.lootChances = {50, 30};
    enemies.push_back(mimic);

    Enemy vampire;
    vampire.id = 10;
    vampire.name = "Vampire Noble";
    vampire.description = "An elegant but deadly bloodsucker.";
    vampire.dialogue = "Your blood smells... exquisite.";
    vampire.stats.maxHP = 55;
    vampire.stats.currentHP = 55;
    vampire.stats.attack = 10;
    vampire.stats.defense = 4;
    vampire.stats.critChance = 15;
    vampire.stats.dodgeChance = 20;
    vampire.stats.speed = 15;
    vampire.experienceReward = 90;
    vampire.goldReward = 60;

    EnemyAbility bloodSuck;
    bloodSuck.name = "Blood Drain";
    bloodSuck.description = "Drains life from the victim.";
    bloodSuck.damage = 14;
    bloodSuck.heals = true;
    bloodSuck.healAmount = 8;
    bloodSuck.cooldown = 3;
    bloodSuck.triggerChance = 0.4f;
    vampire.abilities.push_back(bloodSuck);
    enemies.push_back(vampire);
    
    Enemy wraith;
    wraith.id = 4;
    wraith.name = "Wraith";
    wraith.description = "A ghostly apparition that drains life.";
    wraith.dialogue = "Join usss...";
    wraith.stats.maxHP = 32;
    wraith.stats.currentHP = 32;
    wraith.stats.attack = 8;
    wraith.stats.defense = 1;
    wraith.stats.critChance = 15;
    wraith.stats.dodgeChance = 25;
    wraith.stats.speed = 14;
    wraith.experienceReward = 50;
    wraith.goldReward = 25;
    
    EnemyAbility lifeDrain;
    lifeDrain.name = "Life Drain";
    lifeDrain.description = "Drains health from the target.";
    lifeDrain.damage = 12;
    lifeDrain.heals = true;
    lifeDrain.healAmount = 5;
    // Slightly increase enemy ability cooldowns to make skill pacing more punishing
    lifeDrain.cooldown = 4;
    lifeDrain.triggerChance = 0.25f;
    wraith.abilities.push_back(lifeDrain);
    
    enemies.push_back(wraith);
    
    Enemy troll;
    troll.id = 5;
    troll.name = "Cave Troll";
    troll.description = "A massive troll that regenerates health.";
    troll.dialogue = "RAAAWR!";
    troll.stats.maxHP = 65;
    troll.stats.currentHP = 65;
    troll.stats.attack = 11;
    troll.stats.defense = 6;
    troll.stats.critChance = 5;
    troll.stats.dodgeChance = 3;
    troll.stats.speed = 4;
    troll.experienceReward = 60;
    troll.goldReward = 30;
    
    EnemyAbility regen;
    regen.name = "Regeneration";
    regen.description = "The troll regenerates health.";
    regen.heals = true;
    regen.healAmount = 7;
    regen.cooldown = 4;
    regen.triggerChance = 0.4f;
    troll.abilities.push_back(regen);
    
    enemies.push_back(troll);
    
    // Boss enemies
    Enemy dragon;
    dragon.id = 100;
    dragon.name = "Ancient Dragon";
    dragon.description = "A terrifying ancient dragon guards the exit.";
    dragon.dialogue = "You dare challenge me, mortal?!";
    dragon.stats.maxHP = 180;
    dragon.stats.currentHP = 180;
    dragon.stats.attack = 16;
    dragon.stats.defense = 10;
    dragon.stats.critChance = 12;
    dragon.stats.dodgeChance = 8;
    dragon.stats.speed = 12;
    dragon.experienceReward = 500;
    dragon.goldReward = 200;
    dragon.isBoss = true;
    dragon.lootTable = {13, 50};  // Dragonslayer, Artifact
    dragon.lootChances = {50, 25};
    
    EnemyAbility fireBreath;
    fireBreath.name = "Fire Breath";
    fireBreath.description = "The dragon breathes devastating fire!";
    fireBreath.damage = 24;
    fireBreath.cooldown = 5;
    fireBreath.triggerChance = 0.35f;
    dragon.abilities.push_back(fireBreath);
    
    enemies.push_back(dragon);
    
    Enemy lich;
    lich.id = 101;
    lich.name = "The Lich King";
    lich.description = "An undead sorcerer of immense power.";
    lich.dialogue = "Death is only the beginning!";
    lich.stats.maxHP = 100;
    lich.stats.currentHP = 100;
    lich.stats.attack = 8;
    lich.stats.defense = 5;
    lich.stats.critChance = 5;
    lich.stats.dodgeChance = 5;
    lich.stats.speed = 10;
    lich.experienceReward = 400;
    lich.goldReward = 150;
    lich.isBoss = true;
    
    EnemyAbility darkBolt;
    darkBolt.name = "Dark Bolt";
    darkBolt.description = "A bolt of dark energy!";
    darkBolt.damage = 18;
    darkBolt.cooldown = 4;
    darkBolt.triggerChance = 0.30f;
    lich.abilities.push_back(darkBolt);
    
    EnemyAbility summon;
    summon.name = "Raise Dead";
    summon.description = "The Lich heals itself with dark magic.";
    summon.heals = true;
    summon.healAmount = 12;
    summon.cooldown = 6;
    summon.triggerChance = 0.20f;
    lich.abilities.push_back(summon);
    
    enemies.push_back(lich);

    Enemy beholder;
    beholder.id = 102;
    beholder.name = "Eye Tyrant";
    beholder.description = "A floating orb of eyes and madness.";
    beholder.dialogue = "I see everything!";
    beholder.stats.maxHP = 140;
    beholder.stats.currentHP = 140;
    beholder.stats.attack = 14;
    beholder.stats.defense = 6;
    beholder.stats.critChance = 20;
    beholder.stats.dodgeChance = 15;
    beholder.stats.speed = 11;
    beholder.experienceReward = 450;
    beholder.goldReward = 300;
    beholder.isBoss = true;

    EnemyAbility disintegration;
    disintegration.name = "Disintegration Ray";
    disintegration.description = "A beam of pure destructive energy!";
    disintegration.damage = 35;
    disintegration.cooldown = 6;
    disintegration.triggerChance = 0.25f;
    beholder.abilities.push_back(disintegration);

    EnemyAbility sleepRay;
    sleepRay.name = "Sleep Ray";
    sleepRay.description = "The beholder's eye makes you drowsy.";
    sleepRay.damage = 5;
    sleepRay.cooldown = 4;
    sleepRay.triggerChance = 0.3f;
    beholder.abilities.push_back(sleepRay);

    enemies.push_back(beholder);
    
    return enemies;
}

std::vector<Room> createDefaultRooms() {
    // This returns template rooms for procedural generation to draw from
    // The actual dungeon rooms are generated by MapGenerator
    std::vector<Room> rooms;
    
    Room entrance;
    entrance.id = 0;
    entrance.name = "Dungeon Entrance";
    entrance.description = "The entrance to a dark and foreboding dungeon.";
    entrance.type = RoomType::ENTRANCE;
    rooms.push_back(entrance);
    
    Room exitRoom;
    exitRoom.id = 1;
    exitRoom.name = "Exit Chamber";
    exitRoom.description = "A grand chamber with a portal leading to freedom.";
    exitRoom.type = RoomType::EXIT;
    rooms.push_back(exitRoom);
    
    return rooms;
}

void createDefaultDataFiles(const std::string& dataPath) {
    // Create items.txt
    std::ofstream itemsFile(dataPath + "items.txt");
    if (itemsFile.is_open()) {
        itemsFile << "# Adventure Quest Engine - Items Data File\n";
        itemsFile << "# Format: [ITEM] section with key=value pairs\n\n";
        
        itemsFile << "[ITEM]\n";
        itemsFile << "id=1\n";
        itemsFile << "name=Minor Health Potion\n";
        itemsFile << "description=Restores 25 HP.\n";
        itemsFile << "type=consumable\n";
        itemsFile << "rarity=common\n";
        itemsFile << "heal=25\n";
        itemsFile << "value=10\n";
        itemsFile << "stackable=true\n";
        itemsFile << "usable=true\n\n";
        
        itemsFile << "[ITEM]\n";
        itemsFile << "id=10\n";
        itemsFile << "name=Iron Sword\n";
        itemsFile << "description=A sturdy iron sword.\n";
        itemsFile << "type=weapon\n";
        itemsFile << "rarity=common\n";
        itemsFile << "attack=5\n";
        itemsFile << "value=30\n";
        
        itemsFile.close();
    }
    
    // Create enemies.txt
    std::ofstream enemiesFile(dataPath + "enemies.txt");
    if (enemiesFile.is_open()) {
        enemiesFile << "# Adventure Quest Engine - Enemies Data File\n\n";
        
        enemiesFile << "[ENEMY]\n";
        enemiesFile << "id=1\n";
        enemiesFile << "name=Skeleton\n";
        enemiesFile << "description=A reanimated skeleton warrior.\n";
        enemiesFile << "dialogue=Clatter clatter...\n";
        enemiesFile << "hp=40\n";
        enemiesFile << "attack=8\n";
        enemiesFile << "defense=3\n";
        enemiesFile << "crit=5\n";
        enemiesFile << "dodge=10\n";
        enemiesFile << "speed=8\n";
        enemiesFile << "experience=25\n";
        enemiesFile << "gold=10\n";
        enemiesFile << "boss=false\n\n";

        enemiesFile << "[ENEMY]\n";
        enemiesFile << "id=2\n";
        enemiesFile << "name=Goblin\n";
        enemiesFile << "description=A sneaky green-skinned goblin.\n";
        enemiesFile << "dialogue=Gimme your shinies!\n";
        enemiesFile << "hp=20\n";
        enemiesFile << "attack=5\n";
        enemiesFile << "defense=1\n";
        enemiesFile << "crit=12\n";
        enemiesFile << "dodge=18\n";
        enemiesFile << "speed=12\n";
        enemiesFile << "experience=20\n";
        enemiesFile << "gold=15\n";
        enemiesFile << "boss=false\n\n";

        enemiesFile << "[ENEMY]\n";
        enemiesFile << "id=6\n";
        enemiesFile << "name=Green Slime\n";
        enemiesFile << "description=A gelatinous blob that wobbles menacingly.\n";
        enemiesFile << "dialogue=*Squish squish*\n";
        enemiesFile << "hp=15\n";
        enemiesFile << "attack=4\n";
        enemiesFile << "defense=0\n";
        enemiesFile << "crit=2\n";
        enemiesFile << "dodge=30\n";
        enemiesFile << "speed=5\n";
        enemiesFile << "experience=15\n";
        enemiesFile << "gold=5\n";
        enemiesFile << "boss=false\n\n";

        enemiesFile << "[ENEMY]\n";
        enemiesFile << "id=8\n";
        enemiesFile << "name=Dark Knight\n";
        enemiesFile << "description=A fallen warrior in black plate armor.\n";
        enemiesFile << "dialogue=None shall pass!\n";
        enemiesFile << "hp=60\n";
        enemiesFile << "attack=12\n";
        enemiesFile << "defense=8\n";
        enemiesFile << "crit=10\n";
        enemiesFile << "dodge=2\n";
        enemiesFile << "speed=7\n";
        enemiesFile << "experience=75\n";
        enemiesFile << "gold=40\n";
        enemiesFile << "boss=false\n\n";
        
        enemiesFile << "[ENEMY]\n";
        enemiesFile << "id=100\n";
        enemiesFile << "name=Ancient Dragon\n";
        enemiesFile << "description=A terrifying ancient dragon guards the exit.\n";
        enemiesFile << "dialogue=You dare challenge me, mortal?!\n";
        enemiesFile << "hp=300\n";
        enemiesFile << "attack=25\n";
        enemiesFile << "defense=15\n";
        enemiesFile << "crit=15\n";
        enemiesFile << "dodge=10\n";
        enemiesFile << "speed=12\n";
        enemiesFile << "experience=500\n";
        enemiesFile << "gold=200\n";
        enemiesFile << "boss=true\n";
        
        enemiesFile.close();
    }
    
    // Create puzzles.txt
    std::ofstream puzzlesFile(dataPath + "puzzles.txt");
    if (puzzlesFile.is_open()) {
        puzzlesFile << "# Adventure Quest Engine - Puzzles Data File\n\n";
        
        puzzlesFile << "[PUZZLE]\n";
        puzzlesFile << "question=I have cities, but no houses. Mountains, but no trees. Water, but no fish. What am I?\n";
        puzzlesFile << "answer=map\n";
        puzzlesFile << "hint=Think about what you use to navigate.\n\n";
        
        puzzlesFile << "[PUZZLE]\n";
        puzzlesFile << "question=The more you take, the more you leave behind. What am I?\n";
        puzzlesFile << "answer=footsteps\n";
        puzzlesFile << "hint=Consider what happens when you walk.\n";
        
        puzzlesFile.close();
    }
    
    std::cout << "Default data files created in " << dataPath << "\n";
}

}  // namespace AQE
