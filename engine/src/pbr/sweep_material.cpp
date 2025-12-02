#include <engine/pbr/sweep_material.hpp>
#include <engine/pbr/mesh.hpp>
#include <engine/pbr/scene.hpp>

#include <glad/glad.h>

namespace engine::pbr {

SweepMaterial::SweepMaterial(ShaderLoadFunc shader_loader)
    : _shader(shader_loader("engine/shaders/sweep.vert", "engine/shaders/sweep.frag")) {
}

void SweepMaterial::render(const Mesh& mesh, const glm::mat4& transform, const Scene& scene) {
    if (!_shader || !_shader->valid()) {
        return;  // Shader failed to load
    }

    _shader->use();

    // Set uniforms using Shader's helper methods
    _shader->set_mat4("model", transform);
    _shader->set_mat4("view", scene.camera()->view());
    _shader->set_mat4("projection", scene.camera()->projection());

    _shader->set_vec3("color", color);
    _shader->set_float("alpha", alpha);
    _shader->set_float("sweepProgress", sweep_progress);
    _shader->set_float("trailLength", trail_length);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // Additive blending for glow effect

    // Disable depth writing (but keep depth testing)
    glDepthMask(GL_FALSE);

    // Draw the mesh
    glBindVertexArray(mesh.vao());
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.index_count()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // Restore state
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

}  // namespace engine::pbr
