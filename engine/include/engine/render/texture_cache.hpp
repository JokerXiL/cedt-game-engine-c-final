#pragma once

#include <engine/render/texture.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace engine {

/// Cache for standalone textures (UI, skybox, etc.)
/// For textures embedded in model files, use ModelCache instead
class TextureCache {
public:
    /// Load texture from file, returns cached version if already loaded
    std::shared_ptr<Texture> load(const std::string& filepath);

    /// Remove all cached textures
    void clear();

    /// Check if texture is cached
    bool contains(const std::string& filepath) const;

    /// Number of cached textures
    size_t size() const { return _cache.size(); }

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> _cache;
};

}  // namespace engine
