#pragma once

#include <engine/pbr/model.hpp>
#include <engine/pbr/shader_cache.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace engine::pbr {

/// Cache for model files loaded via Assimp
/// Handles meshes, embedded textures, and (future) animations
class ModelCache {
public:
    /// Constructor requires ShaderCache for creating materials
    explicit ModelCache(ShaderCache& shader_cache);

    /// Load model from file, returns cached version if already loaded
    std::shared_ptr<Model> load(const std::string& filepath);

    /// Remove all cached models
    void clear();

    /// Check if model is cached
    bool contains(const std::string& filepath) const;

    /// Number of cached models
    size_t size() const { return _cache.size(); }

private:
    ShaderCache& _shader_cache;
    std::unordered_map<std::string, std::shared_ptr<Model>> _cache;

    /// Internal loading using Assimp
    std::shared_ptr<Model> load_from_file(const std::string& filepath);
};

}  // namespace engine::pbr
