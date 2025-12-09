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
/// Only render() is pure virtual; other methods have empty defaults for materials
/// that don't support skinning or shadow casting.
class Material {
public:
    virtual ~Material() = default;

    /// Render a static mesh using Scene for camera and lighting
    virtual void render(const Mesh& mesh, const glm::mat4& transform, const Scene& scene) = 0;

    /// Render a skinned mesh using Scene for camera and lighting
    /// Default: falls back to render() ignoring skeleton
    virtual void render_skinned(const Mesh& mesh, const glm::mat4& transform,
                                const Skeleton& skeleton, const Scene& scene) {
        (void)skeleton;
        render(mesh, transform, scene);
    }

    /// Render to shadow map (depth only, no lighting)
    /// Default: no-op (material doesn't cast shadows)
    virtual void render_shadow(const Mesh& mesh, const glm::mat4& transform,
                               const glm::mat4& light_space_matrix) {
        (void)mesh; (void)transform; (void)light_space_matrix;
    }

    /// Render skinned mesh to shadow map
    /// Default: no-op (material doesn't cast shadows)
    virtual void render_shadow_skinned(const Mesh& mesh, const glm::mat4& transform,
                                       const Skeleton& skeleton,
                                       const glm::mat4& light_space_matrix) {
        (void)mesh; (void)transform; (void)skeleton; (void)light_space_matrix;
    }

    /// Render to point light shadow cubemap
    /// Default: no-op (material doesn't cast shadows)
    virtual void render_shadow_cube(const Mesh& mesh, const glm::mat4& transform,
                                    const glm::mat4& light_space_matrix,
                                    const glm::vec3& light_pos, float far_plane) {
        (void)mesh; (void)transform; (void)light_space_matrix;
        (void)light_pos; (void)far_plane;
    }

    /// Render skinned mesh to point light shadow cubemap
    /// Default: no-op (material doesn't cast shadows)
    virtual void render_shadow_cube_skinned(const Mesh& mesh, const glm::mat4& transform,
                                            const Skeleton& skeleton,
                                            const glm::mat4& light_space_matrix,
                                            const glm::vec3& light_pos, float far_plane) {
        (void)mesh; (void)transform; (void)skeleton;
        (void)light_space_matrix; (void)light_pos; (void)far_plane;
    }
};

}  // namespace engine::pbr
