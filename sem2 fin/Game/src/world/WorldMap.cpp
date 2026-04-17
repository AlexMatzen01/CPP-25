#include "world/WorldMap.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <random>
#include <string>

namespace aq {

void WorldMap::InitializeDefaultLayout() {
    GenerateProceduralLayout(1337);
}

void WorldMap::GenerateProceduralLayoutAuto() {
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    const uint32_t seed = static_cast<uint32_t>(now & 0xffffffffU);
    GenerateProceduralLayout(seed);
}

void WorldMap::GenerateProceduralLayout(uint32_t seed) {
    if (seed == 0) {
        seed = 1337;
    }

    generationSeed_ = seed;
    rooms_.clear();
    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            grid_[y][x] = Cell{};
        }
    }

    struct Coord {
        int x = 0;
        int y = 0;
    };

    struct BiomeTemplate {
        const char* name = "";
        Color floorColor{};
        Color wallColor{};
        const char* floorAsset = "";
        const char* wallAsset = "";
    };

    std::array<BiomeTemplate, 5> biomes{{
        {"Verdant", {53, 101, 77, 255}, {62, 89, 70, 255}, "floor_forest", "wall_forest"},
        {"Storm", {64, 72, 88, 255}, {75, 84, 104, 255}, "floor_stone", "wall_stone"},
        {"Ember", {117, 76, 54, 255}, {128, 81, 57, 255}, "floor_ember", "wall_ember"},
        {"Obsidian", {45, 45, 50, 255}, {58, 58, 66, 255}, "floor_obsidian", "wall_obsidian"},
        {"Arcane", {78, 70, 122, 255}, {90, 82, 140, 255}, "floor_arcane", "wall_arcane"},
    }};

    const std::array<const char*, 8> commonNames{{
        "Echo Hall",
        "Glass Atrium",
        "Dawn Bridge",
        "Wisp Garden",
        "Silent Archive",
        "Ashen Span",
        "Crystal Gallery",
        "Twilight Bastion",
    }};

    const std::array<const char*, 8> commonDescriptions{{
        "Footsteps bounce softly along the chamber walls.",
        "Faint light scatters across fractured surfaces.",
        "A cold wind pulls through forgotten corridors.",
        "Ancient markings glow just beyond clear vision.",
        "You hear distant machinery beneath the stone.",
        "Dust spirals upward whenever you take a step.",
        "The room hums with latent magical pressure.",
        "The air tastes metallic and strangely warm.",
    }};

    std::vector<Coord> allCells;
    allCells.reserve(kWidth * kHeight);
    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            allCells.push_back({x, y});
        }
    }

    std::mt19937 rng(seed);
    std::shuffle(allCells.begin(), allCells.end(), rng);

    const Coord startCell = allCells[0];
    const Coord sigilCell = allCells[1];
    const Coord cryptCell = allCells[2];
    const Coord objectiveCell = allCells[3];

    auto roleForCoord = [&](int x, int y) {
        if (x == startCell.x && y == startCell.y) {
            return RoomRole::Start;
        }
        if (x == sigilCell.x && y == sigilCell.y) {
            return RoomRole::SigilVault;
        }
        if (x == cryptCell.x && y == cryptCell.y) {
            return RoomRole::CryptGate;
        }
        if (x == objectiveCell.x && y == objectiveCell.y) {
            return RoomRole::ObjectiveSanctum;
        }
        return RoomRole::Common;
    };

    auto addRoom = [this](int x,
                          int y,
                          std::string id,
                          std::string name,
                          std::string description,
                          Color floor,
                          Color wall,
                          RoomRole role,
                          const std::string& floorAsset,
                          const std::string& wallAsset) {
        rooms_.emplace_back(
            std::move(id), std::move(name), std::move(description), floor, wall, role, floorAsset, wallAsset);
        grid_[y][x].roomIndex = static_cast<int>(rooms_.size()) - 1;
    };

    int roomIndex = 0;
    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            const RoomRole role = roleForCoord(x, y);
            const BiomeTemplate& biome = biomes[static_cast<size_t>((roomIndex + seed) % biomes.size())];

            std::string id;
            std::string name;
            std::string description;

            switch (role) {
                case RoomRole::Start:
                    id = "start_gate";
                    name = "Pilgrim Cross";
                    description = "A calm crossroad where every expedition begins.";
                    break;
                case RoomRole::SigilVault:
                    id = "sigil_vault";
                    name = "Azure Repository";
                    description = "Sealed shelves guard the Ancient Sigil behind arcane wards.";
                    break;
                case RoomRole::CryptGate:
                    id = "crypt_gate";
                    name = "Sunken Crypt";
                    description = "A drowned passage where a guardian waits in silence.";
                    break;
                case RoomRole::ObjectiveSanctum:
                    id = "objective_sanctum";
                    name = "Storm Watch";
                    description = "An altar chamber where the recovered Sigil must be returned.";
                    break;
                case RoomRole::Common:
                default:
                    id = "zone_" + std::to_string(roomIndex);
                    name = std::string(biome.name) + " " + commonNames[static_cast<size_t>(roomIndex % commonNames.size())];
                    description = commonDescriptions[static_cast<size_t>((roomIndex + (seed % 7)) % commonDescriptions.size())];
                    break;
            }

            addRoom(x,
                    y,
                    std::move(id),
                    std::move(name),
                    std::move(description),
                    biome.floorColor,
                    biome.wallColor,
                    role,
                    biome.floorAsset,
                    biome.wallAsset);

            ++roomIndex;
        }
    }

    currentX_ = startCell.x;
    currentY_ = startCell.y;
    grid_[currentY_][currentX_].discovered = true;

    RefreshExits();
}

bool WorldMap::Move(Direction direction) {
    if (!CanMove(direction)) {
        return false;
    }

    int nextX = currentX_;
    int nextY = currentY_;
    switch (direction) {
        case Direction::North:
            --nextY;
            break;
        case Direction::East:
            ++nextX;
            break;
        case Direction::South:
            ++nextY;
            break;
        case Direction::West:
            --nextX;
            break;
    }

    currentX_ = nextX;
    currentY_ = nextY;
    grid_[currentY_][currentX_].discovered = true;
    return true;
}

bool WorldMap::CanMove(Direction direction) const {
    int nextX = currentX_;
    int nextY = currentY_;
    switch (direction) {
        case Direction::North:
            --nextY;
            break;
        case Direction::East:
            ++nextX;
            break;
        case Direction::South:
            ++nextY;
            break;
        case Direction::West:
            --nextX;
            break;
    }

    return IsInBounds(nextX, nextY) && grid_[nextY][nextX].roomIndex >= 0 && CurrentRoom().HasExit(direction);
}

bool WorldMap::SetCurrentCell(int x, int y) {
    if (!HasRoomAt(x, y)) {
        return false;
    }

    currentX_ = x;
    currentY_ = y;
    grid_[currentY_][currentX_].discovered = true;
    return true;
}

uint32_t WorldMap::Seed() const {
    return generationSeed_;
}

const Room* WorldMap::FindRoomByRole(RoomRole role) const {
    for (const Room& room : rooms_) {
        if (room.Role() == role) {
            return &room;
        }
    }

    return nullptr;
}

const Room& WorldMap::CurrentRoom() const {
    return rooms_.at(grid_[currentY_][currentX_].roomIndex);
}

Room& WorldMap::CurrentRoom() {
    return rooms_.at(grid_[currentY_][currentX_].roomIndex);
}

bool WorldMap::HasRoomAt(int x, int y) const {
    return IsInBounds(x, y) && grid_[y][x].roomIndex >= 0;
}

bool WorldMap::IsDiscovered(int x, int y) const {
    return HasRoomAt(x, y) && grid_[y][x].discovered;
}

const WorldMap::Cell& WorldMap::GetCell(int x, int y) const {
    return grid_[y][x];
}

bool WorldMap::IsInBounds(int x, int y) const {
    return x >= 0 && x < kWidth && y >= 0 && y < kHeight;
}

void WorldMap::RefreshExits() {
    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            if (!HasRoomAt(x, y)) {
                continue;
            }

            Room& room = rooms_.at(grid_[y][x].roomIndex);
            room.SetExit(Direction::North, HasRoomAt(x, y - 1));
            room.SetExit(Direction::East, HasRoomAt(x + 1, y));
            room.SetExit(Direction::South, HasRoomAt(x, y + 1));
            room.SetExit(Direction::West, HasRoomAt(x - 1, y));
        }
    }
}

}  // namespace aq
