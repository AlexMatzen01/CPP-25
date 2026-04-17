#include "systems/AssetManager.h"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

namespace aq {

namespace {

std::vector<std::string> Split(const std::string& text, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(text);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        parts.push_back(item);
    }
    return parts;
}

std::string Trim(const std::string& text) {
    size_t start = 0;
    while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start]))) {
        ++start;
    }

    size_t end = text.size();
    while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
        --end;
    }

    return text.substr(start, end - start);
}

}  // namespace

AssetManager::~AssetManager() {
    UnloadAll();
}

bool AssetManager::LoadManifest(const std::string& manifestPath, std::string& error) {
    UnloadAll();
    entries_.clear();
    manifestVersion_ = "unversioned";

    std::ifstream in(manifestPath);
    if (!in.is_open()) {
        error = "Could not open asset manifest: " + manifestPath;
        return false;
    }

    const std::filesystem::path baseDir = std::filesystem::path(manifestPath).parent_path();

    std::string line;
    int lineNumber = 0;
    while (std::getline(in, line)) {
        ++lineNumber;
        const std::string trimmed = Trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        const std::vector<std::string> parts = Split(trimmed, '|');
        if (parts.empty()) {
            continue;
        }

        if (parts[0] == "version") {
            if (parts.size() >= 2) {
                manifestVersion_ = Trim(parts[1]);
            }
            continue;
        }

        if (parts.size() < 6) {
            error = "Invalid manifest entry at line " + std::to_string(lineNumber);
            return false;
        }

        AssetManifestEntry entry;
        entry.type = Trim(parts[0]);
        entry.id = Trim(parts[1]);

        std::filesystem::path localPath = Trim(parts[2]);
        if (!localPath.is_absolute()) {
            localPath = baseDir / localPath;
        }
        entry.localPath = localPath.generic_string();

        entry.urls = Trim(parts[3]);
        entry.sha256 = Trim(parts[4]);
        entry.attribution = Trim(parts[5]);

        if (entry.id.empty()) {
            error = "Manifest entry has empty id at line " + std::to_string(lineNumber);
            return false;
        }

        entries_[entry.id] = entry;
    }

    return true;
}

void AssetManager::LoadAvailableTextures(std::string& infoLog) {
    textures_.clear();

    int loadedCount = 0;
    int missingCount = 0;
    for (const auto& pair : entries_) {
        const AssetManifestEntry& entry = pair.second;
        if (entry.type != "texture") {
            continue;
        }

        if (!FileExists(entry.localPath.c_str())) {
            ++missingCount;
            continue;
        }

        TextureSlot slot;
        slot.texture = LoadTexture(entry.localPath.c_str());
        slot.loaded = slot.texture.id != 0;
        if (slot.loaded) {
            textures_[entry.id] = slot;
            ++loadedCount;
        } else {
            ++missingCount;
        }
    }

    infoLog = "Asset textures loaded: " + std::to_string(loadedCount) + ", missing: " + std::to_string(missingCount);
}

const Texture2D* AssetManager::GetTexture(const std::string& assetId) const {
    const auto it = textures_.find(assetId);
    if (it == textures_.end() || !it->second.loaded) {
        return nullptr;
    }

    return &it->second.texture;
}

bool AssetManager::HasTexture(const std::string& assetId) const {
    return GetTexture(assetId) != nullptr;
}

void AssetManager::UnloadAll() {
    for (auto& pair : textures_) {
        TextureSlot& slot = pair.second;
        if (slot.loaded && slot.texture.id != 0) {
            UnloadTexture(slot.texture);
            slot.loaded = false;
        }
    }
    textures_.clear();
}

}  // namespace aq
