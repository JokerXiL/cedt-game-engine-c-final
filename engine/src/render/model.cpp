#include <engine/render/model.hpp>

namespace engine {

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
