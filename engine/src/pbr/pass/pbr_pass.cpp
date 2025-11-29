#include <engine/pbr/pass/pbr_pass.hpp>
#include <engine/pbr/pass/shadow_pass.hpp>
#include <engine/pbr/scene.hpp>
#include <engine/pbr/model.hpp>
#include <engine/pbr/mesh.hpp>
#include <engine/pbr/standard_material.hpp>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace engine::pbr {

PBRPass::PBRPass() : PBRRenderPass("pbr") {
    reads_resource(pbr::RenderResource::ShadowMaps);
    writes_resource(pbr::RenderResource::ColorBuffer);
    writes_resource(pbr::RenderResource::DepthBuffer);
}

void PBRPass::execute() {
    if (!_pbr_context || !_pbr_context->scene) return;

    // Set shadow data on scene for materials to access
    _pbr_context->scene->set_shadow_data(&_pbr_context->shadow_data);

    // Bind default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set viewport to window size
    int fb_width, fb_height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    // Clear screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind shadow textures for sampling
    if (_shadow_pass) {
        _shadow_pass->bind_shadow_textures();
    }

    // Render all objects with PBR
    for (const auto& r : _pbr_context->renderables) {
        if (r.model) {
            if (r.skeleton) {
                r.model->render_skinned(r.transform, *r.skeleton, *_pbr_context->scene);
            } else {
                r.model->render(r.transform, *_pbr_context->scene);
            }
        } else if (r.mesh && r.material) {
            if (r.skeleton) {
                r.material->render_skinned(*r.mesh, r.transform, *r.skeleton, *_pbr_context->scene);
            } else {
                r.material->render(*r.mesh, r.transform, *_pbr_context->scene);
            }
        }
    }
}

}  // namespace engine::pbr
