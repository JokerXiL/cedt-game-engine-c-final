#include <game/main_game/model.hpp>

#include <engine/render/mesh.hpp>
#include <engine/render/material.hpp>
#include <engine/render/skeleton.hpp>

void Model::draw(const glm::mat4& transform) {
    if (_mesh && _material) {
        _material->render(*_mesh, transform);
    }
}

void Model::draw_skinned(const glm::mat4& transform, const engine::Skeleton& skeleton) {
    if (_mesh && _material) {
        _material->render_skinned(*_mesh, transform, skeleton);
    }
}
