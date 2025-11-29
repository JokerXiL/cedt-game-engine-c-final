#pragma once

#include <engine/render/model.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace engine {

/// Cache for model files loaded via Assimp
/// Handles meshes, embedded textures, and (future) animations
class ModelCache {
public:
    /// Load model from file, returns cached version if already loaded
    std::shared_ptr<Model> load(const std::string& filepath);

    /// Remove all cached models
    void clear();

    /// Check if model is cached
    bool contains(const std::string& filepath) const;

    /// Number of cached models
    size_t size() const { return _cache.size(); }

private:
    std::unordered_map<std::string, std::shared_ptr<Model>> _cache;

    /// Internal loading using Assimp
    std::shared_ptr<Model> load_from_file(const std::string& filepath);
};

}  // namespace engine
