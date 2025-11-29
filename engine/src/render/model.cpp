#include <engine/render/model.hpp>
#include <engine/render/scene.hpp>

namespace engine {

// ============================================================================
// Scene-based rendering (new API)
// ============================================================================

void Model::render(const glm::mat4& transform, const Scene& scene) {
    for (size_t i = 0; i < _meshes.size(); ++i) {
        _materials[i]->render(*_meshes[i], transform, scene);
    }
}

void Model::render_skinned(const glm::mat4& transform, const Skeleton& skeleton,
                           const Scene& scene) {
    for (size_t i = 0; i < _meshes.size(); ++i) {
        _materials[i]->render_skinned(*_meshes[i], transform, skeleton, scene);
    }
}

void Model::render_shadow(const glm::mat4& transform, const glm::mat4& light_space_matrix) {
    for (size_t i = 0; i < _meshes.size(); ++i) {
        _materials[i]->render_shadow(*_meshes[i], transform, light_space_matrix);
    }
}

void Model::render_shadow_skinned(const glm::mat4& transform, const Skeleton& skeleton,
                                   const glm::mat4& light_space_matrix) {
    for (size_t i = 0; i < _meshes.size(); ++i) {
        _materials[i]->render_shadow_skinned(*_meshes[i], transform, skeleton, light_space_matrix);
    }
}

// ============================================================================
// Legacy rendering (backwards compatibility)
// ============================================================================

void Model::render(const glm::mat4& transform, const Camera& camera,
                   const glm::vec3& light_pos, const glm::vec3& light_color) {
    for (size_t i = 0; i < _meshes.size(); ++i) {
        auto& material = _materials[i];
        material->projection = camera.projection();
        material->view = camera.view();
        material->light_pos = light_pos;
        material->view_pos = camera.position();
        material->light_color = light_color;
        material->render(*_meshes[i], transform);
    }
}

void Model::render_skinned(const glm::mat4& transform, const Skeleton& skeleton,
                           const Camera& camera, const glm::vec3& light_pos,
                           const glm::vec3& light_color) {
    for (size_t i = 0; i < _meshes.size(); ++i) {
        auto& material = _materials[i];
        material->projection = camera.projection();
        material->view = camera.view();
        material->light_pos = light_pos;
        material->view_pos = camera.position();
        material->light_color = light_color;
        material->render_skinned(*_meshes[i], transform, skeleton);
    }
}

}  // namespace engine
