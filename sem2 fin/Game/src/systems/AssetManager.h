#pragma once

#include <string>
#include <unordered_map>

#include "raylib.h"

namespace aq {

struct AssetManifestEntry {
    std::string type;
    std::string id;
    std::string localPath;
    std::string urls;
    std::string sha256;
    std::string attribution;
};

class AssetManager {
  public:
    ~AssetManager();

    bool LoadManifest(const std::string& manifestPath, std::string& error);
    void LoadAvailableTextures(std::string& infoLog);

    const Texture2D* GetTexture(const std::string& assetId) const;
    bool HasTexture(const std::string& assetId) const;

    const std::string& ManifestVersion() const {
        return manifestVersion_;
    }

    void UnloadAll();

  private:
    struct TextureSlot {
        Texture2D texture{};
        bool loaded = false;
    };

    std::string manifestVersion_ = "unversioned";
    std::unordered_map<std::string, AssetManifestEntry> entries_;
    std::unordered_map<std::string, TextureSlot> textures_;
};

}  // namespace aq
