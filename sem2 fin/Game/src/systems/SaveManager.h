#pragma once

#include <cstdint>
#include <string>

namespace aq {

struct SaveState {
    int roomX = 1;
    int roomY = 1;
    int health = 100;
    bool hasAncientSigil = false;
    bool cryptEncounterCleared = false;
  uint32_t worldSeed = 1337;
  std::string manifestVersion = "unversioned";
};

class SaveManager {
  public:
    static bool SaveToFile(const std::string& path, const SaveState& state, std::string& error);
    static bool LoadFromFile(const std::string& path, SaveState& outState, std::string& error);
};

}  // namespace aq
