





#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include "Types.h"
#include <vector>
#include <map>
#include <set>

namespace AQE {






class MapGenerator {
public:
    
    MapGenerator(int width, int height);
    
    
    bool generateMap(GameState& gameState, bool mawMode = false);
    
    
    const std::vector<Room>& getRooms() const;
    
    
    int getEntranceRoomId() const;
    
    
    int getExitRoomId() const;
    
    
    const std::vector<std::vector<int>>& getGrid() const;

    
    void syncFromDungeon(const std::vector<Room>& dungeon);
    
    
    void printMap(int currentRoomId = -1) const;

    
    void printMawMap(int currentRoomId = -1, int stratum = 1, bool fogOfWar = true) const;

private:
    
    
    
    
    int mapWidth;
    int mapHeight;
    std::vector<Room> rooms;
    std::vector<std::vector<int>> grid;  
    int entranceId;
    int exitId;
    int nextRoomId;
    int forgeCount;  
    bool mawModeActive;
    int mawIntensityLevel;

    
    const Room* findRoomById(int roomId) const;
    
    
    
    
    
    
    void initializeGrid();
    
    
    int placeEntranceRoom();
    
    
    int placeExitRoom();

    
    bool placeShadowLordRoom();

    
    void ensureMawExitRoom();

    
    bool placeBasicShopNextToEntrance();

    
    bool placeForgeNextToEntrance();
    
    
    bool generateMainPath();
    
    
    void fillRemainingRooms();
    
    
    void placeSecretRooms(int count);
    
    
    void connectRooms();
    
    
    
    
    
    
    Room createRoom(int x, int y, RoomType type);
    
    
    std::string generateRoomName(RoomType type);
    
    
    std::string generateRoomDescription(RoomType type);
    
    
    RoomType determineRoomType(int x, int y, bool isMainPath = false);
    
    
    
    
    
    
    void populateWithEnemies(Room& room, const std::vector<Enemy>& templates, int contextLevel, bool mawMode, int playerLevel);
    
    
    void populateWithItems(Room& room, const std::vector<Item>& templates, bool mawMode, int playerLevel);
    
    
    void assignEvents(Room& room, const std::vector<Event>& templates);
    
    
    void setupPuzzle(Room& room);
    
    
    void setupTrap(Room& room);
    
    
    
    
    
    
    bool isValidPosition(int x, int y) const;
    
    
    bool hasRoom(int x, int y) const;
    
    
    bool verifyConnectivity();
    
    
    std::vector<std::pair<int,int>> getAdjacentEmpty(int x, int y);
    
    
    std::vector<int> getAdjacentRooms(int x, int y);
};






bool generateDungeon(GameState& gameState, int width = 8, int height = 8);


std::vector<Room> loadRoomTemplates(const std::string& filename);


Room* getRoomById(GameState& gameState, int roomId);


Room* getRoomAtPosition(GameState& gameState, int x, int y);

}  

#endif 
