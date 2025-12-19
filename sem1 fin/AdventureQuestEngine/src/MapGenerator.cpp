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
    : mapWidth(width), mapHeight(height), entranceId(-1), exitId(-1), nextRoomId(0), forgeCount(0) {
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

const std::vector<std::vector<int>>& MapGenerator::getGrid() const {
    return grid;
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
                const Room* room = nullptr;
                
                for (const Room& r : rooms) {
                    if (r.id == roomId) {
                        room = &r;
                        break;
                    }
                }
                
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
    std::cout << colorize("☆", Color::BRIGHT_YELLOW) << " Exit (Escape!)        ";
    std::cout << colorize("⚔", Color::BRIGHT_RED) << " Boss Room\n";
    
    std::cout << "  " << colorize("$", Color::YELLOW) << " Treasure Chest        ";
    std::cout << colorize("⚠", Color::RED) << " Dangerous Trap        ";
    std::cout << colorize("Ω", Color::MAGENTA) << " Puzzle Challenge\n";
    
    std::cout << "  " << colorize("◊", Color::CYAN) << " Secret Room           ";
    std::cout << colorize("♥", Color::GREEN) << " Safe Rest Area        ";
    std::cout << colorize("◘", Color::BRIGHT_MAGENTA) << " Merchant Shop\n";
    
    std::cout << "  " << colorize("▤", Color::BRIGHT_BLUE) << " Library (XP)          ";
    std::cout << colorize("⚒", Color::BRIGHT_RED) << " Forge (Crafting)      ";
    std::cout << colorize("⛩", Color::BRIGHT_YELLOW) << " Shrine (Blessing)\n";
    
    std::cout << "  " << colorize("⚗", Color::BRIGHT_GREEN) << " Laboratory (Potions)  ";
    std::cout << colorize("⚰", Color::DARK_GRAY) << " Graveyard (Undead)    ";
    std::cout << colorize("⛨", Color::BRIGHT_WHITE) << " Armory (Gear)\n";
    
    std::cout << "  " << colorize("⛓", Color::RED) << " Prison (Rescue)       ";
    std::cout << colorize("•", Color::BRIGHT_WHITE) << " Room (Unexplored)     ";
    std::cout << colorize("○", Color::WHITE) << " Room (Explored)\n";
    
    std::cout << "  " << colorize("·", Color::DARK_GRAY) << " Room (Cleared)\n\n";
    
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
    // Random fill with decreasing probability based on distance from path
    int fillChance = 40;  // Base chance to fill empty cell
    
    // Collect forge candidates for strategic placement
    std::vector<std::pair<int, int>> forgeCandidates;
    
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if (!hasRoom(x, y)) {
                // Check if adjacent to existing room
                std::vector<int> adjacent = getAdjacentRooms(x, y);
                
                if (!adjacent.empty()) {
                    forgeCandidates.push_back({x, y});
                    
                    if (randomChance(fillChance)) {
                        RoomType type = determineRoomType(x, y);
                        
                        // Force forge generation if we don't have 3 yet and have candidates
                        if (type == RoomType::FORGE && forgeCount < 3) {
                            type = RoomType::FORGE;
                        } else if (forgeCount < 3 && randomChance(25)) {
                            // Extra 25% chance to override with forge if we need more
                            type = RoomType::FORGE;
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
        }
    }
    
    // If we still don't have 3 forges, place them explicitly in remaining candidates
    while (forgeCount < 3 && !forgeCandidates.empty()) {
        int idx = randomInt(0, static_cast<int>(forgeCandidates.size()) - 1);
        auto [x, y] = forgeCandidates[idx];
        
        if (!hasRoom(x, y)) {
            Room forge = createRoom(x, y, RoomType::FORGE);
            forge.name = "Hidden Forge";
            forge.description = "A mastercraft forge stands here, created by legendary smiths. "
                               "You can upgrade and enchant your equipment at this location.";
            rooms.push_back(forge);
            grid[y][x] = forge.id;
            forgeCount++;
        }
        
        forgeCandidates.erase(forgeCandidates.begin() + idx);
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
        default:
            extra = "";
    }
    
    return base + extra;
}

RoomType MapGenerator::determineRoomType(int x, int y, bool isMainPath) {
    // Random determination based on probabilities
    int roll = randomInt(1, 100);
    int cumulative = 0;
    
    // Boss rooms should not be on the main path to keep them "on the side"
    if (!isMainPath) {
        cumulative += BOSS_ROOM_CHANCE;
        if (roll <= cumulative) return RoomType::BOSS;
    }
    
    cumulative += TREASURE_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::TREASURE;
    
    cumulative += SHOP_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::SHOP;
    
    cumulative += TRAP_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::TRAP;
    
    cumulative += PUZZLE_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::PUZZLE;
    
    cumulative += SAFE_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::SAFE;
    
    cumulative += LIBRARY_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::LIBRARY;
    
    cumulative += FORGE_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::FORGE;
    
    cumulative += SHRINE_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::SHRINE;
    
    cumulative += LABORATORY_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::LABORATORY;
    
    cumulative += GRAVEYARD_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::GRAVEYARD;
    
    cumulative += ARMORY_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::ARMORY;
    
    cumulative += PRISON_ROOM_CHANCE;
    if (roll <= cumulative) return RoomType::PRISON;
    
    return RoomType::NORMAL;
}

// ---
// PRIVATE METHODS - Population
// ---

void MapGenerator::populateWithEnemies(Room& room, const std::vector<Enemy>& templates) {
    // Skip certain room types
    if (room.type == RoomType::SAFE || room.type == RoomType::ENTRANCE || 
        room.type == RoomType::EXIT || room.type == RoomType::TREASURE ||
        room.type == RoomType::SHOP || room.type == RoomType::LIBRARY ||
        room.type == RoomType::FORGE || room.type == RoomType::SHRINE) {
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
        case RoomType::ARMORY:
            itemCount = randomInt(1, 3);
            break;
        case RoomType::LIBRARY:
            if (randomChance(70)) itemCount = 1;
            break;
        case RoomType::LABORATORY:
            if (randomChance(60)) itemCount = randomInt(1, 2);
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
