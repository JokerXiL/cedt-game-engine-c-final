#include "state/main_game/model.hpp"

#include "mesh.hpp"
#include "material.hpp"
#include "skeleton.hpp"

void Model::draw(const glm::mat4& transform) {
    if (_mesh && _material) {
        _material->render(*_mesh, transform);
    }
}

void Model::draw_skinned(const glm::mat4& transform, const Skeleton& skeleton) {
    if (_mesh && _material) {
        _material->render_skinned(*_mesh, transform, skeleton);
    }
}
