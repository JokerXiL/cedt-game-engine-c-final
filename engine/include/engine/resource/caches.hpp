#pragma once

#include <engine/resource/cache.hpp>
#include <engine/resource/loaders/shader_loader.hpp>
#include <engine/resource/loaders/texture_loader.hpp>
#include <engine/resource/loaders/model_loader.hpp>

namespace engine::resource {

// Type aliases for specific cache types
using ShaderCache = Cache<std::string, pbr::Shader, ShaderLoader>;
using TextureCache = Cache<std::string, pbr::Texture, TextureLoader>;
using ModelCache = Cache<std::string, pbr::Model, ModelLoader>;

}  // namespace engine::resource
