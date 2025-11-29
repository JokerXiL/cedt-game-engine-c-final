#include <engine/pbr/shader_cache.hpp>

namespace engine::pbr {

std::shared_ptr<Shader> ShaderCache::load(const std::string& vertex_path, const std::string& fragment_path) {
    std::string key = make_key(vertex_path, fragment_path);

    auto it = _cache.find(key);
    if (it != _cache.end()) {
        return it->second;
    }

    auto shader = std::make_shared<Shader>(vertex_path, fragment_path);
    if (shader->valid()) {
        _cache[key] = shader;
    }
    return shader;
}

void ShaderCache::clear() {
    _cache.clear();
}

bool ShaderCache::contains(const std::string& vertex_path, const std::string& fragment_path) const {
    return _cache.find(make_key(vertex_path, fragment_path)) != _cache.end();
}

std::string ShaderCache::make_key(const std::string& vertex_path, const std::string& fragment_path) {
    return vertex_path + "|" + fragment_path;
}

}  // namespace engine::pbr
