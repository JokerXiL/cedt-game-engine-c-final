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

// Forward declaration
class ModelCache;

/// Data loaded from a model file (glTF, FBX, OBJ, etc.)
class Model {
public:
    Model() = default;

    /// Render all meshes with their materials
    void render(const glm::mat4& transform, const Camera& camera,
                const glm::vec3& light_pos,
                const glm::vec3& light_color = glm::vec3(1.0f));

    /// Render all meshes with skeletal animation
    void render_skinned(const glm::mat4& transform, const Skeleton& skeleton,
                        const Camera& camera, const glm::vec3& light_pos,
                        const glm::vec3& light_color = glm::vec3(1.0f));

    /// Get number of meshes
    size_t mesh_count() const { return _meshes.size(); }

private:
    friend class ModelCache;  // Allow ModelCache to populate the model

    std::vector<std::shared_ptr<Mesh>> _meshes;
    std::vector<std::shared_ptr<StandardMaterial>> _materials;  // Parallel to meshes vector
    std::vector<std::shared_ptr<Texture>> _textures;  // Owned textures
    // Future: animations, skeleton, etc.
};

}  // namespace engine
