#pragma once

#include <array>
#include <string>

#include "raylib.h"

namespace aq {

enum class Direction { North = 0, East = 1, South = 2, West = 3 };
enum class RoomRole { Common = 0, Start = 1, SigilVault = 2, CryptGate = 3, ObjectiveSanctum = 4 };

const char* DirectionToString(Direction direction);
Direction OppositeDirection(Direction direction);
const char* RoomRoleToString(RoomRole role);

class Room {
  public:
    Room() = default;
    Room(std::string id,
         std::string name,
         std::string description,
         Color floorColor,
         Color wallColor,
         RoomRole role,
         std::string floorAssetId,
         std::string wallAssetId);

    const std::string& Id() const;
    const std::string& Name() const;
    const std::string& Description() const;
    Color FloorColor() const;
    Color WallColor() const;
    RoomRole Role() const;
    const std::string& FloorAssetId() const;
    const std::string& WallAssetId() const;

    void SetExit(Direction direction, bool available);
    bool HasExit(Direction direction) const;

  private:
    std::string id_;
    std::string name_;
    std::string description_;
    Color floorColor_{60, 60, 60, 255};
    Color wallColor_{90, 90, 90, 255};
    RoomRole role_{RoomRole::Common};
    std::string floorAssetId_;
    std::string wallAssetId_;
    std::array<bool, 4> exits_{{false, false, false, false}};
};

}  // namespace aq
