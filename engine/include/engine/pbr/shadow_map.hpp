#pragma once

#include <engine/pbr/light.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>

namespace engine::pbr {

/// Shadow map resolution (4096 for high quality)
constexpr int SHADOW_MAP_SIZE = 8192;

/// Single shadow map for directional/spot lights
class ShadowMap {
public:
    ShadowMap();
    ~ShadowMap();

    ShadowMap(ShadowMap&& other) noexcept;
    ShadowMap& operator=(ShadowMap&& other) noexcept;

    ShadowMap(const ShadowMap&) = delete;
    ShadowMap& operator=(const ShadowMap&) = delete;

    /// Initialize OpenGL resources (call after GL context is ready)
    void initialize();

    /// Bind framebuffer for shadow rendering
    void bind_for_writing();

    /// Unbind framebuffer (restore default)
    void unbind();

    /// Get depth texture for sampling
    GLuint depth_texture() const { return _depth_texture; }

    /// Light space matrix for this shadow map
    glm::mat4 light_space_matrix{1.0f};

    bool is_initialized() const { return _initialized; }

private:
    GLuint _fbo = 0;
    GLuint _depth_texture = 0;
    bool _initialized = false;

    void cleanup();
};

/// Point light shadow map (cubemap for omnidirectional shadows)
class ShadowCubeMap {
public:
    ShadowCubeMap();
    ~ShadowCubeMap();

    ShadowCubeMap(ShadowCubeMap&& other) noexcept;
    ShadowCubeMap& operator=(ShadowCubeMap&& other) noexcept;

    ShadowCubeMap(const ShadowCubeMap&) = delete;
    ShadowCubeMap& operator=(const ShadowCubeMap&) = delete;

    /// Initialize OpenGL resources
    void initialize();

    /// Bind framebuffer for shadow rendering to a specific face
    /// @param face Cube face index (0-5: +X, -X, +Y, -Y, +Z, -Z)
    void bind_for_writing(int face);

    /// Unbind framebuffer
    void unbind();

    /// Get depth cubemap for sampling
    GLuint depth_cubemap() const { return _depth_cubemap; }

    /// 6 view-projection matrices (one per cube face)
    std::array<glm::mat4, 6> light_space_matrices;

    /// Far plane distance for depth linearization
    float far_plane = 100.0f;

    bool is_initialized() const { return _initialized; }

private:
    GLuint _fbo = 0;
    GLuint _depth_cubemap = 0;
    bool _initialized = false;

    void cleanup();
};

}  // namespace engine::pbr
