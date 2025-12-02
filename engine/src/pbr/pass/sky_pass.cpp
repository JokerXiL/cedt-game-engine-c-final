#include <engine/pbr/pass/sky_pass.hpp>
#include <engine/pbr/scene.hpp>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace engine::pbr {

SkyPass::SkyPass(ShaderLoadFunc shader_loader)
    : PBRRenderPass("sky") {
    // Sky writes to color buffer but NOT depth buffer
    // It should render before PBR pass
    writes_resource(RenderResource::ColorBuffer);

    // Load sky shader
    _shader = shader_loader("engine/shaders/sky.vert", "engine/shaders/sky.frag");

    // Create empty VAO for fullscreen triangle (uses gl_VertexID)
    glGenVertexArrays(1, &_vao);
}

void SkyPass::execute() {
    if (!_pbr_context || !_pbr_context->scene || !_shader || !_shader->valid()) return;

    const auto* camera = _pbr_context->scene->camera();
    if (!camera) return;

    // Bind default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set viewport
    int fb_width, fb_height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    // Clear screen first (sky will fill everything)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Disable depth writing but keep depth test
    // Sky renders at far plane, real objects will render on top
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    // Use sky shader
    _shader->use();

    // Set camera matrices (inverted for ray direction calculation)
    glm::mat4 projection = camera->projection();
    glm::mat4 view = camera->view();

    _shader->set_mat4("inverseProjection", glm::inverse(projection));
    _shader->set_mat4("inverseView", glm::inverse(view));

    // Set sky parameters
    _shader->set_vec3("skyColorTop", _sky_color_top);
    _shader->set_vec3("skyColorHorizon", _sky_color_horizon);
    _shader->set_vec3("sunDirection", _sun_direction);
    _shader->set_vec3("sunColor", _sun_color);
    _shader->set_float("sunSize", _sun_size);

    // Draw fullscreen triangle
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Restore depth state
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

}  // namespace engine::pbr
