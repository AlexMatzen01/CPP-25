





#ifndef FILE_IO_H
#define FILE_IO_H

#include "Types.h"
#include <string>
#include <vector>
#include <fstream>

namespace AQE {






struct LeaderboardEntry {
    std::string playerName;
    int score;
    int turns;
    std::string characterClass;
    std::string ending;
    std::string date;
    
    LeaderboardEntry() : score(0), turns(0) {}
};






class FileIO {
public:
    
    explicit FileIO(const std::string& dataFolder = "data/");
    
    
    
    
    
    
    bool loadAllGameData(GameState& gameState);
    
    
    std::vector<Room> loadRooms(const std::string& filename);
    
    
    std::vector<Item> loadItems(const std::string& filename);
    
    
    std::vector<Enemy> loadEnemies(const std::string& filename);
    
    
    std::vector<Riddle> loadPuzzles(const std::string& filename);
    
    
    std::vector<Event> loadEvents(const std::string& filename);
    
    
    std::vector<Quest> loadQuests(const std::string& filename);
    
    
    
    
    
    
    bool saveGame(const GameState& gameState, const std::string& filename);
    
    
    bool loadGame(GameState& gameState, const std::string& filename);
    
    
    bool saveExists(const std::string& filename) const;
    
    
    bool deleteSave(const std::string& filename);
    
    
    std::vector<std::string> listSaveFiles() const;
    
    
    
    
    
    
    bool saveGameLog(const Player& player, const std::string& filename,
                     EndingType ending);
    
    
    bool appendToLog(const std::string& message, const std::string& filename);
    
    
    
    
    
    
    std::vector<LeaderboardEntry> loadLeaderboard(const std::string& filename);
    
    
    bool saveLeaderboard(const std::vector<LeaderboardEntry>& entries,
                         const std::string& filename);
    
    
    int addToLeaderboard(const LeaderboardEntry& entry,
                         const std::string& filename,
                         int maxEntries = 10);
    
    
    void displayLeaderboard(const std::string& filename, int count = 10);
    
    
    void sortLeaderboardByScore(std::vector<LeaderboardEntry>& entries);
    
    
    void sortLeaderboardByTurns(std::vector<LeaderboardEntry>& entries);

private:
    std::string dataPath;
    
    
    bool ensureDataDirectory();
    
    
    bool parseKeyValue(const std::string& line, std::string& key, std::string& value);
    
    
    std::string getCurrentDateTime() const;
};






std::vector<Item> createDefaultItems();


std::vector<Enemy> createDefaultEnemies();


std::vector<Room> createDefaultRooms();


void createDefaultDataFiles(const std::string& dataPath);

}  

#endif 
