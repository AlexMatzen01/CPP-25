#include "world/Room.h"

namespace aq {

const char* DirectionToString(Direction direction) {
    switch (direction) {
        case Direction::North:
            return "north";
        case Direction::East:
            return "east";
        case Direction::South:
            return "south";
        case Direction::West:
            return "west";
    }

    return "unknown";
}

const char* RoomRoleToString(RoomRole role) {
    switch (role) {
        case RoomRole::Common:
            return "common";
        case RoomRole::Start:
            return "start";
        case RoomRole::SigilVault:
            return "sigil_vault";
        case RoomRole::CryptGate:
            return "crypt_gate";
        case RoomRole::ObjectiveSanctum:
            return "objective_sanctum";
    }

    return "unknown";
}

Direction OppositeDirection(Direction direction) {
    switch (direction) {
        case Direction::North:
            return Direction::South;
        case Direction::East:
            return Direction::West;
        case Direction::South:
            return Direction::North;
        case Direction::West:
            return Direction::East;
    }

    return Direction::North;
}

Room::Room(std::string id,
                     std::string name,
                     std::string description,
                     Color floorColor,
                     Color wallColor,
                     RoomRole role,
                     std::string floorAssetId,
                     std::string wallAssetId)
    : id_(std::move(id)),
      name_(std::move(name)),
      description_(std::move(description)),
      floorColor_(floorColor),
            wallColor_(wallColor),
            role_(role),
            floorAssetId_(std::move(floorAssetId)),
            wallAssetId_(std::move(wallAssetId)) {}

const std::string& Room::Id() const {
    return id_;
}

const std::string& Room::Name() const {
    return name_;
}

const std::string& Room::Description() const {
    return description_;
}

Color Room::FloorColor() const {
    return floorColor_;
}

Color Room::WallColor() const {
    return wallColor_;
}

RoomRole Room::Role() const {
    return role_;
}

const std::string& Room::FloorAssetId() const {
    return floorAssetId_;
}

const std::string& Room::WallAssetId() const {
    return wallAssetId_;
}

void Room::SetExit(Direction direction, bool available) {
    exits_[static_cast<int>(direction)] = available;
}

bool Room::HasExit(Direction direction) const {
    return exits_[static_cast<int>(direction)];
}

}  // namespace aq
