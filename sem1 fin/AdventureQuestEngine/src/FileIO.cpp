/*******************************************************************************
 * C++ Adventure Quest Engine
 * FileIO.cpp - File Input/Output Implementation
 * 
 * This file contains the implementation of file I/O operations including
 * loading game data, saving game logs, save/load game state, and leaderboards.
 * 
 * Author: Adventure Quest Engine Team
 * Version: 1.0
 ******************************************************************************/

#include "../include/FileIO.h"
#include "../include/Utils.h"
#include "../include/Constants.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <chrono>

// Platform-specific includes for directory creation
#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
#else
    #include <sys/stat.h>
    #define MKDIR(path) mkdir(path, 0755)
#endif

namespace AQE {

//==============================================================================
// CONSTRUCTOR
//==============================================================================

FileIO::FileIO(const std::string& dataFolder) : dataPath(dataFolder) {
    ensureDataDirectory();
}

//==============================================================================
// DATA LOADING
//==============================================================================

bool FileIO::loadAllGameData(GameState& gameState) {
    bool success = true;
    
    // Load items
    gameState.itemTemplates = loadItems(dataPath + "items.txt");
    if (gameState.itemTemplates.empty()) {
        gameState.itemTemplates = createDefaultItems();
    }
    
    // Load enemies
    gameState.enemyTemplates = loadEnemies(dataPath + "enemies.txt");
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
        return createDefaultItems();
    }
    
    std::string line;
    Item current;
    bool inItem = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') continue;
        
        if (line == "[ITEM]") {
            if (inItem) {
                items.push_back(current);
            }
            current = Item();
            inItem = true;
            continue;
        }
        
        if (!inItem) continue;
        
        std::string key, value;
        if (parseKeyValue(line, key, value)) {
            if (key == "id") current.id = std::stoi(value);
            else if (key == "name") current.name = value;
            else if (key == "description") current.description = value;
            else if (key == "type") {
                if (value == "consumable") current.type = ItemType::CONSUMABLE;
                else if (value == "weapon") current.type = ItemType::WEAPON;
                else if (value == "armor") current.type = ItemType::ARMOR;
                else if (value == "accessory") current.type = ItemType::ACCESSORY;
                else if (value == "key") current.type = ItemType::KEY;
                else if (value == "quest") current.type = ItemType::QUEST;
                else if (value == "special") current.type = ItemType::SPECIAL;
            }
            else if (key == "rarity") {
                if (value == "common") current.rarity = ItemRarity::COMMON;
                else if (value == "uncommon") current.rarity = ItemRarity::UNCOMMON;
                else if (value == "rare") current.rarity = ItemRarity::RARE;
                else if (value == "epic") current.rarity = ItemRarity::EPIC;
                else if (value == "legendary") current.rarity = ItemRarity::LEGENDARY;
            }
            else if (key == "value") current.value = std::stoi(value);
            else if (key == "heal") current.healAmount = std::stoi(value);
            else if (key == "attack") current.attackBonus = std::stoi(value);
            else if (key == "defense") current.defenseBonus = std::stoi(value);
            else if (key == "crit") current.critBonus = std::stoi(value);
            else if (key == "dodge") current.dodgeBonus = std::stoi(value);
            else if (key == "speed") current.speedBonus = std::stoi(value);
            else if (key == "duration") current.duration = std::stoi(value);
            else if (key == "stackable") current.isStackable = (value == "true");
            else if (key == "usable") current.isUsable = (value == "true");
        }
    }
    
    if (inItem) {
        items.push_back(current);
    }
    
    file.close();
    return items;
}

std::vector<Enemy> FileIO::loadEnemies(const std::string& filename) {
    std::vector<Enemy> enemies;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return createDefaultEnemies();
    }
    
    std::string line;
    Enemy current;
    bool inEnemy = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') continue;
        
        if (line == "[ENEMY]") {
            if (inEnemy) {
                enemies.push_back(current);
            }
            current = Enemy();
            inEnemy = true;
            continue;
        }
        
        if (!inEnemy) continue;
        
        std::string key, value;
        if (parseKeyValue(line, key, value)) {
            if (key == "id") current.id = std::stoi(value);
            else if (key == "name") current.name = value;
            else if (key == "description") current.description = value;
            else if (key == "dialogue") current.dialogue = value;
            else if (key == "hp") {
                current.stats.maxHP = std::stoi(value);
                current.stats.currentHP = current.stats.maxHP;
            }
            else if (key == "attack") current.stats.attack = std::stoi(value);
            else if (key == "defense") current.stats.defense = std::stoi(value);
            else if (key == "crit") current.stats.critChance = std::stoi(value);
            else if (key == "dodge") current.stats.dodgeChance = std::stoi(value);
            else if (key == "speed") current.stats.speed = std::stoi(value);
            else if (key == "experience") current.experienceReward = std::stoi(value);
            else if (key == "gold") current.goldReward = std::stoi(value);
            else if (key == "boss") current.isBoss = (value == "true");
        }
    }
    
    if (inEnemy) {
        enemies.push_back(current);
    }
    
    file.close();
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

//==============================================================================
// GAME SAVING/LOADING
//==============================================================================

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
                    int roomId = std::stoi(parts[0]);
                    for (Room& room : gameState.dungeon) {
                        if (room.id == roomId) {
                            room.isExplored = (parts[1] == "1");
                            room.isCleared = (parts[2] == "1");
                            room.isTrapActive = (parts[3] == "1");
                            room.puzzleSolved = (parts[4] == "1");
                            break;
                        }
                    }
                }
            }
        }
    }
    
    file.close();
    
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
    
    // Simple implementation - just return default save file if it exists
    if (saveExists(dataPath + "savegame.txt")) {
        saveFiles.push_back("savegame.txt");
    }
    
    // In a full implementation, this would scan the data directory
    // for all .txt files matching save file patterns
    
    return saveFiles;
}

//==============================================================================
// GAME LOG
//==============================================================================

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

//==============================================================================
// LEADERBOARD
//==============================================================================

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

//==============================================================================
// PRIVATE METHODS
//==============================================================================

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

//==============================================================================
// DEFAULT DATA CREATION
//==============================================================================

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
    sword.attackBonus = 5;
    sword.value = 30;
    items.push_back(sword);
    
    Item axe;
    axe.id = 11;
    axe.name = "Battle Axe";
    axe.description = "A heavy battle axe.";
    axe.type = ItemType::WEAPON;
    axe.rarity = ItemRarity::UNCOMMON;
    axe.attackBonus = 8;
    axe.critBonus = 5;
    axe.value = 60;
    items.push_back(axe);
    
    Item staff;
    staff.id = 12;
    staff.name = "Arcane Staff";
    staff.description = "A staff imbued with magic.";
    staff.type = ItemType::WEAPON;
    staff.rarity = ItemRarity::RARE;
    staff.attackBonus = 6;
    staff.critBonus = 15;
    staff.value = 100;
    items.push_back(staff);
    
    Item legendSword;
    legendSword.id = 13;
    legendSword.name = "Dragonslayer";
    legendSword.description = "A legendary blade forged to slay dragons.";
    legendSword.type = ItemType::WEAPON;
    legendSword.rarity = ItemRarity::LEGENDARY;
    legendSword.attackBonus = 20;
    legendSword.critBonus = 10;
    legendSword.value = 500;
    items.push_back(legendSword);
    
    // Armor
    Item leather;
    leather.id = 20;
    leather.name = "Leather Armor";
    leather.description = "Basic protective leather armor.";
    leather.type = ItemType::ARMOR;
    leather.rarity = ItemRarity::COMMON;
    leather.defenseBonus = 3;
    leather.value = 25;
    items.push_back(leather);
    
    Item chainmail;
    chainmail.id = 21;
    chainmail.name = "Chainmail";
    chainmail.description = "Interlocking metal rings provide good protection.";
    chainmail.type = ItemType::ARMOR;
    chainmail.rarity = ItemRarity::UNCOMMON;
    chainmail.defenseBonus = 6;
    chainmail.speedBonus = -1;
    chainmail.value = 75;
    items.push_back(chainmail);
    
    Item plate;
    plate.id = 22;
    plate.name = "Plate Armor";
    plate.description = "Heavy plate armor with excellent protection.";
    plate.type = ItemType::ARMOR;
    plate.rarity = ItemRarity::RARE;
    plate.defenseBonus = 12;
    plate.speedBonus = -3;
    plate.value = 200;
    items.push_back(plate);
    
    // Accessories
    Item ring;
    ring.id = 30;
    ring.name = "Ring of Swiftness";
    ring.description = "Increases movement and attack speed.";
    ring.type = ItemType::ACCESSORY;
    ring.rarity = ItemRarity::UNCOMMON;
    ring.speedBonus = 5;
    ring.dodgeBonus = 5;
    ring.value = 80;
    items.push_back(ring);
    
    Item amulet;
    amulet.id = 31;
    amulet.name = "Amulet of Fortune";
    amulet.description = "Increases critical hit chance.";
    amulet.type = ItemType::ACCESSORY;
    amulet.rarity = ItemRarity::RARE;
    amulet.critBonus = 15;
    amulet.value = 150;
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
    skeleton.stats.maxHP = 40;
    skeleton.stats.currentHP = 40;
    skeleton.stats.attack = 8;
    skeleton.stats.defense = 3;
    skeleton.stats.critChance = 5;
    skeleton.stats.dodgeChance = 10;
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
    goblin.stats.maxHP = 30;
    goblin.stats.currentHP = 30;
    goblin.stats.attack = 6;
    goblin.stats.defense = 2;
    goblin.stats.critChance = 15;
    goblin.stats.dodgeChance = 20;
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
    orc.stats.maxHP = 70;
    orc.stats.currentHP = 70;
    orc.stats.attack = 12;
    orc.stats.defense = 5;
    orc.stats.critChance = 10;
    orc.stats.dodgeChance = 5;
    orc.stats.speed = 6;
    orc.experienceReward = 40;
    orc.goldReward = 20;
    orc.lootTable = {2, 11};
    orc.lootChances = {25, 15};
    enemies.push_back(orc);
    
    Enemy wraith;
    wraith.id = 4;
    wraith.name = "Wraith";
    wraith.description = "A ghostly apparition that drains life.";
    wraith.dialogue = "Join usss...";
    wraith.stats.maxHP = 50;
    wraith.stats.currentHP = 50;
    wraith.stats.attack = 10;
    wraith.stats.defense = 2;
    wraith.stats.critChance = 20;
    wraith.stats.dodgeChance = 30;
    wraith.stats.speed = 14;
    wraith.experienceReward = 50;
    wraith.goldReward = 25;
    
    EnemyAbility lifeDrain;
    lifeDrain.name = "Life Drain";
    lifeDrain.description = "Drains health from the target.";
    lifeDrain.damage = 15;
    lifeDrain.heals = true;
    lifeDrain.healAmount = 10;
    lifeDrain.cooldown = 3;
    lifeDrain.triggerChance = 0.3f;
    wraith.abilities.push_back(lifeDrain);
    
    enemies.push_back(wraith);
    
    Enemy troll;
    troll.id = 5;
    troll.name = "Cave Troll";
    troll.description = "A massive troll that regenerates health.";
    troll.dialogue = "RAAAWR!";
    troll.stats.maxHP = 100;
    troll.stats.currentHP = 100;
    troll.stats.attack = 15;
    troll.stats.defense = 8;
    troll.stats.critChance = 5;
    troll.stats.dodgeChance = 5;
    troll.stats.speed = 4;
    troll.experienceReward = 60;
    troll.goldReward = 30;
    
    EnemyAbility regen;
    regen.name = "Regeneration";
    regen.description = "The troll regenerates health.";
    regen.heals = true;
    regen.healAmount = 20;
    regen.cooldown = 2;
    regen.triggerChance = 0.5f;
    troll.abilities.push_back(regen);
    
    enemies.push_back(troll);
    
    // Boss enemies
    Enemy dragon;
    dragon.id = 100;
    dragon.name = "Ancient Dragon";
    dragon.description = "A terrifying ancient dragon guards the exit.";
    dragon.dialogue = "You dare challenge me, mortal?!";
    dragon.stats.maxHP = 300;
    dragon.stats.currentHP = 300;
    dragon.stats.attack = 25;
    dragon.stats.defense = 15;
    dragon.stats.critChance = 15;
    dragon.stats.dodgeChance = 10;
    dragon.stats.speed = 12;
    dragon.experienceReward = 500;
    dragon.goldReward = 200;
    dragon.isBoss = true;
    dragon.lootTable = {13, 50};  // Dragonslayer, Artifact
    dragon.lootChances = {50, 25};
    
    EnemyAbility fireBreath;
    fireBreath.name = "Fire Breath";
    fireBreath.description = "The dragon breathes devastating fire!";
    fireBreath.damage = 40;
    fireBreath.cooldown = 4;
    fireBreath.triggerChance = 0.4f;
    dragon.abilities.push_back(fireBreath);
    
    enemies.push_back(dragon);
    
    Enemy lich;
    lich.id = 101;
    lich.name = "The Lich King";
    lich.description = "An undead sorcerer of immense power.";
    lich.dialogue = "Death is only the beginning!";
    lich.stats.maxHP = 200;
    lich.stats.currentHP = 200;
    lich.stats.attack = 20;
    lich.stats.defense = 10;
    lich.stats.critChance = 25;
    lich.stats.dodgeChance = 15;
    lich.stats.speed = 10;
    lich.experienceReward = 400;
    lich.goldReward = 150;
    lich.isBoss = true;
    
    EnemyAbility darkBolt;
    darkBolt.name = "Dark Bolt";
    darkBolt.description = "A bolt of dark energy!";
    darkBolt.damage = 30;
    darkBolt.cooldown = 3;
    darkBolt.triggerChance = 0.35f;
    lich.abilities.push_back(darkBolt);
    
    EnemyAbility summon;
    summon.name = "Raise Dead";
    summon.description = "The Lich heals itself with dark magic.";
    summon.heals = true;
    summon.healAmount = 30;
    summon.cooldown = 5;
    summon.triggerChance = 0.25f;
    lich.abilities.push_back(summon);
    
    enemies.push_back(lich);
    
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
