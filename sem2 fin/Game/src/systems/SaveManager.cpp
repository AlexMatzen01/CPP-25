#include "systems/SaveManager.h"

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace aq {

bool SaveManager::SaveToFile(const std::string& path, const SaveState& state, std::string& error) {
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open()) {
        error = "Could not open save file for writing.";
        return false;
    }

    out << "version=2\n";
    out << "room_x=" << state.roomX << "\n";
    out << "room_y=" << state.roomY << "\n";
    out << "health=" << state.health << "\n";
    out << "has_sigil=" << (state.hasAncientSigil ? 1 : 0) << "\n";
    out << "crypt_cleared=" << (state.cryptEncounterCleared ? 1 : 0) << "\n";
    out << "world_seed=" << state.worldSeed << "\n";
    out << "manifest_ver=" << state.manifestVersion << "\n";

    if (!out.good()) {
        error = "Failed while writing save file.";
        return false;
    }

    return true;
}

bool SaveManager::LoadFromFile(const std::string& path, SaveState& outState, std::string& error) {
    std::ifstream in(path);
    if (!in.is_open()) {
        error = "Could not open save file.";
        return false;
    }

    std::unordered_map<std::string, std::string> entries;
    std::string line;
    while (std::getline(in, line)) {
        const size_t split = line.find('=');
        if (split == std::string::npos || split == 0 || split + 1 >= line.size()) {
            continue;
        }

        entries[line.substr(0, split)] = line.substr(split + 1);
    }

    if (!entries.count("version")) {
        error = "Unsupported or missing save version.";
        return false;
    }

    try {
        const std::string version = entries.at("version");

        outState.roomX = std::stoi(entries.at("room_x"));
        outState.roomY = std::stoi(entries.at("room_y"));
        outState.health = std::stoi(entries.at("health"));
        outState.hasAncientSigil = std::stoi(entries.at("has_sigil")) != 0;
        outState.cryptEncounterCleared = std::stoi(entries.at("crypt_cleared")) != 0;

        if (version == "1") {
            outState.worldSeed = 1337;
            outState.manifestVersion = "legacy-v1";
            return true;
        }

        if (version == "2") {
            if (entries.count("world_seed")) {
                outState.worldSeed = static_cast<uint32_t>(std::stoul(entries.at("world_seed")));
            } else {
                outState.worldSeed = 1337;
            }

            if (entries.count("manifest_ver") && !entries.at("manifest_ver").empty()) {
                outState.manifestVersion = entries.at("manifest_ver");
            } else {
                outState.manifestVersion = "unversioned";
            }
            return true;
        }

        error = "Unsupported save version: " + version;
        return false;
    } catch (const std::exception&) {
        error = "Save file is malformed.";
        return false;
    }
}

}  // namespace aq
