#pragma once

#include "world/Room.h"

#include <cstdint>
#include <vector>

namespace aq {

class WorldMap {
  public:
    static constexpr int kWidth = 4;
    static constexpr int kHeight = 3;

    struct Cell {
        int roomIndex = -1;
        bool discovered = false;
    };

    void InitializeDefaultLayout();
    void GenerateProceduralLayout(uint32_t seed);
    void GenerateProceduralLayoutAuto();

    bool Move(Direction direction);
    bool CanMove(Direction direction) const;
    bool SetCurrentCell(int x, int y);

    const Room& CurrentRoom() const;
    Room& CurrentRoom();

    int CurrentX() const {
        return currentX_;
    }

    int CurrentY() const {
        return currentY_;
    }

    bool HasRoomAt(int x, int y) const;
    bool IsDiscovered(int x, int y) const;
    const Cell& GetCell(int x, int y) const;
    uint32_t Seed() const;
    const Room* FindRoomByRole(RoomRole role) const;

  private:
    bool IsInBounds(int x, int y) const;
    void RefreshExits();

    std::vector<Room> rooms_;
    Cell grid_[kHeight][kWidth]{};
    int currentX_ = 1;
    int currentY_ = 1;
    uint32_t generationSeed_ = 1337;
};

}  // namespace aq
