//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// File I/O for game data, save/load, and leaderboards

#ifndef FILE_IO_H
#define FILE_IO_H

#include "Types.h"
#include <string>
#include <vector>
#include <fstream>

namespace AQE {

// ---
// LEADERBOARD ENTRY
// ---

/**
// Represents a leaderboard entry
 */
struct LeaderboardEntry {
    std::string playerName;
    int score;
    int turns;
    std::string characterClass;
    std::string ending;
    std::string date;
    
    LeaderboardEntry() : score(0), turns(0) {}
};

// ---
// FILE I/O CLASS
// ---

/**
// Handles all file input/output operations
 */
class FileIO {
public:
    /**
// Constructor with optional data folder path
     * @param dataFolder Path to data folder
     */
    explicit FileIO(const std::string& dataFolder = "data/");
    
    //==========================================================================
    // DATA LOADING
    //==========================================================================
    
    /**
// Load all game data from files
     * @param gameState Game state to populate
     * @return True if at least base data loaded
     */
    bool loadAllGameData(GameState& gameState);
    
    /**
// Load room descriptions from file
     * @param filename Path to rooms file
     * @return Vector of room templates
     */
    std::vector<Room> loadRooms(const std::string& filename);
    
    /**
// Load item templates from file
     * @param filename Path to items file
     * @return Vector of item templates
     */
    std::vector<Item> loadItems(const std::string& filename);
    
    /**
// Load enemy templates from file
     * @param filename Path to enemies file
     * @return Vector of enemy templates
     */
    std::vector<Enemy> loadEnemies(const std::string& filename);
    
    /**
// Load puzzle/riddle data from file
     * @param filename Path to puzzles file
     * @return Vector of riddle structures
     */
    std::vector<Riddle> loadPuzzles(const std::string& filename);
    
    /**
// Load event templates from file
     * @param filename Path to events file
     * @return Vector of event templates
     */
    std::vector<Event> loadEvents(const std::string& filename);
    
    /**
// Load quest templates from file
     * @param filename Path to quests file
     * @return Vector of quest templates
     */
    std::vector<Quest> loadQuests(const std::string& filename);
    
    //==========================================================================
    // GAME SAVING/LOADING
    //==========================================================================
    
    /**
// Save current game state
     * @param gameState Game state to save
     * @param filename Save file path
     * @return True if saved successfully
     */
    bool saveGame(const GameState& gameState, const std::string& filename);
    
    /**
// Load game state from file
     * @param gameState Game state to populate
     * @param filename Save file path
     * @return True if loaded successfully
     */
    bool loadGame(GameState& gameState, const std::string& filename);
    
    /**
// Check if save file exists
     * @param filename Save file path
     * @return True if file exists
     */
    bool saveExists(const std::string& filename) const;
    
    /**
// Delete save file
     * @param filename Save file path
     * @return True if deleted
     */
    bool deleteSave(const std::string& filename);
    
    /**
// List available save files
     * @return Vector of save file names
     */
    std::vector<std::string> listSaveFiles() const;
    
    //==========================================================================
    // GAME LOG
    //==========================================================================
    
    /**
// Save game log to file
     * @param player Player with game log
     * @param filename Log file path
     * @param ending Game ending type
     * @return True if saved
     */
    bool saveGameLog(const Player& player, const std::string& filename,
                     EndingType ending);
    
    /**
// Append to game log
     * @param message Message to append
     * @param filename Log file path
     * @return True if appended
     */
    bool appendToLog(const std::string& message, const std::string& filename);
    
    //==========================================================================
    // LEADERBOARD
    //==========================================================================
    
    /**
// Load leaderboard from file
     * @param filename Leaderboard file path
     * @return Vector of leaderboard entries
     */
    std::vector<LeaderboardEntry> loadLeaderboard(const std::string& filename);
    
    /**
// Save leaderboard to file
     * @param entries Leaderboard entries
     * @param filename Leaderboard file path
     * @return True if saved
     */
    bool saveLeaderboard(const std::vector<LeaderboardEntry>& entries,
                         const std::string& filename);
    
    /**
// Add entry to leaderboard (sorted by score)
     * @param entry New entry
     * @param filename Leaderboard file path
     * @param maxEntries Maximum entries to keep
     * @return Position in leaderboard (0 = first, -1 = didn't make it)
     */
    int addToLeaderboard(const LeaderboardEntry& entry,
                         const std::string& filename,
                         int maxEntries = 10);
    
    /**
// Display leaderboard
     * @param filename Leaderboard file path
     * @param count Number of entries to show
     */
    void displayLeaderboard(const std::string& filename, int count = 10);
    
    /**
// Sort leaderboard by score (descending)
     * @param entries Entries to sort
     */
    void sortLeaderboardByScore(std::vector<LeaderboardEntry>& entries);
    
    /**
// Sort leaderboard by turns (ascending - fewer is better)
     * @param entries Entries to sort
     */
    void sortLeaderboardByTurns(std::vector<LeaderboardEntry>& entries);

private:
    std::string dataPath;
    
    /**
// Ensure data directory exists
     * @return True if directory exists or was created
     */
    bool ensureDataDirectory();
    
    /**
// Parse a key=value line
     * @param line Line to parse
     * @param key Output key
     * @param value Output value
     * @return True if parsed successfully
     */
    bool parseKeyValue(const std::string& line, std::string& key, std::string& value);
    
    /**
// Get current date/time as string
     * @return Date string
     */
    std::string getCurrentDateTime() const;
};

// ---
// DEFAULT DATA CREATION
// ---

/**
// Create default item templates if file not found
 * @return Vector of default items
 */
std::vector<Item> createDefaultItems();

/**
// Create default enemy templates if file not found
 * @return Vector of default enemies
 */
std::vector<Enemy> createDefaultEnemies();

/**
// Create default room templates if file not found
 * @return Vector of default rooms
 */
std::vector<Room> createDefaultRooms();

/**
// Write default data files
 * @param dataPath Path to data folder
 */
void createDefaultDataFiles(const std::string& dataPath);

}  // namespace AQE

#endif // FILE_IO_H
