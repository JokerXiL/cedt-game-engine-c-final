#pragma once

#include <engine/pbr/shader.hpp>
#include <engine/pbr/skeleton.hpp>

#include <memory>

namespace engine::pbr {

class Mesh;
class Scene;
class ShaderCache;

/// Base class for materials
/// Materials own shaders via shared_ptr for automatic resource management
class Material {
public:
    virtual ~Material() = default;

    /// Render a static mesh using Scene for camera and lighting
    virtual void render(const Mesh& mesh, const glm::mat4& transform, const Scene& scene) = 0;

    /// Render a skinned mesh using Scene for camera and lighting
    virtual void render_skinned(const Mesh& mesh, const glm::mat4& transform,
                        const Skeleton& skeleton, const Scene& scene) = 0;

    /// Render to shadow map (depth only, no lighting)
    virtual void render_shadow(const Mesh& mesh, const glm::mat4& transform,
                       const glm::mat4& light_space_matrix) = 0;
    /// Render skinned mesh to shadow map
    virtual void render_shadow_skinned(const Mesh& mesh, const glm::mat4& transform,
                               const Skeleton& skeleton,
                               const glm::mat4& light_space_matrix) = 0;

    /// Render to point light shadow cubemap (single face, multi-pass approach)
    virtual void render_shadow_cube(const Mesh& mesh, const glm::mat4& transform,
                            const glm::mat4& light_space_matrix,
                            const glm::vec3& light_pos, float far_plane) = 0;

    /// Render skinned mesh to point light shadow cubemap
    virtual void render_shadow_cube_skinned(const Mesh& mesh, const glm::mat4& transform,
                                    const Skeleton& skeleton,
                                    const glm::mat4& light_space_matrix,
                                    const glm::vec3& light_pos, float far_plane) = 0;
};

}  // namespace engine::pbr
