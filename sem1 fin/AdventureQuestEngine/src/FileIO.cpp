//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// File I/O implementation

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

    Item frostmourne;
    frostmourne.id = 14;
    frostmourne.name = "Frostmourne";
    frostmourne.description = "A cursed blade that steals the souls of its victims.";
    frostmourne.type = ItemType::WEAPON;
    frostmourne.rarity = ItemRarity::LEGENDARY;
    frostmourne.attackBonus = 45;
    frostmourne.critBonus = 20;
    frostmourne.value = 5000;
    items.push_back(frostmourne);
    
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

    Item demonsteel;
    demonsteel.id = 23;
    demonsteel.name = "Demonsteel Armor";
    demonsteel.description = "Armor forged in the bowels of Hell, offering divine defense.";
    demonsteel.type = ItemType::ARMOR;
    demonsteel.rarity = ItemRarity::LEGENDARY;
    demonsteel.defenseBonus = 20;
    demonsteel.speedBonus = -2;
    demonsteel.value = 800;
    items.push_back(demonsteel);

    Item dominationhelm;
    dominationhelm.id = 24;
    dominationhelm.name = "Helm of Domination";
    dominationhelm.description = "A helm that grants control over death itself.";
    dominationhelm.type = ItemType::ARMOR;
    dominationhelm.rarity = ItemRarity::LEGENDARY;
    dominationhelm.defenseBonus = 15;
    dominationhelm.critBonus = 10;
    dominationhelm.value = 10000;
    items.push_back(dominationhelm);
    
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
    bronzeSword.attackBonus = 3;
    bronzeSword.value = 15;
    items.push_back(bronzeSword);
    
    Item steelSword;
    steelSword.id = 16;
    steelSword.name = "Steel Sword";
    steelSword.description = "A well-crafted steel blade.";
    steelSword.type = ItemType::WEAPON;
    steelSword.rarity = ItemRarity::UNCOMMON;
    steelSword.attackBonus = 10;
    steelSword.critBonus = 3;
    steelSword.value = 80;
    items.push_back(steelSword);
    
    Item mithrilSword;
    mithrilSword.id = 17;
    mithrilSword.name = "Mithril Sword";
    mithrilSword.description = "A lightweight sword of superior craftsmanship.";
    mithrilSword.type = ItemType::WEAPON;
    mithrilSword.rarity = ItemRarity::RARE;
    mithrilSword.attackBonus = 15;
    mithrilSword.critBonus = 8;
    mithrilSword.speedBonus = 2;
    mithrilSword.value = 250;
    items.push_back(mithrilSword);
    
    Item flamingSword;
    flamingSword.id = 18;
    flamingSword.name = "Flaming Longsword";
    flamingSword.description = "A blade wreathed in eternal flames.";
    flamingSword.type = ItemType::WEAPON;
    flamingSword.rarity = ItemRarity::EPIC;
    flamingSword.attackBonus = 18;
    flamingSword.critBonus = 12;
    flamingSword.value = 400;
    items.push_back(flamingSword);
    
    Item vorpalBlade;
    vorpalBlade.id = 19;
    vorpalBlade.name = "Vorpal Blade";
    vorpalBlade.description = "One, two! One, two! And through and through...";
    vorpalBlade.type = ItemType::WEAPON;
    vorpalBlade.rarity = ItemRarity::LEGENDARY;
    vorpalBlade.attackBonus = 25;
    vorpalBlade.critBonus = 30;
    vorpalBlade.value = 1200;
    items.push_back(vorpalBlade);
    
    // Daggers
    Item rustyDagger;
    rustyDagger.id = 70;
    rustyDagger.name = "Rusty Dagger";
    rustyDagger.description = "A worn dagger, still sharp enough.";
    rustyDagger.type = ItemType::WEAPON;
    rustyDagger.rarity = ItemRarity::COMMON;
    rustyDagger.attackBonus = 2;
    rustyDagger.critBonus = 5;
    rustyDagger.speedBonus = 3;
    rustyDagger.value = 8;
    items.push_back(rustyDagger);
    
    Item poisonDagger;
    poisonDagger.id = 71;
    poisonDagger.name = "Poison Dagger";
    poisonDagger.description = "Coated with deadly venom.";
    poisonDagger.type = ItemType::WEAPON;
    poisonDagger.rarity = ItemRarity::UNCOMMON;
    poisonDagger.attackBonus = 5;
    poisonDagger.critBonus = 15;
    poisonDagger.speedBonus = 4;
    poisonDagger.value = 90;
    items.push_back(poisonDagger);
    
    Item shadowDagger;
    shadowDagger.id = 72;
    shadowDagger.name = "Shadow Fang";
    shadowDagger.description = "Forged in darkness, strikes from nowhere.";
    shadowDagger.type = ItemType::WEAPON;
    shadowDagger.rarity = ItemRarity::RARE;
    shadowDagger.attackBonus = 8;
    shadowDagger.critBonus = 25;
    shadowDagger.speedBonus = 6;
    shadowDagger.dodgeBonus = 5;
    shadowDagger.value = 200;
    items.push_back(shadowDagger);
    
    // Axes
    Item woodcutterAxe;
    woodcutterAxe.id = 80;
    woodcutterAxe.name = "Woodcutter's Axe";
    woodcutterAxe.description = "Better for trees than combat, but it'll do.";
    woodcutterAxe.type = ItemType::WEAPON;
    woodcutterAxe.rarity = ItemRarity::COMMON;
    woodcutterAxe.attackBonus = 4;
    woodcutterAxe.critBonus = 3;
    woodcutterAxe.value = 20;
    items.push_back(woodcutterAxe);
    
    Item warAxe;
    warAxe.id = 81;
    warAxe.name = "War Axe";
    warAxe.description = "A brutal weapon designed for war.";
    warAxe.type = ItemType::WEAPON;
    warAxe.rarity = ItemRarity::UNCOMMON;
    warAxe.attackBonus = 12;
    warAxe.critBonus = 8;
    warAxe.speedBonus = -1;
    warAxe.value = 95;
    items.push_back(warAxe);
    
    Item greatAxe;
    greatAxe.id = 82;
    greatAxe.name = "Executioner's Greataxe";
    greatAxe.description = "A massive axe that cleaves through armor.";
    greatAxe.type = ItemType::WEAPON;
    greatAxe.rarity = ItemRarity::RARE;
    greatAxe.attackBonus = 20;
    greatAxe.critBonus = 15;
    greatAxe.speedBonus = -3;
    greatAxe.value = 280;
    items.push_back(greatAxe);
    
    // Spears
    Item woodenSpear;
    woodenSpear.id = 85;
    woodenSpear.name = "Wooden Spear";
    woodenSpear.description = "A simple spear with reach advantage.";
    woodenSpear.type = ItemType::WEAPON;
    woodenSpear.rarity = ItemRarity::COMMON;
    woodenSpear.attackBonus = 4;
    woodenSpear.value = 12;
    items.push_back(woodenSpear);
    
    Item pike;
    pike.id = 86;
    pike.name = "Steel Pike";
    pike.description = "A long pike for keeping enemies at bay.";
    pike.type = ItemType::WEAPON;
    pike.rarity = ItemRarity::UNCOMMON;
    pike.attackBonus = 9;
    pike.defenseBonus = 2;
    pike.value = 70;
    items.push_back(pike);
    
    Item halberd;
    halberd.id = 87;
    halberd.name = "Halberd";
    halberd.description = "A versatile polearm weapon.";
    halberd.type = ItemType::WEAPON;
    halberd.rarity = ItemRarity::RARE;
    halberd.attackBonus = 14;
    halberd.defenseBonus = 4;
    halberd.critBonus = 5;
    halberd.value = 180;
    items.push_back(halberd);
    
    // Staves and Wands
    Item apprenticeStaff;
    apprenticeStaff.id = 90;
    apprenticeStaff.name = "Apprentice's Staff";
    apprenticeStaff.description = "A basic staff for novice mages.";
    apprenticeStaff.type = ItemType::WEAPON;
    apprenticeStaff.rarity = ItemRarity::COMMON;
    apprenticeStaff.attackBonus = 3;
    apprenticeStaff.critBonus = 8;
    apprenticeStaff.value = 25;
    items.push_back(apprenticeStaff);
    
    Item fireStaff;
    fireStaff.id = 91;
    fireStaff.name = "Staff of Flames";
    fireStaff.description = "Channels the power of fire.";
    fireStaff.type = ItemType::WEAPON;
    fireStaff.rarity = ItemRarity::UNCOMMON;
    fireStaff.attackBonus = 7;
    fireStaff.critBonus = 12;
    fireStaff.value = 110;
    items.push_back(fireStaff);
    
    Item frostStaff;
    frostStaff.id = 92;
    frostStaff.name = "Staff of Winter";
    frostStaff.description = "Freezes enemies with icy magic.";
    frostStaff.type = ItemType::WEAPON;
    frostStaff.rarity = ItemRarity::RARE;
    frostStaff.attackBonus = 10;
    frostStaff.critBonus = 18;
    frostStaff.speedBonus = 1;
    frostStaff.value = 220;
    items.push_back(frostStaff);
    
    Item archmagusStaff;
    archmagusStaff.id = 93;
    archmagusStaff.name = "Archmagus Staff";
    archmagusStaff.description = "The staff of a grand wizard.";
    archmagusStaff.type = ItemType::WEAPON;
    archmagusStaff.rarity = ItemRarity::EPIC;
    archmagusStaff.attackBonus = 12;
    archmagusStaff.critBonus = 25;
    archmagusStaff.defenseBonus = 3;
    archmagusStaff.value = 450;
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
    hideArmor.defenseBonus = 4;
    hideArmor.value = 35;
    items.push_back(hideArmor);
    
    Item studdedLeather;
    studdedLeather.id = 103;
    studdedLeather.name = "Studded Leather Armor";
    studdedLeather.description = "Leather reinforced with metal studs.";
    studdedLeather.type = ItemType::ARMOR;
    studdedLeather.rarity = ItemRarity::UNCOMMON;
    studdedLeather.defenseBonus = 5;
    studdedLeather.dodgeBonus = 3;
    studdedLeather.value = 60;
    items.push_back(studdedLeather);
    
    // Armor Sets - Medium
    Item scalemail;
    scalemail.id = 104;
    scalemail.name = "Scale Mail";
    scalemail.description = "Overlapping metal scales.";
    scalemail.type = ItemType::ARMOR;
    scalemail.rarity = ItemRarity::UNCOMMON;
    scalemail.defenseBonus = 7;
    scalemail.speedBonus = -1;
    scalemail.value = 90;
    items.push_back(scalemail);
    
    Item breastplate;
    breastplate.id = 105;
    breastplate.name = "Breastplate";
    breastplate.description = "Solid chest protection.";
    breastplate.type = ItemType::ARMOR;
    breastplate.rarity = ItemRarity::UNCOMMON;
    breastplate.defenseBonus = 8;
    breastplate.speedBonus = -1;
    breastplate.value = 110;
    items.push_back(breastplate);
    
    Item halfPlate;
    halfPlate.id = 106;
    halfPlate.name = "Half Plate";
    halfPlate.description = "Partial plate armor with good mobility.";
    halfPlate.type = ItemType::ARMOR;
    halfPlate.rarity = ItemRarity::RARE;
    halfPlate.defenseBonus = 10;
    halfPlate.speedBonus = -2;
    halfPlate.value = 180;
    items.push_back(halfPlate);
    
    // Armor Sets - Heavy
    Item bandedArmor;
    bandedArmor.id = 107;
    bandedArmor.name = "Banded Mail";
    bandedArmor.description = "Strips of metal bound together.";
    bandedArmor.type = ItemType::ARMOR;
    bandedArmor.rarity = ItemRarity::UNCOMMON;
    bandedArmor.defenseBonus = 9;
    bandedArmor.speedBonus = -2;
    bandedArmor.value = 125;
    items.push_back(bandedArmor);
    
    Item splintArmor;
    splintArmor.id = 108;
    splintArmor.name = "Splint Armor";
    splintArmor.description = "Vertical metal strips over leather.";
    splintArmor.type = ItemType::ARMOR;
    splintArmor.rarity = ItemRarity::RARE;
    splintArmor.defenseBonus = 11;
    splintArmor.speedBonus = -2;
    splintArmor.value = 195;
    items.push_back(splintArmor);
    
    Item fullPlate;
    fullPlate.id = 109;
    fullPlate.name = "Full Plate Armor";
    fullPlate.description = "Complete heavy plate protection.";
    fullPlate.type = ItemType::ARMOR;
    fullPlate.rarity = ItemRarity::RARE;
    fullPlate.defenseBonus = 15;
    fullPlate.speedBonus = -4;
    fullPlate.value = 350;
    items.push_back(fullPlate);
    
    Item dragonscale;
    dragonscale.id = 110;
    dragonscale.name = "Dragonscale Armor";
    dragonscale.description = "Crafted from genuine dragon scales.";
    dragonscale.type = ItemType::ARMOR;
    dragonscale.rarity = ItemRarity::EPIC;
    dragonscale.defenseBonus = 14;
    dragonscale.dodgeBonus = 5;
    dragonscale.speedBonus = -1;
    dragonscale.value = 600;
    items.push_back(dragonscale);
    
    Item celestialArmor;
    celestialArmor.id = 111;
    celestialArmor.name = "Celestial Armor";
    celestialArmor.description = "Blessed armor from the heavens.";
    celestialArmor.type = ItemType::ARMOR;
    celestialArmor.rarity = ItemRarity::LEGENDARY;
    celestialArmor.defenseBonus = 18;
    celestialArmor.dodgeBonus = 8;
    celestialArmor.speedBonus = 1;
    celestialArmor.value = 1500;
    items.push_back(celestialArmor);
    
    // Shields
    Item woodenShield;
    woodenShield.id = 120;
    woodenShield.name = "Wooden Shield";
    woodenShield.description = "A basic wooden shield.";
    woodenShield.type = ItemType::ARMOR;
    woodenShield.rarity = ItemRarity::COMMON;
    woodenShield.defenseBonus = 2;
    woodenShield.value = 15;
    items.push_back(woodenShield);
    
    Item steelShield;
    steelShield.id = 121;
    steelShield.name = "Steel Shield";
    steelShield.description = "Sturdy metal shield.";
    steelShield.type = ItemType::ARMOR;
    steelShield.rarity = ItemRarity::UNCOMMON;
    steelShield.defenseBonus = 4;
    steelShield.value = 55;
    items.push_back(steelShield);
    
    Item towerShield;
    towerShield.id = 122;
    towerShield.name = "Tower Shield";
    towerShield.description = "Massive shield offering great protection.";
    towerShield.type = ItemType::ARMOR;
    towerShield.rarity = ItemRarity::RARE;
    towerShield.defenseBonus = 7;
    towerShield.speedBonus = -2;
    towerShield.value = 160;
    items.push_back(towerShield);
    
    // More Accessories
    Item strengthRing;
    strengthRing.id = 130;
    strengthRing.name = "Ring of Might";
    strengthRing.description = "Increases physical strength.";
    strengthRing.type = ItemType::ACCESSORY;
    strengthRing.rarity = ItemRarity::UNCOMMON;
    strengthRing.attackBonus = 5;
    strengthRing.value = 85;
    items.push_back(strengthRing);
    
    Item defenseRing;
    defenseRing.id = 131;
    defenseRing.name = "Ring of Protection";
    defenseRing.description = "Enhances defensive capabilities.";
    defenseRing.type = ItemType::ACCESSORY;
    defenseRing.rarity = ItemRarity::UNCOMMON;
    defenseRing.defenseBonus = 4;
    defenseRing.value = 75;
    items.push_back(defenseRing);
    
    Item vitalityAmulet;
    vitalityAmulet.id = 132;
    vitalityAmulet.name = "Amulet of Vitality";
    vitalityAmulet.description = "Increases maximum health.";
    vitalityAmulet.type = ItemType::ACCESSORY;
    vitalityAmulet.rarity = ItemRarity::RARE;
    vitalityAmulet.defenseBonus = 3;
    vitalityAmulet.value = 140;
    items.push_back(vitalityAmulet);
    
    Item shadowCloak;
    shadowCloak.id = 133;
    shadowCloak.name = "Cloak of Shadows";
    shadowCloak.description = "Enhances stealth and evasion.";
    shadowCloak.type = ItemType::ACCESSORY;
    shadowCloak.rarity = ItemRarity::RARE;
    shadowCloak.dodgeBonus = 12;
    shadowCloak.speedBonus = 3;
    shadowCloak.value = 175;
    items.push_back(shadowCloak);
    
    Item dragonHeart;
    dragonHeart.id = 134;
    dragonHeart.name = "Heart of the Dragon";
    dragonHeart.description = "Contains the essence of a slain dragon.";
    dragonHeart.type = ItemType::ACCESSORY;
    dragonHeart.rarity = ItemRarity::EPIC;
    dragonHeart.attackBonus = 8;
    dragonHeart.defenseBonus = 6;
    dragonHeart.critBonus = 10;
    dragonHeart.value = 500;
    items.push_back(dragonHeart);
    
    Item phoenixFeather;
    phoenixFeather.id = 135;
    phoenixFeather.name = "Phoenix Feather";
    phoenixFeather.description = "Grants regenerative powers.";
    phoenixFeather.type = ItemType::ACCESSORY;
    phoenixFeather.rarity = ItemRarity::LEGENDARY;
    phoenixFeather.defenseBonus = 5;
    phoenixFeather.dodgeBonus = 15;
    phoenixFeather.value = 800;
    items.push_back(phoenixFeather);
    
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
    ultimatePotion.healAmount = 9999;
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
    lifeDrain.healAmount = 8;
    lifeDrain.cooldown = 3;
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
    regen.healAmount = 12;
    regen.cooldown = 3;
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
    fireBreath.cooldown = 4;
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
    darkBolt.cooldown = 3;
    darkBolt.triggerChance = 0.30f;
    lich.abilities.push_back(darkBolt);
    
    EnemyAbility summon;
    summon.name = "Raise Dead";
    summon.description = "The Lich heals itself with dark magic.";
    summon.heals = true;
    summon.healAmount = 20;
    summon.cooldown = 5;
    summon.triggerChance = 0.20f;
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
