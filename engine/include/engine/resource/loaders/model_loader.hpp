#pragma once

#include <engine/pbr/model.hpp>
#include <engine/pbr/standard_material.hpp>

#include <functional>
#include <memory>
#include <string>

namespace engine::resource {

/// Loader for Model resources
/// Takes a ShaderLoadFunc to create materials during model loading
class ModelLoader {
public:
    using ResourcePtr = std::shared_ptr<pbr::Model>;
    using ShaderLoadFunc = pbr::StandardMaterial::ShaderLoadFunc;

    /// Construct with a shader loading function for creating materials
    explicit ModelLoader(ShaderLoadFunc shader_loader)
        : _shader_loader(std::move(shader_loader)) {}

    /// Load model from filepath
    ResourcePtr operator()(const std::string& filepath) const;

    /// Identity key for single-argument loaders
    static std::string make_key(const std::string& filepath) {
        return filepath;
    }

private:
    ShaderLoadFunc _shader_loader;
};

}  // namespace engine::resource
