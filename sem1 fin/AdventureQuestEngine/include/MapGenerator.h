//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Procedural dungeon map generation

#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include "Types.h"
#include <vector>
#include <map>
#include <set>

namespace AQE {

// ---
// MAP GENERATOR CLASS
// ---

/**
// Procedural dungeon map generator
 * 
 * Generates a connected grid-based map with various room types,
 * guaranteeing a path from entrance to exit with optional secret rooms.
 */
class MapGenerator {
public:
    /**
// Constructor with map dimensions
     * @param width Map width in rooms
     * @param height Map height in rooms
     */
    MapGenerator(int width, int height);
    
    /**
// Generate a complete dungeon map
     * @param gameState Reference to game state to populate
     * @return True if generation successful
     */
    bool generateMap(GameState& gameState, bool mawMode = false);
    
    /**
// Get the generated rooms
     * @return Vector of generated rooms
     */
    const std::vector<Room>& getRooms() const;
    
    /**
// Get entrance room ID
     * @return ID of the entrance room
     */
    int getEntranceRoomId() const;
    
    /**
// Get exit room ID
     * @return ID of the exit room
     */
    int getExitRoomId() const;
    
    /**
// Get the dungeon grid
     * @return 2D grid of room IDs
     */
    const std::vector<std::vector<int>>& getGrid() const;

    /**
// Sync the generator's internal map state from an existing dungeon
     * @param dungeon Loaded dungeon rooms
     */
    void syncFromDungeon(const std::vector<Room>& dungeon);
    
    /**
// Print ASCII representation of the map
     * @param currentRoomId ID of the room the player is currently in (for highlighting)
     */
    void printMap(int currentRoomId = -1) const;

    /**
// Print a Maw-specific map view with optional fog of war
     * @param currentRoomId ID of the room the player is currently in (for highlighting)
     * @param stratum Current Maw stratum depth being traversed
     * @param fogOfWar True to hide unexplored rooms
     */
    void printMawMap(int currentRoomId = -1, int stratum = 1, bool fogOfWar = true) const;

private:
    //==========================================================================
    // PRIVATE MEMBER VARIABLES
    //==========================================================================
    
    int mapWidth;
    int mapHeight;
    std::vector<Room> rooms;
    std::vector<std::vector<int>> grid;  // 2D grid of room IDs (-1 = empty)
    int entranceId;
    int exitId;
    int nextRoomId;
    int forgeCount;  // Track forge spawn count for balanced distribution
    bool mawModeActive;
    int mawIntensityLevel;

    /**
// Find room pointer by ID
     * @param roomId Room identifier to find
     * @return Pointer to room or nullptr if not found
     */
    const Room* findRoomById(int roomId) const;
    
    //==========================================================================
    // PRIVATE METHODS - Core Generation
    //==========================================================================
    
    /**
// Initialize the grid with empty cells
     */
    void initializeGrid();
    
    /**
// Place entrance room at starting position
     * @return ID of entrance room
     */
    int placeEntranceRoom();
    
    /**
// Place exit room at ending position
     * @return ID of exit room
     */
    int placeExitRoom();

    /**
// Place the unique Shadow Lord chamber far from the entrance
     * @return True if the room was placed successfully
     */
    bool placeShadowLordRoom();

    /**
// Place a basic shop next to the entrance
     * @return True if shop successfully placed
     */
    bool placeBasicShopNextToEntrance();

    /**
// Place a forge next to the entrance
     * @return True if forge successfully placed
     */
    bool placeForgeNextToEntrance();
    
    /**
// Generate the main path from entrance to exit
     * @return True if path successfully generated
     */
    bool generateMainPath();
    
    /**
// Fill remaining grid spaces with optional rooms
     */
    void fillRemainingRooms();
    
    /**
// Place secret rooms off the main path
     * @param count Number of secret rooms to place
     */
    void placeSecretRooms(int count);
    
    /**
// Connect adjacent rooms with doorways
     */
    void connectRooms();
    
    //==========================================================================
    // PRIVATE METHODS - Room Creation
    //==========================================================================
    
    /**
// Create a room at specified grid position
     * @param x Grid X coordinate
     * @param y Grid Y coordinate
     * @param type Type of room to create
     * @return Created room
     */
    Room createRoom(int x, int y, RoomType type);
    
    /**
// Generate room name based on type and position
     * @param type Room type
     * @return Generated room name
     */
    std::string generateRoomName(RoomType type);
    
    /**
// Generate room description based on type
     * @param type Room type
     * @return Generated room description
     */
    std::string generateRoomDescription(RoomType type);
    
    /**
// Determine room type for a position
     * @param x Grid X coordinate
     * @param y Grid Y coordinate
     * @param isMainPath Whether this room is on the main path
     * @return Determined room type
     */
    RoomType determineRoomType(int x, int y, bool isMainPath = false);
    
    //==========================================================================
    // PRIVATE METHODS - Population
    //==========================================================================
    
    /**
// Populate room with enemies based on type
     * @param room Reference to room to populate
     * @param templates Available enemy templates
     */
    void populateWithEnemies(Room& room, const std::vector<Enemy>& templates, int contextLevel, bool mawMode);
    
    /**
// Populate room with items based on type
     * @param room Reference to room to populate
     * @param templates Available item templates
     */
    void populateWithItems(Room& room, const std::vector<Item>& templates, int contextLevel, bool mawMode);
    
    /**
// Assign events to room based on type
     * @param room Reference to room
     * @param templates Available event templates
     */
    void assignEvents(Room& room, const std::vector<Event>& templates);
    
    /**
// Setup puzzle for puzzle rooms
     * @param room Reference to puzzle room
     */
    void setupPuzzle(Room& room);
    
    /**
// Setup trap for trap rooms
     * @param room Reference to trap room
     */
    void setupTrap(Room& room);
    
    //==========================================================================
    // PRIVATE METHODS - Validation
    //==========================================================================
    
    /**
// Check if position is valid grid coordinate
     * @param x X coordinate
     * @param y Y coordinate
     * @return True if valid
     */
    bool isValidPosition(int x, int y) const;
    
    /**
// Check if position has a room
     * @param x X coordinate
     * @param y Y coordinate
     * @return True if room exists at position
     */
    bool hasRoom(int x, int y) const;
    
    /**
// Verify map is fully connected (BFS)
     * @return True if all rooms reachable from entrance
     */
    bool verifyConnectivity();
    
    /**
// Get adjacent empty cells
     * @param x Current X
     * @param y Current Y
     * @return Vector of (x,y) pairs
     */
    std::vector<std::pair<int,int>> getAdjacentEmpty(int x, int y);
    
    /**
// Get adjacent rooms
     * @param x Current X
     * @param y Current Y
     * @return Vector of room IDs
     */
    std::vector<int> getAdjacentRooms(int x, int y);
};

// ---
// STANDALONE HELPER FUNCTIONS
// ---

/**
// Generate a complete default dungeon
 * @param gameState Game state to populate
 * @param width Map width
 * @param height Map height
 * @return True if successful
 */
bool generateDungeon(GameState& gameState, int width = 8, int height = 8);

/**
// Load room templates from file
 * @param filename Path to rooms file
 * @return Vector of room templates
 */
std::vector<Room> loadRoomTemplates(const std::string& filename);

/**
// Get room by ID from game state
 * @param gameState Game state to search
 * @param roomId ID of room to find
 * @return Pointer to room or nullptr
 */
Room* getRoomById(GameState& gameState, int roomId);

/**
// Get room at grid position
 * @param gameState Game state to search
 * @param x Grid X coordinate
 * @param y Grid Y coordinate
 * @return Pointer to room or nullptr
 */
Room* getRoomAtPosition(GameState& gameState, int x, int y);

}  // namespace AQE

#endif // MAP_GENERATOR_H
