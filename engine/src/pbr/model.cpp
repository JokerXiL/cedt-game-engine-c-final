#include <engine/pbr/model.hpp>
#include <engine/pbr/scene.hpp>

namespace engine::pbr {

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

void Model::render_shadow_cube(const glm::mat4& transform, const glm::mat4& light_space_matrix,
                                const glm::vec3& light_pos, float far_plane) {
    for (size_t i = 0; i < _meshes.size(); ++i) {
        _materials[i]->render_shadow_cube(*_meshes[i], transform, light_space_matrix,
                                          light_pos, far_plane);
    }
}

void Model::render_shadow_cube_skinned(const glm::mat4& transform, const Skeleton& skeleton,
                                        const glm::mat4& light_space_matrix,
                                        const glm::vec3& light_pos, float far_plane) {
    for (size_t i = 0; i < _meshes.size(); ++i) {
        _materials[i]->render_shadow_cube_skinned(*_meshes[i], transform, skeleton,
                                                   light_space_matrix, light_pos, far_plane);
    }
}

}  // namespace engine::pbr
