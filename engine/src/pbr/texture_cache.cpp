#include <engine/pbr/texture_cache.hpp>

namespace engine::pbr {

std::shared_ptr<Texture> TextureCache::load(const std::string& filepath) {
    auto it = _cache.find(filepath);
    if (it != _cache.end()) {
        return it->second;
    }

    auto texture = std::make_shared<Texture>(filepath);
    _cache[filepath] = texture;
    return texture;
}

void TextureCache::clear() {
    _cache.clear();
}

bool TextureCache::contains(const std::string& filepath) const {
    return _cache.find(filepath) != _cache.end();
}

}  // namespace engine::pbr
