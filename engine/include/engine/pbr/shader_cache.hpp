#pragma once

#include <engine/pbr/shader.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace engine::pbr {

/// Cache for shader programs
/// Shaders are keyed by "vertex_path|fragment_path" to allow reuse
class ShaderCache {
public:
    /// Load shader from vertex/fragment paths, returns cached version if already loaded
    std::shared_ptr<Shader> load(const std::string& vertex_path, const std::string& fragment_path);

    /// Remove all cached shaders
    void clear();

    /// Check if shader is cached
    bool contains(const std::string& vertex_path, const std::string& fragment_path) const;

    /// Number of cached shaders
    size_t size() const { return _cache.size(); }

private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> _cache;

    static std::string make_key(const std::string& vertex_path, const std::string& fragment_path);
};

}  // namespace engine::pbr
