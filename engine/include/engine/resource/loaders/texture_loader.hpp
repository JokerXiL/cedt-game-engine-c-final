#pragma once

#include <engine/pbr/texture.hpp>

#include <memory>
#include <string>

namespace engine::resource {

/// Loader for Texture resources (standalone textures like UI, skybox)
/// For textures embedded in model files, use ModelLoader instead
class TextureLoader {
public:
    using ResourcePtr = std::shared_ptr<pbr::Texture>;

    /// Create texture from filepath
    ResourcePtr operator()(const std::string& filepath) const {
        return std::make_shared<pbr::Texture>(filepath);
    }

    /// Identity key for single-argument loaders
    static std::string make_key(const std::string& filepath) {
        return filepath;
    }
};

}  // namespace engine::resource
