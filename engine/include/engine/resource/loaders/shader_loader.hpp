#pragma once

#include <engine/pbr/shader.hpp>

#include <memory>
#include <string>

namespace engine::resource {

/// Loader for Shader resources
/// Handles composite key generation from vertex/fragment paths
class ShaderLoader {
public:
    using ResourcePtr = std::shared_ptr<pbr::Shader>;

    /// Create shader from vertex and fragment paths
    ResourcePtr operator()(const std::string& vertex_path,
                           const std::string& fragment_path) const {
        auto shader = std::make_shared<pbr::Shader>(vertex_path, fragment_path);
        return shader->valid() ? shader : nullptr;
    }

    /// Generate composite key from vertex/fragment paths
    static std::string make_key(const std::string& vertex_path,
                                const std::string& fragment_path) {
        return vertex_path + "|" + fragment_path;
    }
};

}  // namespace engine::resource
