//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Procedural map generation implementation

#include "../include/MapGenerator.h"
#include "../include/Utils.h"
#include "../include/Constants.h"
#include <queue>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>

namespace AQE {

// ---
// CONSTRUCTOR
// ---

MapGenerator::MapGenerator(int width, int height)
    : mapWidth(width), mapHeight(height), entranceId(-1), exitId(-1), nextRoomId(0) {
    // Clamp dimensions to valid range
    if (mapWidth < MIN_MAP_WIDTH) mapWidth = MIN_MAP_WIDTH;
    if (mapWidth > MAX_MAP_WIDTH) mapWidth = MAX_MAP_WIDTH;
    if (mapHeight < MIN_MAP_HEIGHT) mapHeight = MIN_MAP_HEIGHT;
    if (mapHeight > MAX_MAP_HEIGHT) mapHeight = MAX_MAP_HEIGHT;
}

// ---
// PUBLIC METHODS
// ---

bool MapGenerator::generateMap(GameState& gameState) {
    // Store map dimensions in game state
    gameState.mapWidth = mapWidth;
    gameState.mapHeight = mapHeight;
    
    // Clear previous data
    rooms.clear();
    initializeGrid();
    
    // Step 1: Place entrance at bottom-left area
    entranceId = placeEntranceRoom();
    if (entranceId < 0) return false;
    
    // Step 2: Place exit at top-right area
    exitId = placeExitRoom();
    if (exitId < 0) return false;
    
    // Step 3: Generate main path between entrance and exit
    if (!generateMainPath()) return false;
    
    // Step 4: Fill remaining spaces with optional rooms
    fillRemainingRooms();
    
    // Step 5: Place secret rooms (1-3 based on map size)
    int secretCount = 1 + randomInt(0, mapWidth * mapHeight / 20);
    placeSecretRooms(secretCount);
    
    // Step 6: Connect all adjacent rooms
    connectRooms();
    
    // Step 7: Verify connectivity
    if (!verifyConnectivity()) {
        // If not connected, regenerate (shouldn't happen with proper algorithm)
        return generateMap(gameState);
    }
    
    // Step 8: Populate rooms with content
    for (Room& room : rooms) {
        populateWithEnemies(room, gameState.enemyTemplates);
        populateWithItems(room, gameState.itemTemplates);
        assignEvents(room, gameState.eventTemplates);
        
        if (room.type == RoomType::PUZZLE) {
            setupPuzzle(room);
        }
        if (room.type == RoomType::TRAP) {
            setupTrap(room);
        }
    }
    
    // Copy rooms to game state
    gameState.dungeon = rooms;
    
    // Build room lookup map
    gameState.roomMap.clear();
    for (Room& room : gameState.dungeon) {
        gameState.roomMap[room.id] = &room;
    }
    
    return true;
}

const std::vector<Room>& MapGenerator::getRooms() const {
    return rooms;
}

int MapGenerator::getEntranceRoomId() const {
    return entranceId;
}

int MapGenerator::getExitRoomId() const {
    return exitId;
}

void MapGenerator::printMap() const {
    std::cout << "\n=== DUNGEON MAP ===\n\n";
    
    for (int y = mapHeight - 1; y >= 0; --y) {
        // Print top border of row
        for (int x = 0; x < mapWidth; ++x) {
            if (grid[y][x] >= 0) {
                std::cout << "+---";
            } else {
                std::cout << "+   ";
            }
        }
        std::cout << "+\n";
        
        // Print room content
        for (int x = 0; x < mapWidth; ++x) {
            if (grid[y][x] >= 0) {
                int roomId = grid[y][x];
                char symbol = ' ';
                
                // Find room and get symbol
                for (const Room& room : rooms) {
                    if (room.id == roomId) {
                        switch (room.type) {
                            case RoomType::ENTRANCE: symbol = 'E'; break;
                            case RoomType::EXIT:     symbol = 'X'; break;
                            case RoomType::BOSS:     symbol = 'B'; break;
                            case RoomType::TREASURE: symbol = 'T'; break;
                            case RoomType::TRAP:     symbol = '!'; break;
                            case RoomType::PUZZLE:   symbol = '?'; break;
                            case RoomType::SECRET:   symbol = 'S'; break;
                            case RoomType::SAFE:     symbol = 'R'; break;
                            default:                 symbol = '.'; break;
                        }
                        break;
                    }
                }
                std::cout << "| " << symbol << " ";
            } else {
                std::cout << "|   ";
            }
        }
        std::cout << "|\n";
    }
    
    // Print bottom border
    for (int x = 0; x < mapWidth; ++x) {
        std::cout << "+---";
    }
    std::cout << "+\n";
    
    // Print legend
    std::cout << "\nLegend: E=Entrance, X=Exit, B=Boss, T=Treasure, !=Trap, ?=Puzzle, S=Secret, R=Rest, .=Normal\n\n";
}

// ---
// PRIVATE METHODS - Core Generation
// ---

void MapGenerator::initializeGrid() {
    grid.clear();
    grid.resize(mapHeight);
    for (int y = 0; y < mapHeight; ++y) {
        grid[y].resize(mapWidth, -1);  // -1 means empty
    }
    nextRoomId = 0;
}

int MapGenerator::placeEntranceRoom() {
    // Place entrance in bottom-left quadrant
    int x = randomInt(0, mapWidth / 3);
    int y = randomInt(0, mapHeight / 3);
    
    Room entrance = createRoom(x, y, RoomType::ENTRANCE);
    entrance.name = "Dungeon Entrance";
    entrance.description = "You stand at the entrance of a dark, foreboding dungeon. "
                          "Cold air flows from within, carrying the scent of ancient stone and forgotten secrets. "
                          "There's no turning back now.";
    entrance.isExplored = true;  // Player starts here
    
    rooms.push_back(entrance);
    grid[y][x] = entrance.id;
    
    return entrance.id;
}

int MapGenerator::placeExitRoom() {
    // Place exit in top-right quadrant (away from entrance)
    int x = randomInt(mapWidth * 2 / 3, mapWidth - 1);
    int y = randomInt(mapHeight * 2 / 3, mapHeight - 1);
    
    // Make sure it's not the same as entrance
    while (grid[y][x] >= 0) {
        x = randomInt(mapWidth / 2, mapWidth - 1);
        y = randomInt(mapHeight / 2, mapHeight - 1);
    }
    
    Room exitRoom = createRoom(x, y, RoomType::EXIT);
    exitRoom.name = "Chamber of Escape";
    exitRoom.description = "A grand chamber stretches before you, ancient runes glowing on the walls. "
                          "In the center stands a magnificent portal, swirling with ethereal light. "
                          "This is the exit - your path to freedom and glory!";
    
    rooms.push_back(exitRoom);
    grid[y][x] = exitRoom.id;
    
    return exitRoom.id;
}

bool MapGenerator::generateMainPath() {
    // Find entrance and exit positions
    int startX = -1, startY = -1, endX = -1, endY = -1;
    
    for (const Room& room : rooms) {
        if (room.id == entranceId) {
            startX = room.x;
            startY = room.y;
        }
        if (room.id == exitId) {
            endX = room.x;
            endY = room.y;
        }
    }
    
    if (startX < 0 || endX < 0) return false;
    
    // Generate path using random walk with bias toward exit
    int currentX = startX;
    int currentY = startY;
    int maxIterations = mapWidth * mapHeight * 2;  // Prevent infinite loops
    
    while ((currentX != endX || currentY != endY) && maxIterations > 0) {
        maxIterations--;
        
        // Calculate direction bias toward exit
        int dx = (endX > currentX) ? 1 : (endX < currentX) ? -1 : 0;
        int dy = (endY > currentY) ? 1 : (endY < currentY) ? -1 : 0;
        
        // Choose direction (70% chance to move toward exit, 30% random)
        int moveX = 0, moveY = 0;
        
        if (randomChance(70)) {
            // Move toward exit
            if (randomChance(50) && dx != 0) {
                moveX = dx;
            } else if (dy != 0) {
                moveY = dy;
            } else {
                moveX = dx;
            }
        } else {
            // Random direction
            int dir = randomInt(0, 3);
            switch (dir) {
                case 0: moveX = 1; break;
                case 1: moveX = -1; break;
                case 2: moveY = 1; break;
                case 3: moveY = -1; break;
            }
        }
        
        int newX = currentX + moveX;
        int newY = currentY + moveY;
        
        // Check if valid position
        if (isValidPosition(newX, newY) && !hasRoom(newX, newY)) {
            // Create room at this position
            RoomType type = determineRoomType(newX, newY);
            Room room = createRoom(newX, newY, type);
            rooms.push_back(room);
            grid[newY][newX] = room.id;
        }
        
        // Move to new position if valid
        if (isValidPosition(newX, newY)) {
            currentX = newX;
            currentY = newY;
        }
    }
    
    // Ensure we have a connected path (verify later)
    return true;
}

void MapGenerator::fillRemainingRooms() {
    // Random fill with decreasing probability based on distance from path
    int fillChance = 40;  // Base chance to fill empty cell
    
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if (!hasRoom(x, y)) {
                // Check if adjacent to existing room
                std::vector<int> adjacent = getAdjacentRooms(x, y);
                
                if (!adjacent.empty() && randomChance(fillChance)) {
                    RoomType type = determineRoomType(x, y);
                    Room room = createRoom(x, y, type);
                    rooms.push_back(room);
                    grid[y][x] = room.id;
                }
            }
        }
    }
}

void MapGenerator::placeSecretRooms(int count) {
    std::vector<std::pair<int,int>> candidates;
    
    // Find all empty cells adjacent to exactly one room
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if (!hasRoom(x, y)) {
                std::vector<int> adjacent = getAdjacentRooms(x, y);
                if (adjacent.size() == 1) {
                    candidates.push_back({x, y});
                }
            }
        }
    }
    
    // Place secret rooms in random candidate positions
    for (int i = 0; i < count && !candidates.empty(); ++i) {
        int idx = randomInt(0, static_cast<int>(candidates.size()) - 1);
        auto [x, y] = candidates[idx];
        
        Room secret = createRoom(x, y, RoomType::SECRET);
        secret.isSecret = true;
        secret.name = "Hidden Chamber";
        secret.description = "You've discovered a secret room! Ancient treasures and forgotten "
                            "relics fill this hidden space. The air is thick with mystery.";
        
        rooms.push_back(secret);
        grid[y][x] = secret.id;
        
        candidates.erase(candidates.begin() + idx);
    }
}

void MapGenerator::connectRooms() {
    // Connect each room to its adjacent rooms
    for (Room& room : rooms) {
        int x = room.x;
        int y = room.y;
        
        // Check North (y+1)
        if (isValidPosition(x, y + 1) && hasRoom(x, y + 1)) {
            room.hasNorth = true;
            room.northRoomId = grid[y + 1][x];
        }
        
        // Check South (y-1)
        if (isValidPosition(x, y - 1) && hasRoom(x, y - 1)) {
            room.hasSouth = true;
            room.southRoomId = grid[y - 1][x];
        }
        
        // Check East (x+1)
        if (isValidPosition(x + 1, y) && hasRoom(x + 1, y)) {
            room.hasEast = true;
            room.eastRoomId = grid[y][x + 1];
        }
        
        // Check West (x-1)
        if (isValidPosition(x - 1, y) && hasRoom(x - 1, y)) {
            room.hasWest = true;
            room.westRoomId = grid[y][x - 1];
        }
    }
}

// ---
// PRIVATE METHODS - Room Creation
// ---

Room MapGenerator::createRoom(int x, int y, RoomType type) {
    Room room;
    room.id = nextRoomId++;
    room.x = x;
    room.y = y;
    room.type = type;
    room.name = generateRoomName(type);
    room.description = generateRoomDescription(type);
    room.shortDesc = "A " + toLower(roomTypeToString(type)) + " you've visited before.";
    room.isExplored = false;
    room.isCleared = (type == RoomType::SAFE || type == RoomType::ENTRANCE);
    
    return room;
}

std::string MapGenerator::generateRoomName(RoomType type) {
    std::string prefix = randomElement(ROOM_PREFIXES);
    std::string roomType = randomElement(ROOM_TYPES_TEXT);
    
    switch (type) {
        case RoomType::TREASURE:
            return prefix + " Treasure " + roomType;
        case RoomType::BOSS:
            return "The " + prefix + " Boss " + roomType;
        case RoomType::TRAP:
            return prefix + " Trap " + roomType;
        case RoomType::PUZZLE:
            return prefix + " Puzzle " + roomType;
        case RoomType::SECRET:
            return "Hidden " + roomType;
        case RoomType::SAFE:
            return "Peaceful " + roomType;
        default:
            return prefix + " " + roomType;
    }
}

std::string MapGenerator::generateRoomDescription(RoomType type) {
    std::string base = randomElement(ROOM_DESCRIPTORS);
    std::string extra;
    
    switch (type) {
        case RoomType::TREASURE:
            extra = " Glittering objects catch your eye from various corners.";
            break;
        case RoomType::BOSS:
            extra = " A powerful presence fills the air. Something dangerous lurks here.";
            break;
        case RoomType::TRAP:
            extra = " Something feels off. You sense danger with every step.";
            break;
        case RoomType::PUZZLE:
            extra = " Strange mechanisms and cryptic symbols suggest a puzzle awaits.";
            break;
        case RoomType::SECRET:
            extra = " This place has been hidden for ages, untouched by time.";
            break;
        case RoomType::SAFE:
            extra = " A calm serenity fills the space. You feel safe here.";
            break;
        default:
            extra = "";
    }
    
    return base + extra;
}

RoomType MapGenerator::determineRoomType(int x, int y) {
    // Random determination based on probabilities
    int roll = randomInt(1, 100);
    int cumulative = 0;
    
    cumulative += BOSS_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::BOSS;
    
    cumulative += TREASURE_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::TREASURE;
    
    cumulative += TRAP_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::TRAP;
    
    cumulative += PUZZLE_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::PUZZLE;
    
    cumulative += SAFE_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::SAFE;
    
    return RoomType::NORMAL;
}

// ---
// PRIVATE METHODS - Population
// ---

void MapGenerator::populateWithEnemies(Room& room, const std::vector<Enemy>& templates) {
    // Skip certain room types
    if (room.type == RoomType::SAFE || room.type == RoomType::ENTRANCE || 
        room.type == RoomType::EXIT || room.type == RoomType::TREASURE) {
        return;
    }
    
    if (templates.empty()) return;
    
    int enemyCount = 0;
    bool needsBoss = false;
    
    switch (room.type) {
        case RoomType::BOSS:
            needsBoss = true;
            enemyCount = 1;
            break;
        case RoomType::NORMAL:
            if (randomChance(60)) {
                enemyCount = randomInt(1, 3);
            }
            break;
        case RoomType::TRAP:
            if (randomChance(40)) {
                enemyCount = randomInt(1, 2);
            }
            break;
        case RoomType::SECRET:
            if (randomChance(30)) {
                enemyCount = 1;  // Rare guardian
            }
            break;
        default:
            break;
    }
    
    for (int i = 0; i < enemyCount; ++i) {
        // Find appropriate enemy
        std::vector<const Enemy*> validEnemies;
        for (const Enemy& e : templates) {
            if (needsBoss && e.isBoss) {
                validEnemies.push_back(&e);
            } else if (!needsBoss && !e.isBoss) {
                validEnemies.push_back(&e);
            }
        }
        
        if (!validEnemies.empty()) {
            int idx = randomInt(0, static_cast<int>(validEnemies.size()) - 1);
            Enemy enemy = *validEnemies[idx];  // Copy the template
            room.enemies.push_back(enemy);
        }
    }
}

void MapGenerator::populateWithItems(Room& room, const std::vector<Item>& templates) {
    if (templates.empty()) return;
    
    int itemCount = 0;
    
    switch (room.type) {
        case RoomType::TREASURE:
            itemCount = randomInt(2, 4);
            break;
        case RoomType::SECRET:
            itemCount = randomInt(1, 3);
            break;
        case RoomType::NORMAL:
            if (randomChance(30)) {
                itemCount = 1;
            }
            break;
        case RoomType::BOSS:
            // Boss loot handled in combat
            break;
        default:
            if (randomChance(15)) {
                itemCount = 1;
            }
            break;
    }
    
    // Higher rarity in special rooms
    int rarityBoost = 0;
    if (room.type == RoomType::SECRET) rarityBoost = 2;
    if (room.type == RoomType::TREASURE) rarityBoost = 1;
    
    for (int i = 0; i < itemCount; ++i) {
        // Weight by rarity
        std::vector<const Item*> validItems;
        for (const Item& item : templates) {
            int rarity = static_cast<int>(item.rarity) + rarityBoost;
            if (rarity >= static_cast<int>(ItemRarity::LEGENDARY)) {
                rarity = static_cast<int>(ItemRarity::LEGENDARY);
            }
            
            // Add item multiple times based on inverse rarity (common items appear more)
            int weight = 5 - static_cast<int>(item.rarity);
            for (int w = 0; w < weight; ++w) {
                validItems.push_back(&item);
            }
        }
        
        if (!validItems.empty()) {
            int idx = randomInt(0, static_cast<int>(validItems.size()) - 1);
            Item item = *validItems[idx];  // Copy
            item.quantity = 1;
            room.items.push_back(item);
        }
    }
}

void MapGenerator::assignEvents(Room& room, const std::vector<Event>& templates) {
    // Events are referenced by ID, actual handling done during gameplay
    if (templates.empty()) return;
    
    // Add random events based on room type
    for (const Event& evt : templates) {
        bool shouldAdd = false;
        
        switch (room.type) {
            case RoomType::NORMAL:
                if (evt.type == EventType::STORY || evt.type == EventType::CHOICE) {
                    shouldAdd = randomChance(20);
                }
                break;
            case RoomType::TRAP:
                if (evt.type == EventType::TRAP) {
                    shouldAdd = randomChance(80);
                }
                break;
            case RoomType::PUZZLE:
                if (evt.type == EventType::RIDDLE) {
                    shouldAdd = randomChance(90);
                }
                break;
            default:
                break;
        }
        
        if (shouldAdd) {
            room.eventIds.push_back(evt.id);
        }
    }
}

void MapGenerator::setupPuzzle(Room& room) {
    if (RIDDLE_DATABASE.empty()) return;
    
    // Pick a random riddle
    int idx = randomInt(0, static_cast<int>(RIDDLE_DATABASE.size()) - 1);
    const Riddle& riddle = RIDDLE_DATABASE[idx];
    
    room.puzzleQuestion = riddle.question;
    room.puzzleAnswer = toLower(riddle.answer);
    room.puzzleSolved = false;
    room.puzzleRewardItemId = randomInt(1, 10);  // Random reward item ID
}

void MapGenerator::setupTrap(Room& room) {
    room.isTrapActive = true;
    room.trapDamage = randomInt(10, 30);
    
    // Add trap description
    std::vector<std::string> trapDescs = {
        "Poison darts shoot from the walls!",
        "The floor gives way to spikes!",
        "A swinging blade descends from the ceiling!",
        "Flames erupt from hidden vents!",
        "A heavy stone block drops from above!"
    };
    
    room.description += " " + randomElement(trapDescs);
}

// ---
// PRIVATE METHODS - Validation
// ---

bool MapGenerator::isValidPosition(int x, int y) const {
    return x >= 0 && x < mapWidth && y >= 0 && y < mapHeight;
}

bool MapGenerator::hasRoom(int x, int y) const {
    if (!isValidPosition(x, y)) return false;
    return grid[y][x] >= 0;
}

bool MapGenerator::verifyConnectivity() {
    if (rooms.empty()) return false;
    
    // BFS from entrance to verify all rooms reachable
    std::set<int> visited;
    std::queue<int> toVisit;
    toVisit.push(entranceId);
    
    while (!toVisit.empty()) {
        int currentId = toVisit.front();
        toVisit.pop();
        
        if (visited.count(currentId)) continue;
        visited.insert(currentId);
        
        // Find current room
        for (const Room& room : rooms) {
            if (room.id == currentId) {
                if (room.hasNorth && !visited.count(room.northRoomId)) {
                    toVisit.push(room.northRoomId);
                }
                if (room.hasSouth && !visited.count(room.southRoomId)) {
                    toVisit.push(room.southRoomId);
                }
                if (room.hasEast && !visited.count(room.eastRoomId)) {
                    toVisit.push(room.eastRoomId);
                }
                if (room.hasWest && !visited.count(room.westRoomId)) {
                    toVisit.push(room.westRoomId);
                }
                break;
            }
        }
    }
    
    // Check if exit is reachable
    return visited.count(exitId) > 0;
}

std::vector<std::pair<int,int>> MapGenerator::getAdjacentEmpty(int x, int y) {
    std::vector<std::pair<int,int>> result;
    
    if (isValidPosition(x+1, y) && !hasRoom(x+1, y)) result.push_back({x+1, y});
    if (isValidPosition(x-1, y) && !hasRoom(x-1, y)) result.push_back({x-1, y});
    if (isValidPosition(x, y+1) && !hasRoom(x, y+1)) result.push_back({x, y+1});
    if (isValidPosition(x, y-1) && !hasRoom(x, y-1)) result.push_back({x, y-1});
    
    return result;
}

std::vector<int> MapGenerator::getAdjacentRooms(int x, int y) {
    std::vector<int> result;
    
    if (isValidPosition(x+1, y) && hasRoom(x+1, y)) result.push_back(grid[y][x+1]);
    if (isValidPosition(x-1, y) && hasRoom(x-1, y)) result.push_back(grid[y][x-1]);
    if (isValidPosition(x, y+1) && hasRoom(x, y+1)) result.push_back(grid[y+1][x]);
    if (isValidPosition(x, y-1) && hasRoom(x, y-1)) result.push_back(grid[y-1][x]);
    
    return result;
}

// ---
// STANDALONE HELPER FUNCTIONS
// ---

bool generateDungeon(GameState& gameState, int width, int height) {
    MapGenerator generator(width, height);
    return generator.generateMap(gameState);
}

std::vector<Room> loadRoomTemplates(const std::string& filename) {
    std::vector<Room> templates;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        // Return empty vector if file doesn't exist
        return templates;
    }
    
    std::string line;
    Room current;
    bool inRoom = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') continue;
        
        if (line == "[ROOM]") {
            if (inRoom) {
                templates.push_back(current);
            }
            current = Room();
            inRoom = true;
            continue;
        }
        
        if (!inRoom) continue;
        
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));
            
            if (key == "name") current.name = value;
            else if (key == "description") current.description = value;
            else if (key == "type") {
                if (value == "normal") current.type = RoomType::NORMAL;
                else if (value == "treasure") current.type = RoomType::TREASURE;
                else if (value == "boss") current.type = RoomType::BOSS;
                else if (value == "trap") current.type = RoomType::TRAP;
                else if (value == "puzzle") current.type = RoomType::PUZZLE;
                else if (value == "secret") current.type = RoomType::SECRET;
                else if (value == "safe") current.type = RoomType::SAFE;
            }
        }
    }
    
    if (inRoom) {
        templates.push_back(current);
    }
    
    file.close();
    return templates;
}

Room* getRoomById(GameState& gameState, int roomId) {
    auto it = gameState.roomMap.find(roomId);
    if (it != gameState.roomMap.end()) {
        return it->second;
    }
    
    // Fallback to linear search
    for (Room& room : gameState.dungeon) {
        if (room.id == roomId) {
            return &room;
        }
    }
    return nullptr;
}

Room* getRoomAtPosition(GameState& gameState, int x, int y) {
    for (Room& room : gameState.dungeon) {
        if (room.x == x && room.y == y) {
            return &room;
        }
    }
    return nullptr;
}

}  // namespace AQE
