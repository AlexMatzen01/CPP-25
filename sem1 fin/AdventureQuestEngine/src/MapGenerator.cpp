//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Procedural map generation implementation

#include "../include/MapGenerator.h"
#include "../include/Utils.h"
#include "../include/Constants.h"
#include <queue>
#include <set>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

namespace AQE {

// ---
// CONSTRUCTOR
// ---

MapGenerator::MapGenerator(int width, int height)
    : mapWidth(width), mapHeight(height), entranceId(-1), exitId(-1), nextRoomId(0), forgeCount(0), mawModeActive(false), mawIntensityLevel(0) {
    // Clamp dimensions to valid range
    if (mapWidth < MIN_MAP_WIDTH) mapWidth = MIN_MAP_WIDTH;
    if (mapWidth > MAX_MAP_WIDTH) mapWidth = MAX_MAP_WIDTH;
    if (mapHeight < MIN_MAP_HEIGHT) mapHeight = MIN_MAP_HEIGHT;
    if (mapHeight > MAX_MAP_HEIGHT) mapHeight = MAX_MAP_HEIGHT;
}

// ---
// PUBLIC METHODS
// ---

bool MapGenerator::generateMap(GameState& gameState, bool mawMode) {
    mawModeActive = mawMode;
    mawIntensityLevel = mawModeActive ? std::max(1, gameState.currentDungeonLevel) : 0;

    // Store map dimensions in game state
    gameState.mapWidth = mapWidth;
    gameState.mapHeight = mapHeight;
    
    // Clear previous data
    rooms.clear();
    initializeGrid();
    forgeCount = 0;  // Reset forge counter for this generation
    
    // Step 1: Place entrance at bottom-left area
    entranceId = placeEntranceRoom();
    if (entranceId < 0) return false;

    // Step 1.5: Place a basic shop next to the entrance
    if (!placeBasicShopNextToEntrance()) return false;
    
    // Step 1.6: Place a forge next to the entrance
    if (!placeForgeNextToEntrance()) return false;
    
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
    
    // Step 6: Place the unique Shadow Lord chamber as far from the entrance as possible
    if (!mawModeActive && !placeShadowLordRoom()) return false;
    if (mawModeActive) ensureMawExitRoom();
    
    // Step 7: Connect all adjacent rooms
    connectRooms();
    
    // Step 8: Verify connectivity
    if (!verifyConnectivity()) {
        // If not connected, regenerate (shouldn't happen with proper algorithm)
        return generateMap(gameState, mawMode);
    }
    
    // Step 9: Populate rooms with content
    const int dungeonLevel = std::max(1, gameState.currentDungeonLevel);
    for (Room& room : rooms) {
        populateWithEnemies(room, gameState.enemyTemplates, dungeonLevel, mawModeActive, gameState.player.level);
        populateWithItems(room, gameState.itemTemplates, mawModeActive, gameState.player.level);
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

const std::vector<std::vector<int>>& MapGenerator::getGrid() const {
    return grid;
}

void MapGenerator::syncFromDungeon(const std::vector<Room>& dungeon) {
    rooms = dungeon;

    grid.assign(mapHeight, std::vector<int>(mapWidth, -1));
    entranceId = -1;
    exitId = -1;
    nextRoomId = 0;
    forgeCount = 0;

    int fallbackExitId = -1;

    for (const Room& room : rooms) {
        if (isValidPosition(room.x, room.y)) {
            grid[room.y][room.x] = room.id;
        }

        nextRoomId = std::max(nextRoomId, room.id + 1);

        if (room.type == RoomType::ENTRANCE) {
            entranceId = room.id;
        } else if (room.type == RoomType::SHADOWLORD) {
            exitId = room.id;
        } else if (room.type == RoomType::EXIT && fallbackExitId < 0) {
            fallbackExitId = room.id;
        } else if (room.type == RoomType::FORGE) {
            ++forgeCount;
        }
    }

    if (exitId < 0) {
        exitId = fallbackExitId;
    }
}

const Room* MapGenerator::findRoomById(int roomId) const {
    for (const Room& room : rooms) {
        if (room.id == roomId) {
            return &room;
        }
    }
    return nullptr;
}

void MapGenerator::printMap(int currentRoomId) const {
    std::cout << "\n";
    displaySeparator(80, '=');
    printColor(centerString("◆◆◆ DUNGEON MAP ◆◆◆", 80), Color::BRIGHT_CYAN);
    displaySeparator(80, '=');
    std::cout << "\n";
    
    // Print coordinate numbers on top
    std::cout << "    ";
    for (int x = 0; x < mapWidth; ++x) {
        std::cout << "  " << colorize(std::to_string(x), Color::DARK_GRAY) << " ";
    }
    std::cout << "\n";
    
    for (int y = mapHeight - 1; y >= 0; --y) {
        // Print top border of row with fancy corners
        std::cout << "    ";
        for (int x = 0; x < mapWidth; ++x) {
            if (grid[y][x] >= 0) {
                // Highlight current player room with special border
                if (grid[y][x] == currentRoomId) {
                    std::cout << colorize("┏━━━", Color::BRIGHT_GREEN);
                } else {
                    std::cout << colorize("┌───", Color::BRIGHT_BLACK);
                }
            } else {
                std::cout << "    ";
            }
        }
        std::cout << "\n";
        
        // Print room content with color-coded symbols
        std::cout << " " << colorize(std::to_string(y), Color::DARK_GRAY) << "  ";
        for (int x = 0; x < mapWidth; ++x) {
            if (grid[y][x] >= 0) {
                int roomId = grid[y][x];
                const Room* room = findRoomById(roomId);
                
                if (room) {
                    std::string symbol;
                    std::string color;
                    
                    switch (room->type) {
                        case RoomType::ENTRANCE:
                            symbol = " ⌂ ";
                            color = Color::BRIGHT_GREEN;
                            break;
                        case RoomType::EXIT:
                            symbol = " ☆ ";
                            color = Color::BRIGHT_YELLOW;
                            break;
                        case RoomType::SHADOWLORD:
                            symbol = " ☠ ";
                            color = Color::BRIGHT_MAGENTA;
                            break;
                        case RoomType::BOSS:
                            symbol = " ⚔ ";
                            color = Color::BRIGHT_RED;
                            break;
                        case RoomType::TREASURE:
                            symbol = " $ ";
                            color = Color::YELLOW;
                            break;
                        case RoomType::TRAP:
                            symbol = " ⚠ ";
                            color = Color::RED;
                            break;
                        case RoomType::PUZZLE:
                            symbol = " Ω ";
                            color = Color::MAGENTA;
                            break;
                        case RoomType::SECRET:
                            symbol = " ◊ ";
                            color = Color::CYAN;
                            break;
                        case RoomType::SAFE:
                            symbol = " ♥ ";
                            color = Color::GREEN;
                            break;
                        case RoomType::SHOP:
                            symbol = " ◘ ";
                            color = Color::BRIGHT_MAGENTA;
                            break;
                        case RoomType::LIBRARY:
                            symbol = " ▤ ";
                            color = Color::BRIGHT_BLUE;
                            break;
                        case RoomType::FORGE:
                            symbol = " ⚒ ";
                            color = Color::BRIGHT_RED;
                            break;
                        case RoomType::SHRINE:
                            symbol = " ⛩ ";
                            color = Color::BRIGHT_YELLOW;
                            break;
                        case RoomType::LABORATORY:
                            symbol = " ⚗ ";
                            color = Color::BRIGHT_GREEN;
                            break;
                        case RoomType::GRAVEYARD:
                            symbol = " ⚰ ";
                            color = Color::DARK_GRAY;
                            break;
                        case RoomType::ARMORY:
                            symbol = " ⛨ ";
                            color = Color::BRIGHT_WHITE;
                            break;
                        case RoomType::PRISON:
                            symbol = " ⛓ ";
                            color = Color::RED;
                            break;
                        default:
                            if (room->enemies.empty() && room->isCleared) {
                                symbol = " · ";
                                color = Color::DARK_GRAY;
                            } else if (room->enemies.empty()) {
                                symbol = " ○ ";
                                color = Color::WHITE;
                            } else {
                                symbol = " • ";
                                color = Color::BRIGHT_WHITE;
                            }
                            break;
                    }
                    
                    // Highlight current player room
                    if (room->id == currentRoomId) {
                        // Use bright green border and bold symbol for player location
                        std::cout << colorize("┃", Color::BRIGHT_GREEN) << "\033[7m" << colorize(symbol, color) << "\033[27m";
                    } else {
                        std::cout << colorize("│", Color::BRIGHT_BLACK) << colorize(symbol, color);
                    }
                } else {
                    std::cout << "│ ? ";
                }
            } else {
                std::cout << "    ";
            }
        }
        std::cout << "\n";
        
        // Print bottom border
        std::cout << "    ";
        for (int x = 0; x < mapWidth; ++x) {
            if (grid[y][x] >= 0) {
                // Highlight current player room with special border
                if (grid[y][x] == currentRoomId) {
                    std::cout << colorize("┗━━━", Color::BRIGHT_GREEN);
                } else {
                    std::cout << colorize("└───", Color::BRIGHT_BLACK);
                }
            } else {
                std::cout << "    ";
            }
        }
        std::cout << "\n";
    }
    
    std::cout << "\n";
    displaySeparator(80, '-');
    
    // Enhanced legend with colors and symbols
    std::cout << "\n" << colorize("◆ LEGEND", Color::BRIGHT_CYAN) << "\n\n";
    
    std::cout << "  " << colorize("⌂", Color::BRIGHT_GREEN) << " Entrance (Start)      ";
    std::cout << colorize("☆", Color::BRIGHT_YELLOW) << " Exit (Legacy)         ";
    std::cout << colorize("☠", Color::BRIGHT_MAGENTA) << " Shadow Lord Chamber\n";

    std::cout << "  " << colorize("⚔", Color::BRIGHT_RED) << " Boss Room             ";
    std::cout << colorize("$", Color::YELLOW) << " Treasure Chest        ";
    std::cout << colorize("⚠", Color::RED) << " Dangerous Trap\n";
    
    std::cout << "  " << colorize("Ω", Color::MAGENTA) << " Puzzle Challenge      ";
    std::cout << colorize("◊", Color::CYAN) << " Secret Room           ";
    std::cout << colorize("♥", Color::GREEN) << " Safe Rest Area\n";
    
    std::cout << "  " << colorize("◘", Color::BRIGHT_MAGENTA) << " Merchant Shop         ";
    std::cout << colorize("▤", Color::BRIGHT_BLUE) << " Library (XP)          ";
    std::cout << colorize("⚒", Color::BRIGHT_RED) << " Forge (Crafting)\n";
    
    std::cout << "  " << colorize("⛩", Color::BRIGHT_YELLOW) << " Shrine (Blessing)     ";
    std::cout << colorize("⚗", Color::BRIGHT_GREEN) << " Laboratory (Potions)  ";
    std::cout << colorize("⚰", Color::DARK_GRAY) << " Graveyard (Undead)\n";
    
    std::cout << "  " << colorize("⛨", Color::BRIGHT_WHITE) << " Armory (Gear)         ";
    std::cout << colorize("⛓", Color::RED) << " Prison (Rescue)       ";
    std::cout << colorize("•", Color::BRIGHT_WHITE) << " Room (Unexplored)\n";
    
    std::cout << "  " << colorize("○", Color::WHITE) << " Room (Explored)       ";
    std::cout << colorize("·", Color::DARK_GRAY) << " Room (Cleared)\n\n";
    
    std::cout << "\n";
    displaySeparator(80, '=');
    std::cout << "\n";
}

void MapGenerator::printMawMap(int currentRoomId, int stratum, bool fogOfWar) const {
    std::cout << "\n";
    displaySeparator(80, '=');
    printColor(centerString("◆◆◆ THE MAW CARTOGRAPH ◆◆◆", 80), Color::BRIGHT_MAGENTA);
    displaySeparator(80, '=');
    std::cout << "\n";
    std::cout << "  Stratum: " << colorize(std::to_string(std::max(1, stratum)), Color::BRIGHT_MAGENTA)
              << "    Fog of War: " << colorize(fogOfWar ? "ACTIVE" : "DISABLED", Color::BRIGHT_CYAN)
              << "\n\n";

    std::cout << "    ";
    for (int x = 0; x < mapWidth; ++x) {
        std::cout << "  " << colorize(std::to_string(x), Color::DARK_GRAY) << " ";
    }
    std::cout << "\n";

    for (int y = mapHeight - 1; y >= 0; --y) {
        std::cout << "    ";
        for (int x = 0; x < mapWidth; ++x) {
            if (grid[y][x] >= 0) {
                if (grid[y][x] == currentRoomId) {
                    std::cout << colorize("╔═══", Color::BRIGHT_GREEN);
                } else {
                    std::cout << colorize("╭───", Color::MAGENTA);
                }
            } else {
                std::cout << "    ";
            }
        }
        std::cout << "\n";

        std::cout << " " << colorize(std::to_string(y), Color::DARK_GRAY) << "  ";
        for (int x = 0; x < mapWidth; ++x) {
            if (grid[y][x] < 0) {
                std::cout << "    ";
                continue;
            }

            const Room* room = findRoomById(grid[y][x]);
            if (!room) {
                std::cout << colorize("│ ? ", Color::BRIGHT_BLACK);
                continue;
            }

            const bool isCurrentRoom = room->id == currentRoomId;
            const bool visible = !fogOfWar || room->isExplored || isCurrentRoom;

            if (!visible) {
                std::cout << colorize("│", Color::MAGENTA)
                          << colorize(" ▒ ", Color::BRIGHT_BLACK);
                continue;
            }

            std::string symbol;
            std::string color;

            switch (room->type) {
                case RoomType::ENTRANCE:
                    symbol = " ⌂ ";
                    color = Color::BRIGHT_CYAN;
                    break;
                case RoomType::EXIT:
                    symbol = " ⌯ ";
                    color = Color::BRIGHT_MAGENTA;
                    break;
                case RoomType::BOSS:
                    symbol = " ☣ ";
                    color = Color::BRIGHT_RED;
                    break;
                case RoomType::TREASURE:
                    symbol = " ✧ ";
                    color = Color::BRIGHT_YELLOW;
                    break;
                case RoomType::TRAP:
                    symbol = " ⛧ ";
                    color = Color::RED;
                    break;
                case RoomType::PUZZLE:
                    symbol = " ⌘ ";
                    color = Color::CYAN;
                    break;
                case RoomType::SECRET:
                    symbol = " ✶ ";
                    color = Color::BRIGHT_CYAN;
                    break;
                case RoomType::SAFE:
                    symbol = " ◇ ";
                    color = Color::GREEN;
                    break;
                case RoomType::SHOP:
                    symbol = " § ";
                    color = Color::YELLOW;
                    break;
                case RoomType::FORGE:
                    symbol = " ⚒ ";
                    color = Color::BRIGHT_RED;
                    break;
                case RoomType::SHRINE:
                    symbol = " ☾ ";
                    color = Color::BRIGHT_YELLOW;
                    break;
                case RoomType::LABORATORY:
                    symbol = " ⚗ ";
                    color = Color::BRIGHT_GREEN;
                    break;
                case RoomType::GRAVEYARD:
                    symbol = " ⚱ ";
                    color = Color::DARK_GRAY;
                    break;
                case RoomType::ARMORY:
                    symbol = " ⛨ ";
                    color = Color::BRIGHT_WHITE;
                    break;
                case RoomType::PRISON:
                    symbol = " ⛓ ";
                    color = Color::RED;
                    break;
                default:
                    if (room->enemies.empty() && room->isCleared) {
                        symbol = " · ";
                        color = Color::DARK_GRAY;
                    } else if (room->enemies.empty()) {
                        symbol = " ○ ";
                        color = Color::WHITE;
                    } else {
                        symbol = " • ";
                        color = Color::BRIGHT_WHITE;
                    }
                    break;
            }

            if (isCurrentRoom) {
                std::cout << colorize("║", Color::BRIGHT_GREEN)
                          << "\033[7m" << colorize(symbol, color) << "\033[27m";
            } else {
                std::cout << colorize("│", Color::MAGENTA)
                          << colorize(symbol, color);
            }
        }
        std::cout << "\n";

        std::cout << "    ";
        for (int x = 0; x < mapWidth; ++x) {
            if (grid[y][x] >= 0) {
                if (grid[y][x] == currentRoomId) {
                    std::cout << colorize("╚═══", Color::BRIGHT_GREEN);
                } else {
                    std::cout << colorize("╰───", Color::MAGENTA);
                }
            } else {
                std::cout << "    ";
            }
        }
        std::cout << "\n";
    }

    std::cout << "\n";
    displaySeparator(80, '-');

    std::cout << "\n" << colorize("◆ MAW LEGEND", Color::BRIGHT_MAGENTA) << "\n\n";
    std::cout << "  " << colorize("⌂", Color::BRIGHT_CYAN) << " Breach Start          ";
    std::cout << colorize("⌯", Color::BRIGHT_MAGENTA) << " Descent Rift (Next)   ";
    std::cout << colorize("☣", Color::BRIGHT_RED) << " Apex Predator\n";
    std::cout << "  " << colorize("✧", Color::BRIGHT_YELLOW) << " Relic Cache           ";
    std::cout << colorize("⛧", Color::RED) << " Hazard Nexus          ";
    std::cout << colorize("⌘", Color::CYAN) << " Glyph Trial\n";
    std::cout << "  " << colorize("✶", Color::BRIGHT_CYAN) << " Hidden Vein           ";
    std::cout << colorize("◇", Color::GREEN) << " Calm Pocket           ";
    std::cout << colorize("▒", Color::BRIGHT_BLACK) << " Fog (Unseen Room)\n";
    std::cout << "  " << colorize("§", Color::YELLOW) << " Bazaar                ";
    std::cout << colorize("⚒", Color::BRIGHT_RED) << " Forge                 ";
    std::cout << colorize("☾", Color::BRIGHT_YELLOW) << " Void Altar\n";
    std::cout << "  " << colorize("⚗", Color::BRIGHT_GREEN) << " Fleshcraft Lab        ";
    std::cout << colorize("⚱", Color::DARK_GRAY) << " Bonefield             ";
    std::cout << colorize("⛓", Color::RED) << " Echo Cells\n";
    std::cout << "  " << colorize("•", Color::BRIGHT_WHITE) << " Hostile Room          ";
    std::cout << colorize("○", Color::WHITE) << " Explored Room         ";
    std::cout << colorize("·", Color::DARK_GRAY) << " Cleared Room\n";
    std::cout << "\n";
    displaySeparator(80, '=');
    std::cout << "\n";
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
    if (mawModeActive) {
        entrance.name = "Maw Breach";
        entrance.description = "A wound in reality seals behind you as void pressure settles over the chamber. "
                              "This is the last stable foothold before the abyss starts rewriting itself.";
    } else {
        entrance.name = "Dungeon Entrance";
        entrance.description = "You stand at the entrance of a dark, foreboding dungeon. "
                              "Cold air flows from within, carrying the scent of ancient stone and forgotten secrets. "
                              "There's no turning back now.";
    }
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
    if (mawModeActive) {
        exitRoom.name = "Descent Rift";
        exitRoom.description = "A jagged rift vibrates with downward pull. "
                               "Crossing it will collapse this layout and drag you into a deeper stratum.";
    } else {
        exitRoom.name = "Chamber of Escape";
        exitRoom.description = "A grand chamber stretches before you, ancient runes glowing on the walls. "
                              "In the center stands a magnificent portal, swirling with ethereal light. "
                              "This is the exit - your path to freedom and glory!";
    }
    
    rooms.push_back(exitRoom);
    grid[y][x] = exitRoom.id;
    
    return exitRoom.id;
}

bool MapGenerator::placeShadowLordRoom() {
    int entranceX = -1;
    int entranceY = -1;

    for (const Room& room : rooms) {
        if (room.id == entranceId) {
            entranceX = room.x;
            entranceY = room.y;
            break;
        }
    }

    if (entranceX < 0 || entranceY < 0) {
        return false;
    }

    int bestIndex = -1;
    int bestDistance = -1;
    int bestAdjacency = -1;

    for (size_t i = 0; i < rooms.size(); ++i) {
        const Room& room = rooms[i];

        if (room.id == entranceId || room.type == RoomType::SHOP || room.type == RoomType::FORGE) {
            continue;
        }

        const int adjacencyCount = static_cast<int>(getAdjacentRooms(room.x, room.y).size());
        if (adjacencyCount <= 0) {
            continue;
        }

        const bool isEndpointCandidate = (adjacencyCount <= 1);
        const int distance = std::abs(room.x - entranceX) + std::abs(room.y - entranceY);

        bool better = false;
        if (bestIndex < 0) {
            better = true;
        } else {
            const Room& bestRoom = rooms[bestIndex];
            const bool bestIsEndpoint = (bestAdjacency <= 1);

            if (isEndpointCandidate != bestIsEndpoint) {
                better = isEndpointCandidate;
            } else if (distance != bestDistance) {
                better = (distance > bestDistance);
            } else if (room.y != bestRoom.y) {
                better = (room.y > bestRoom.y);
            } else if (room.x != bestRoom.x) {
                better = (room.x > bestRoom.x);
            } else {
                better = (room.id > bestRoom.id);
            }
        }

        if (better) {
            bestIndex = static_cast<int>(i);
            bestDistance = distance;
            bestAdjacency = adjacencyCount;
        }
    }

    if (bestIndex < 0) {
        for (size_t i = 0; i < rooms.size(); ++i) {
            if (rooms[i].id == exitId) {
                bestIndex = static_cast<int>(i);
                break;
            }
        }
    }

    if (bestIndex < 0) {
        return false;
    }

    const int shadowRoomId = rooms[bestIndex].id;

    for (Room& room : rooms) {
        if (room.id == shadowRoomId) {
            continue;
        }

        if (room.type == RoomType::SHADOWLORD || room.type == RoomType::EXIT) {
            room.type = RoomType::NORMAL;
            room.name = generateRoomName(RoomType::NORMAL);
            room.description = generateRoomDescription(RoomType::NORMAL);
            room.shortDesc = "A normal room you've visited before.";
        }
    }

    Room& shadowRoom = rooms[bestIndex];
    shadowRoom.type = RoomType::SHADOWLORD;
    shadowRoom.name = "Shadow Lord Chamber";
    shadowRoom.shortDesc = "The oppressive final chamber where the Shadow Lord was sealed.";
    shadowRoom.description = "A colossal obsidian hall drenched in cold shadow. Ancient seals carved into the "
                             "floor flicker and crackle around a throne of black stone. This is the Shadow Lord's "
                             "chamber - the final threshold of your journey.";

    exitId = shadowRoom.id;
    return true;
}

bool MapGenerator::placeBasicShopNextToEntrance() {
    // Find entrance position
    int startX = -1, startY = -1;
    for (const Room& room : rooms) {
        if (room.id == entranceId) {
            startX = room.x;
            startY = room.y;
            break;
        }
    }
    
    if (startX < 0) return false;
    
    // Try to find an empty adjacent spot
    std::vector<std::pair<int, int>> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    
    // Simple shuffle using randomInt
    for (int i = 0; i < 4; ++i) {
        int j = randomInt(0, 3);
        std::swap(directions[i], directions[j]);
    }
    
    for (auto [dx, dy] : directions) {
        int nx = startX + dx;
        int ny = startY + dy;
        
        if (isValidPosition(nx, ny) && !hasRoom(nx, ny)) {
            Room shop = createRoom(nx, ny, RoomType::SHOP);
            shop.name = "Basic Equipment Shop";
            shop.description = "A small, tidy shop stands here, conveniently located near the dungeon entrance. "
                               "The merchant inside specializes in basic gear for new adventurers.";
            rooms.push_back(shop);
            grid[ny][nx] = shop.id;
            return true;
        }
    }
    
    return false; // Should rarely happen
}

bool MapGenerator::placeForgeNextToEntrance() {
    // Find entrance position
    int startX = -1, startY = -1;
    for (const Room& room : rooms) {
        if (room.id == entranceId) {
            startX = room.x;
            startY = room.y;
            break;
        }
    }
    
    if (startX < 0) return false;
    
    // Try to find an empty adjacent spot (prefer diagonal or farther from shop)
    std::vector<std::pair<int, int>> directions = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    
    for (auto [dx, dy] : directions) {
        int nx = startX + dx;
        int ny = startY + dy;
        
        if (isValidPosition(nx, ny) && !hasRoom(nx, ny)) {
            Room forge = createRoom(nx, ny, RoomType::FORGE);
            forge.name = "Entrance Forge";
            forge.description = "A well-maintained forge stands near the dungeon entrance. The blacksmith here "
                                "offers essential weapon and armor upgrades for adventurers beginning their journey.";
            rooms.push_back(forge);
            grid[ny][nx] = forge.id;
            forgeCount++;
            return true;
        }
    }
    
    return false; // Should rarely happen
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
            RoomType type = determineRoomType(newX, newY, true);
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
    // Random fill with higher probability to create denser maps
    int fillChance = 65;  // Base chance to fill empty cell (increased from 40 for more density)
    
    // Collect all candidates first
    std::vector<std::pair<int, int>> allCandidates;
    
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if (!hasRoom(x, y)) {
                std::vector<int> adjacent = getAdjacentRooms(x, y);
                if (!adjacent.empty()) {
                    allCandidates.push_back({x, y});
                }
            }
        }
    }
    
    // Shuffle candidates for better distribution
    for (size_t i = 0; i < allCandidates.size(); ++i) {
        size_t j = randomInt(0, static_cast<int>(allCandidates.size()) - 1);
        std::swap(allCandidates[i], allCandidates[j]);
    }
    
    // Fill rooms with balanced distribution
    for (auto [x, y] : allCandidates) {
        if (!hasRoom(x, y) && randomChance(fillChance)) {
            RoomType type = determineRoomType(x, y);
            
            // Don't place extra forges if we already have enough
            if (type == RoomType::FORGE && forgeCount >= 4) {
                type = RoomType::NORMAL;
            }
            
            Room room = createRoom(x, y, type);
            if (type == RoomType::FORGE) {
                forgeCount++;
            }
            rooms.push_back(room);
            grid[y][x] = room.id;
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
    room.shortDesc = mawModeActive
        ? "The Maw reshapes this chamber, but its scars are still familiar."
        : "A " + toLower(roomTypeToString(type)) + " you've visited before.";
    room.isExplored = false;
    room.isCleared = (type == RoomType::SAFE || type == RoomType::ENTRANCE);
    
    return room;
}

std::string MapGenerator::generateRoomName(RoomType type) {
    if (mawModeActive) {
        switch (type) {
            case RoomType::ENTRANCE: return "Maw Breach";
            case RoomType::EXIT: return "Extraction Rift";
            case RoomType::TREASURE: return "Ruin Cache";
            case RoomType::BOSS: return "Sovereign Crucible";
            case RoomType::TRAP: return "Entropy Snare";
            case RoomType::PUZZLE: return "Echo Sigil Vault";
            case RoomType::SECRET: return "Hidden Null Vein";
            case RoomType::SAFE: return "Stillness Pocket";
            case RoomType::SHOP: return "Abyss Bazaar";
            case RoomType::LIBRARY: return "Whisper Archive";
            case RoomType::FORGE: return "Abyss Forge";
            case RoomType::SHRINE: return "Void Altar";
            case RoomType::LABORATORY: return "Fleshcraft Lab";
            case RoomType::GRAVEYARD: return "Bonefield Hollow";
            case RoomType::ARMORY: return "Relic Arsenal";
            case RoomType::PRISON: return "Chains of Echoes";
            case RoomType::SHADOWLORD: return "Sovereign Rift";
            default: return "Maw Corridor";
        }
    }

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
        case RoomType::LIBRARY:
            return prefix + " Library of " + roomType;
        case RoomType::FORGE:
            return "The " + prefix + " Forge";
        case RoomType::SHRINE:
            return "Ancient Shrine of " + prefix;
        case RoomType::LABORATORY:
            return prefix + " Alchemical Laboratory";
        case RoomType::GRAVEYARD:
            return "The " + prefix + " Graveyard";
        case RoomType::ARMORY:
            return prefix + " Armory";
        case RoomType::PRISON:
            return "The " + prefix + " Prison";
        case RoomType::SHADOWLORD:
            return "Shadow Lord Chamber";
        default:
            return prefix + " " + roomType;
    }
}

std::string MapGenerator::generateRoomDescription(RoomType type) {
    if (mawModeActive) {
        switch (type) {
            case RoomType::ENTRANCE:
                return "The portal seals behind you as voidstone pulses underfoot.";
            case RoomType::EXIT:
                return "A stable rift trembles here. You can extract back to the portal room.";
            case RoomType::TREASURE:
                return "Fractured reliquaries spill relics from failed expeditions.";
            case RoomType::BOSS:
                return "Pressure thickens into malice. A sovereign predator dominates this chamber.";
            case RoomType::TRAP:
                return "The floor is lined with predatory sigils waiting for a careless step.";
            case RoomType::PUZZLE:
                return "Interlocking glyphs hum in dissonant rhythm, demanding precise attunement.";
            case RoomType::SECRET:
                return "This hidden seam in reality shelters contraband of the abyss.";
            case RoomType::SAFE:
                return "For a brief moment, the Maw's hunger recedes and the air goes still.";
            case RoomType::SHOP:
                return "A hooded scavenger trades in void salvage and forbidden materials.";
            case RoomType::LIBRARY:
                return "Floating tablets whisper strategies from heroes who never returned.";
            case RoomType::FORGE:
                return "Black iron tools glow without flame, eager for new signatures of war.";
            case RoomType::SHRINE:
                return "A monolith offers power at a cost only the desperate can afford.";
            case RoomType::LABORATORY:
                return "Vats of volatile ichor churn beside instruments made from bone and glass.";
            case RoomType::GRAVEYARD:
                return "Shattered memorial stones drift in a gravity well of ash and regret.";
            case RoomType::ARMORY:
                return "Weapon racks hold relics warped by endless exposure to the abyss.";
            case RoomType::PRISON:
                return "Cells of black chain hold echoes of captives that are no longer human.";
            case RoomType::SHADOWLORD:
                return "A crown-shaped tear in space marks a throne abandoned by lesser tyrants.";
            default:
                return "The walls flex and breathe as the Maw rewrites this corridor.";
        }
    }

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
        case RoomType::LIBRARY:
            extra = " Towering bookshelves line the walls, filled with ancient knowledge.";
            break;
        case RoomType::FORGE:
            extra = " The heat of a furnace and the sound of an anvil echo through the room.";
            break;
        case RoomType::SHRINE:
            extra = " An ornate altar stands in the center, radiating a strange energy.";
            break;
        case RoomType::LABORATORY:
            extra = " Bubbling beakers and strange smells suggest alchemical experiments.";
            break;
        case RoomType::GRAVEYARD:
            extra = " Rows of weathered tombstones and a chilling mist fill the area.";
            break;
        case RoomType::ARMORY:
            extra = " Racks of weapons and suits of armor are displayed along the walls.";
            break;
        case RoomType::PRISON:
            extra = " Iron bars and heavy chains suggest this was a place of confinement.";
            break;
        case RoomType::SHADOWLORD:
            extra = " The chamber trembles with ancient malice. A final confrontation awaits.";
            break;
        default:
            extra = "";
    }
    
    return base + extra;
}

RoomType MapGenerator::determineRoomType(int x, int y, bool isMainPath) {
    (void)x;
    (void)y;

    if (mawModeActive) {
        std::map<RoomType, int> roomCounts;
        for (const Room& room : rooms) {
            roomCounts[room.type]++;
        }

        const int tier = std::max(1, mawIntensityLevel);
        const int intensityBand = std::min(5, tier / 5);

        struct MawChoice {
            RoomType type;
            int weight;
            int cap;
            bool allowMainPath;
        };

        std::vector<MawChoice> choices = {
            {RoomType::BOSS,       8 + intensityBand * 2, 2 + intensityBand, false},
            {RoomType::TRAP,      14 + intensityBand * 2, 6 + intensityBand * 2, true},
            {RoomType::PRISON,    11 + intensityBand,     5 + intensityBand, true},
            {RoomType::GRAVEYARD, 10 + intensityBand,     5 + intensityBand, true},
            {RoomType::ARMORY,     9 + intensityBand,     4 + intensityBand, true},
            {RoomType::LABORATORY, 7 + intensityBand,     4 + intensityBand, true},
            {RoomType::PUZZLE,     8 + intensityBand,     4 + intensityBand, true},
            {RoomType::TREASURE,   9,                     4 + intensityBand, true},
            {RoomType::SECRET,     6,                     3 + intensityBand / 2, false},
            {RoomType::FORGE,      4,                     2 + intensityBand / 2, true},
            {RoomType::SHRINE,     4,                     2 + intensityBand / 2, true},
            {RoomType::SAFE,       std::max(1, 4 - intensityBand), 2, true},
            {RoomType::LIBRARY,    std::max(1, 3 - intensityBand / 2), 2, true},
        };

        int totalWeight = 0;
        for (const MawChoice& choice : choices) {
            if (!choice.allowMainPath && isMainPath) {
                continue;
            }
            if (roomCounts[choice.type] >= choice.cap) {
                continue;
            }
            totalWeight += std::max(0, choice.weight);
        }

        if (totalWeight > 0) {
            int roll = randomInt(1, totalWeight);
            int cumulative = 0;

            for (const MawChoice& choice : choices) {
                if (!choice.allowMainPath && isMainPath) {
                    continue;
                }
                if (roomCounts[choice.type] >= choice.cap) {
                    continue;
                }

                cumulative += std::max(0, choice.weight);
                if (roll <= cumulative) {
                    return choice.type;
                }
            }
        }

        return RoomType::NORMAL;
    }

    // Count existing rooms to balance distribution
    std::map<RoomType, int> roomCounts;
    for (const Room& room : rooms) {
        roomCounts[room.type]++;
    }
    
    int totalRooms = static_cast<int>(rooms.size());
    int maxRoomTypeCount = std::max(3, totalRooms / 8);  // Limit any one type to ~12.5% of rooms
    
    // Reduce chance for room types that are already well-represented
    int roll = randomInt(1, 100);
    int cumulative = 0;
    
    // Boss rooms should not be on the main path to keep them "on the side"
    if (!isMainPath && roomCounts[RoomType::BOSS] < 2) {
        cumulative += BOSS_ROOM_CHANCE;
        if (roll <= cumulative) return RoomType::BOSS;
    }
    
    cumulative += TREASURE_ROOM_CHANCE;
    if (roomCounts[RoomType::TREASURE] < maxRoomTypeCount && roll <= cumulative) {
        return RoomType::TREASURE;
    }
    
    cumulative += LIBRARY_ROOM_CHANCE;
    if (roomCounts[RoomType::LIBRARY] < maxRoomTypeCount && roll <= cumulative) {
        return RoomType::LIBRARY;
    }
    
    cumulative += GRAVEYARD_ROOM_CHANCE;
    if (roomCounts[RoomType::GRAVEYARD] < maxRoomTypeCount && roll <= cumulative) {
        return RoomType::GRAVEYARD;
    }
    
    cumulative += ARMORY_ROOM_CHANCE;
    if (roomCounts[RoomType::ARMORY] < maxRoomTypeCount && roll <= cumulative) {
        return RoomType::ARMORY;
    }
    
    cumulative += LABORATORY_ROOM_CHANCE;
    if (roomCounts[RoomType::LABORATORY] < maxRoomTypeCount && roll <= cumulative) {
        return RoomType::LABORATORY;
    }
    
    cumulative += SHRINE_ROOM_CHANCE;
    if (roomCounts[RoomType::SHRINE] < maxRoomTypeCount && roll <= cumulative) {
        return RoomType::SHRINE;
    }
    
    cumulative += PRISON_ROOM_CHANCE;
    if (roomCounts[RoomType::PRISON] < maxRoomTypeCount && roll <= cumulative) {
        return RoomType::PRISON;
    }
    
    cumulative += TRAP_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::TRAP;
    
    cumulative += PUZZLE_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::PUZZLE;
    
    cumulative += SAFE_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::SAFE;
    
    cumulative += SECRET_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::SECRET;
    
    return RoomType::NORMAL;
}

// ---
// PRIVATE METHODS - Population
// ---

void MapGenerator::populateWithEnemies(Room& room, const std::vector<Enemy>& templates, int contextLevel, bool mawMode, int playerLevel) {
    (void)contextLevel;

    // Skip certain room types
    if (room.type == RoomType::SAFE || room.type == RoomType::ENTRANCE || 
        room.type == RoomType::EXIT || room.type == RoomType::TREASURE ||
        room.type == RoomType::SHOP || room.type == RoomType::LIBRARY ||
        room.type == RoomType::SHADOWLORD ||
        room.type == RoomType::FORGE || room.type == RoomType::SHRINE) {
        return;
    }
    
    if (templates.empty()) return;
    
    int enemyCount = 0;
    bool needsBoss = false;
    enum class MawEncounterSize {
        WEAK_PACK,
        MEDIUM_PACK,
        STRONG_SINGLE
    };
    MawEncounterSize mawEncounterSize = MawEncounterSize::MEDIUM_PACK;
    
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
        case RoomType::GRAVEYARD:
            enemyCount = randomInt(2, 4);
            break;
        case RoomType::PRISON:
            enemyCount = randomInt(1, 3);
            break;
        case RoomType::ARMORY:
            enemyCount = randomInt(1, 2);
            break;
        case RoomType::LABORATORY:
            if (randomChance(50)) {
                enemyCount = randomInt(1, 2);
            }
            break;
        default:
            break;
    }

    if (mawMode) {
        switch (room.type) {
            case RoomType::BOSS:
                mawEncounterSize = MawEncounterSize::STRONG_SINGLE;
                break;
            case RoomType::SECRET:
            case RoomType::ARMORY:
            case RoomType::LABORATORY:
            case RoomType::PRISON:
            case RoomType::GRAVEYARD:
                mawEncounterSize = MawEncounterSize::MEDIUM_PACK;
                break;
            default:
                mawEncounterSize = MawEncounterSize::WEAK_PACK;
                break;
        }

        switch (mawEncounterSize) {
            case MawEncounterSize::STRONG_SINGLE:
                enemyCount = 1;
                needsBoss = true;
                break;
            case MawEncounterSize::MEDIUM_PACK:
                enemyCount = 2;
                break;
            case MawEncounterSize::WEAK_PACK:
                enemyCount = 3;
                break;
        }
    }

    auto getEnemyPowerScore = [](const Enemy& enemy) {
        const int baseScore = (enemy.stats.maxHP / 10)
            + (enemy.stats.attack * 2)
            + enemy.stats.defense
            + (enemy.stats.magicPower / 2)
            + (enemy.trueDamage * 8)
            + (enemy.isBoss ? 20 : 0);
        return baseScore;
    };

    auto getMawEnemyTier = [&](const Enemy& enemy) {
        const int playerPower = std::max(1, playerLevel);
        const int weakThreshold = playerPower * 8;
        const int mediumThreshold = playerPower * 14;
        const int score = getEnemyPowerScore(enemy);

        if (score <= weakThreshold) {
            return 0;
        }
        if (score <= mediumThreshold) {
            return 1;
        }
        return 2;
    };
    
    for (int i = 0; i < enemyCount; ++i) {
        // Find appropriate enemy
        std::vector<const Enemy*> validEnemies;
        std::vector<const Enemy*> weakEnemies;
        std::vector<const Enemy*> mediumEnemies;
        std::vector<const Enemy*> strongEnemies;
        for (const Enemy& e : templates) {
            if (mawMode && !e.mawOnly) {
                continue;
            }
            if (!mawMode && e.mawOnly) {
                continue;
            }

            if (mawMode) {
                if (e.isBoss) {
                    strongEnemies.push_back(&e);
                } else {
                    const int tier = getMawEnemyTier(e);
                    if (tier == 0) {
                        weakEnemies.push_back(&e);
                    } else if (tier == 1) {
                        mediumEnemies.push_back(&e);
                    } else {
                        strongEnemies.push_back(&e);
                    }
                }
            } else if (needsBoss && e.isBoss) {
                validEnemies.push_back(&e);
            } else if (!needsBoss && !e.isBoss) {
                validEnemies.push_back(&e);
            }
        }

        if (mawMode) {
            if (mawEncounterSize == MawEncounterSize::WEAK_PACK) {
                validEnemies = weakEnemies;
                if (validEnemies.empty()) validEnemies = mediumEnemies;
                if (validEnemies.empty()) validEnemies = strongEnemies;
            } else if (mawEncounterSize == MawEncounterSize::MEDIUM_PACK) {
                validEnemies = mediumEnemies;
                if (validEnemies.empty()) validEnemies = strongEnemies;
                if (validEnemies.empty()) validEnemies = weakEnemies;
            } else {
                validEnemies = strongEnemies;
                if (validEnemies.empty()) validEnemies = mediumEnemies;
                if (validEnemies.empty()) validEnemies = weakEnemies;
            }
        }
        
        if (!validEnemies.empty()) {
            int idx = randomInt(0, static_cast<int>(validEnemies.size()) - 1);
            Enemy enemy = *validEnemies[idx];  // Copy the template

            // Determine effective level for scaling: Maw now follows the player's level directly.
            int effectiveLevel = std::max(1, playerLevel);

            if (mawMode) {
                const float hpScale = 1.15f + (static_cast<float>(effectiveLevel) * 0.055f);
                const float atkScale = 1.10f + (static_cast<float>(effectiveLevel) * 0.045f);
                const float defScale = 1.05f + (static_cast<float>(effectiveLevel) * 0.035f);

                enemy.stats.maxHP = std::max(50, static_cast<int>(enemy.stats.maxHP * hpScale));
                enemy.stats.maxHP = std::max(1, static_cast<int>(enemy.stats.maxHP * 0.65f));
                enemy.stats.currentHP = enemy.stats.maxHP;
                enemy.stats.attack = std::max(10, static_cast<int>(enemy.stats.attack * atkScale));
                enemy.stats.defense = std::max(6, static_cast<int>(enemy.stats.defense * defScale));
                enemy.stats.magicPower = std::max(8, static_cast<int>(enemy.stats.magicPower * atkScale));
                enemy.stats.magicResist = std::max(5, static_cast<int>(enemy.stats.magicResist * defScale));
                enemy.stats.speed = std::max(8, enemy.stats.speed + effectiveLevel / 4);
                enemy.stats.critChance = std::min(MAX_CRIT_CHANCE, enemy.stats.critChance + effectiveLevel / 5);
                enemy.stats.dodgeChance = std::min(MAX_DODGE_CHANCE, enemy.stats.dodgeChance + effectiveLevel / 6);
                enemy.trueDamage = std::max(enemy.trueDamage, 1 + effectiveLevel / 7 + (needsBoss ? 4 : 1));
                enemy.experienceReward += effectiveLevel * (needsBoss ? 20 : 10);
                enemy.goldReward += effectiveLevel * (needsBoss ? 22 : 9);
            } else {
                // Non-Maw: scale enemies primarily with player's level to keep combat balanced
                int levelDelta = std::max(0, playerLevel - 1);
                const float hpScale = 1.0f + levelDelta * 0.06f;   // ~6% HP per level
                const float atkScale = 1.0f + levelDelta * 0.045f; // ~4.5% ATK per level
                const float defScale = 1.0f + levelDelta * 0.035f; // ~3.5% DEF per level

                enemy.stats.maxHP = std::max(20, static_cast<int>(enemy.stats.maxHP * hpScale));
                enemy.stats.currentHP = enemy.stats.maxHP;
                enemy.stats.attack = std::max(6, static_cast<int>(enemy.stats.attack * atkScale));
                enemy.stats.defense = std::max(2, static_cast<int>(enemy.stats.defense * defScale));
                enemy.stats.magicPower = std::max(4, static_cast<int>(enemy.stats.magicPower * atkScale));
                enemy.stats.magicResist = std::max(2, static_cast<int>(enemy.stats.magicResist * defScale));
                enemy.stats.speed = std::max(4, enemy.stats.speed + playerLevel / 4);
                enemy.stats.critChance = std::min(MAX_CRIT_CHANCE, enemy.stats.critChance + playerLevel / 5);
                enemy.stats.dodgeChance = std::min(MAX_DODGE_CHANCE, enemy.stats.dodgeChance + playerLevel / 6);
                enemy.experienceReward += playerLevel * (needsBoss ? 18 : 6);
                enemy.goldReward += playerLevel * (needsBoss ? 18 : 6);

                for (EnemyAbility& ability : enemy.abilities) {
                    ability.damage += playerLevel * (needsBoss ? 6 : 2);
                    ability.healAmount += playerLevel * (needsBoss ? 3 : 1);
                    ability.currentCooldown = 0;
                    ability.triggerChance = std::min(0.8f, ability.triggerChance + (needsBoss ? 0.12f : 0.06f));
                }
            }

            room.enemies.push_back(enemy);
        } else if (mawMode) {
            Enemy fallback;
            fallback.name = needsBoss ? "Maw Sovereign" : "Abyss Wraith";
            fallback.description = "A nightmare entity formed from concentrated void pressure.";
            fallback.isBoss = needsBoss;
            fallback.mawOnly = true;
            const int mawLevel = std::max(1, playerLevel);
            fallback.stats.maxHP = (needsBoss ? 280 : 140) + mawLevel * (needsBoss ? 24 : 12);
            fallback.stats.maxHP = std::max(1, static_cast<int>(fallback.stats.maxHP * 0.65f));
            fallback.stats.currentHP = fallback.stats.maxHP;
            fallback.stats.attack = (needsBoss ? 44 : 24) + mawLevel * (needsBoss ? 3 : 2);
            fallback.stats.defense = (needsBoss ? 18 : 10) + mawLevel;
            fallback.stats.speed = (needsBoss ? 20 : 14) + mawLevel / 4;
            fallback.stats.magicPower = (needsBoss ? 36 : 20) + mawLevel * 2;
            fallback.stats.magicResist = (needsBoss ? 16 : 10) + mawLevel / 2;
            fallback.trueDamage = (needsBoss ? 8 : 3) + mawLevel / 6;
            fallback.experienceReward = (needsBoss ? 420 : 110) + mawLevel * (needsBoss ? 24 : 10);
            fallback.goldReward = (needsBoss ? 350 : 90) + mawLevel * (needsBoss ? 24 : 10);
            room.enemies.push_back(fallback);
        }
    }
}

void MapGenerator::populateWithItems(Room& room, const std::vector<Item>& templates, bool mawMode, int playerLevel) {
    if (templates.empty()) return;

    const int floorLevel = std::max(1, playerLevel);
    
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
        case RoomType::ARMORY:
            itemCount = randomInt(1, 3);
            break;
        case RoomType::LIBRARY:
            if (randomChance(70)) itemCount = 1;
            break;
        case RoomType::LABORATORY:
            if (randomChance(60)) itemCount = randomInt(1, 2);
            break;
        case RoomType::SHRINE:
            if (randomChance(50)) itemCount = 1;
            break;
        case RoomType::FORGE:
            if (randomChance(40)) itemCount = 1;
            break;
        case RoomType::GRAVEYARD:
            if (randomChance(35)) itemCount = 1;
            break;
        case RoomType::PRISON:
            if (randomChance(25)) itemCount = 1;
            break;
        case RoomType::PUZZLE:
            if (randomChance(50)) itemCount = 1;
            break;
        case RoomType::TRAP:
            if (randomChance(20)) itemCount = 1;
            break;
        case RoomType::SAFE:
            // Safe room - minimal items, focus on rest
            if (randomChance(10)) itemCount = 1;
            break;
        case RoomType::SHOP:
            // Shop handled by merchant system
            break;
        case RoomType::ENTRANCE:
        case RoomType::EXIT:
        case RoomType::SHADOWLORD:
            // No items at entrance/exit
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
    if (room.type == RoomType::ARMORY) rarityBoost = 1;
    if (room.type == RoomType::LIBRARY) rarityBoost = 1;
    if (room.type == RoomType::SHRINE) rarityBoost = 1;
    if (room.type == RoomType::GRAVEYARD) rarityBoost = 1;
    
    // Track used templates to prevent duplicates
    std::set<const Item*> usedTemplates;
    
    for (int i = 0; i < itemCount; ++i) {
        // Weight by rarity, excluding non-stackable duplicates
        std::vector<const Item*> validItems;
        for (const Item& item : templates) {
            if (mawMode && !item.mawOnly) {
                continue;
            }
            if (!mawMode && item.mawOnly) {
                continue;
            }
            
            // Skip non-stackable items that have already been used
            if (!item.isStackable && usedTemplates.count(&item) > 0) {
                continue;
            }
            
            int effectiveRarity = static_cast<int>(item.rarity) - rarityBoost;
            if (effectiveRarity < static_cast<int>(ItemRarity::COMMON)) {
                effectiveRarity = static_cast<int>(ItemRarity::COMMON);
            }
            
            // Add item multiple times based on inverse rarity (common items appear more)
            int weight = 5 - effectiveRarity;
            for (int w = 0; w < weight; ++w) {
                validItems.push_back(&item);
            }
        }
        
        if (!validItems.empty()) {
            int idx = randomInt(0, static_cast<int>(validItems.size()) - 1);
            const Item* selectedTemplate = validItems[idx];
            Item item = *selectedTemplate;  // Copy
            item.quantity = 1;
            const int rolledItemLevel = rollItemLevel(floorLevel, item.rarity, false);
            applyItemLevelScaling(item, rolledItemLevel, true);
            room.items.push_back(item);
            
            // Track non-stackable templates to prevent duplicates
            if (!item.isStackable) {
                usedTemplates.insert(selectedTemplate);
            }
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

void MapGenerator::ensureMawExitRoom() {
    if (!mawModeActive) {
        return;
    }

    Room* exitRoom = nullptr;
    for (Room& room : rooms) {
        if (room.type == RoomType::EXIT) {
            exitRoom = &room;
            break;
        }
    }

    if (!exitRoom) {
        int entranceX = -1;
        int entranceY = -1;
        for (const Room& room : rooms) {
            if (room.id == entranceId) {
                entranceX = room.x;
                entranceY = room.y;
                break;
            }
        }

        int bestIndex = -1;
        int bestDistance = -1;

        for (size_t i = 0; i < rooms.size(); ++i) {
            Room& room = rooms[i];
            if (room.id == entranceId || room.type == RoomType::SHOP || room.type == RoomType::FORGE) {
                continue;
            }

            const int distance = std::abs(room.x - entranceX) + std::abs(room.y - entranceY);
            if (distance > bestDistance) {
                bestDistance = distance;
                bestIndex = static_cast<int>(i);
            }
        }

        if (bestIndex >= 0) {
            exitRoom = &rooms[bestIndex];
            exitId = exitRoom->id;
            exitRoom->type = RoomType::EXIT;
        }
    }

    if (exitRoom) {
        exitRoom->name = "Descent Rift";
        exitRoom->description = "A jagged rift vibrates with downward pull. Crossing it will collapse this layout and drag you into a deeper stratum.";
        exitRoom->shortDesc = "A Maw rift leading to the next stratum.";
        exitRoom->isCleared = true;
    }
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
