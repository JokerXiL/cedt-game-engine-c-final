#pragma once

#include <engine/render/mesh.hpp>
#include <engine/render/texture.hpp>
#include <engine/render/standard_material.hpp>
#include <engine/render/skeleton.hpp>
#include <engine/render/camera.hpp>

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace engine {

// Forward declarations
class ModelCache;
class Scene;

/// Data loaded from a model file (glTF, FBX, OBJ, etc.)
class Model {
public:
    Model() = default;

    // ========================================================================
    // New Scene-based render methods (preferred)
    // ========================================================================

    /// Render all meshes using Scene for camera and lighting
    void render(const glm::mat4& transform, const Scene& scene);

    /// Render all meshes with skeletal animation using Scene
    void render_skinned(const glm::mat4& transform, const Skeleton& skeleton,
                        const Scene& scene);

    /// Render to shadow map (depth only)
    void render_shadow(const glm::mat4& transform, const glm::mat4& light_space_matrix);

    /// Render skinned mesh to shadow map
    void render_shadow_skinned(const glm::mat4& transform, const Skeleton& skeleton,
                               const glm::mat4& light_space_matrix);

    // ========================================================================
    // Legacy render methods (for backwards compatibility)
    // ========================================================================

    /// Legacy render - uses single light position
    void render(const glm::mat4& transform, const Camera& camera,
                const glm::vec3& light_pos,
                const glm::vec3& light_color = glm::vec3(1.0f));

    /// Legacy skinned render - uses single light position
    void render_skinned(const glm::mat4& transform, const Skeleton& skeleton,
                        const Camera& camera, const glm::vec3& light_pos,
                        const glm::vec3& light_color = glm::vec3(1.0f));

    // ========================================================================
    // Accessors
    // ========================================================================

    /// Get number of meshes
    size_t mesh_count() const { return _meshes.size(); }

private:
    friend class ModelCache;  // Allow ModelCache to populate the model

    std::vector<std::shared_ptr<Mesh>> _meshes;
    std::vector<std::shared_ptr<StandardMaterial>> _materials;  // Parallel to meshes vector
    std::vector<std::shared_ptr<Texture>> _textures;  // Owned textures
};

}  // namespace engine
