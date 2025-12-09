#include <engine/pbr/pass/shadow_pass.hpp>
#include <engine/pbr/scene.hpp>
#include <engine/pbr/model.hpp>
#include <engine/pbr/mesh.hpp>
#include <engine/pbr/standard_material.hpp>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace engine::pbr {

ShadowPass::ShadowPass() : PBRRenderPass("shadow") {
    writes_resource(pbr::RenderResource::ShadowMaps);
}

void ShadowPass::execute() {
    if (!_pbr_context || !_pbr_context->scene) return;

    // Initialize on first use
    if (!_initialized) {
        // Initialize shadow maps
        for (auto& shadow_map : _shadow_maps) {
            shadow_map.initialize();
        }
        for (auto& cubemap : _shadow_cubemaps) {
            cubemap.initialize();
        }
        _initialized = true;
    }

    auto shadow_lights = _pbr_context->scene->get_shadow_casting_lights();
    const Camera* camera = _pbr_context->scene->camera();

    if (!camera) return;

    for (const auto* light : shadow_lights) {
        if (light->shadow_map_index < 0) continue;

        switch (light->type) {
            case LightType::Directional:
                render_directional_shadow(*light, *camera);
                break;
            case LightType::Spot:
                render_spot_shadow(*light);
                break;
            case LightType::Point:
                render_point_shadow(*light);
                break;
        }
    }

    // Restore viewport to window size
    int fb_width, fb_height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    // Populate shadow data for materials
    auto& shadow_data = _pbr_context->shadow_data;
    shadow_data.shadow_map_texture_unit = shadow_map_texture_unit();
    shadow_data.shadow_cubemap_texture_unit = shadow_cubemap_texture_unit();

    for (int i = 0; i < MAX_SHADOW_MAPS; ++i) {
        shadow_data.light_space_matrices[static_cast<size_t>(i)] = _shadow_maps[static_cast<size_t>(i)].light_space_matrix;
    }
    for (int i = 0; i < MAX_SHADOW_CUBEMAPS; ++i) {
        shadow_data.point_light_far_planes[static_cast<size_t>(i)] = _shadow_cubemaps[static_cast<size_t>(i)].far_plane;
    }
}

ShadowMap* ShadowPass::get_shadow_map(int index) {
    if (index >= 0 && index < MAX_SHADOW_MAPS) {
        return &_shadow_maps[static_cast<size_t>(index)];
    }
    return nullptr;
}

ShadowCubeMap* ShadowPass::get_shadow_cubemap(int index) {
    if (index >= 0 && index < MAX_SHADOW_CUBEMAPS) {
        return &_shadow_cubemaps[static_cast<size_t>(index)];
    }
    return nullptr;
}

void ShadowPass::bind_shadow_textures(int start_texture_unit) {
    // Bind 2D shadow maps
    for (int i = 0; i < MAX_SHADOW_MAPS; ++i) {
        glActiveTexture(GL_TEXTURE0 + start_texture_unit + i);
        glBindTexture(GL_TEXTURE_2D, _shadow_maps[static_cast<size_t>(i)].depth_texture());
    }

    // Bind cubemap shadow maps
    int cubemap_start = start_texture_unit + MAX_SHADOW_MAPS;
    for (int i = 0; i < MAX_SHADOW_CUBEMAPS; ++i) {
        glActiveTexture(GL_TEXTURE0 + cubemap_start + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _shadow_cubemaps[static_cast<size_t>(i)].depth_cubemap());
    }

    // Reset to texture unit 0
    glActiveTexture(GL_TEXTURE0);
}

void ShadowPass::render_shadow_renderables(const glm::mat4& light_space_matrix) {
    for (const auto& r : _pbr_context->renderables) {
        if (!r.casts_shadow) continue;

        if (r.model) {
            if (r.skeleton) {
                r.model->render_shadow_skinned(r.transform, *r.skeleton, light_space_matrix);
            } else {
                r.model->render_shadow(r.transform, light_space_matrix);
            }
        } else if (r.mesh && r.material) {
            if (r.skeleton) {
                r.material->render_shadow_skinned(*r.mesh, r.transform, *r.skeleton,
                                                  light_space_matrix);
            } else {
                r.material->render_shadow(*r.mesh, r.transform, light_space_matrix);
            }
        }
    }
}

void ShadowPass::render_shadow_renderables_cube(const glm::mat4& light_space_matrix,
                                                 const glm::vec3& light_pos, float far_plane) {
    for (const auto& r : _pbr_context->renderables) {
        if (!r.casts_shadow) continue;

        if (r.model) {
            if (r.skeleton) {
                r.model->render_shadow_cube_skinned(r.transform, *r.skeleton,
                                                    light_space_matrix, light_pos, far_plane);
            } else {
                r.model->render_shadow_cube(r.transform, light_space_matrix, light_pos, far_plane);
            }
        } else if (r.mesh && r.material) {
            if (r.skeleton) {
                r.material->render_shadow_cube_skinned(*r.mesh, r.transform, *r.skeleton,
                                                       light_space_matrix, light_pos, far_plane);
            } else {
                r.material->render_shadow_cube(*r.mesh, r.transform,
                                               light_space_matrix, light_pos, far_plane);
            }
        }
    }
}

void ShadowPass::render_directional_shadow(const Light& light, const Camera& camera) {
    auto* shadow_map = get_shadow_map(light.shadow_map_index);
    if (!shadow_map) return;

    shadow_map->light_space_matrix = calculate_directional_light_matrix(light, camera);
    shadow_map->bind_for_writing();
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);

    render_shadow_renderables(shadow_map->light_space_matrix);

    glCullFace(GL_BACK);
    shadow_map->unbind();
}

void ShadowPass::render_spot_shadow(const Light& light) {
    auto* shadow_map = get_shadow_map(light.shadow_map_index);
    if (!shadow_map) return;

    shadow_map->light_space_matrix = calculate_spot_light_matrix(light);
    shadow_map->bind_for_writing();
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);

    render_shadow_renderables(shadow_map->light_space_matrix);

    glCullFace(GL_BACK);
    shadow_map->unbind();
}

void ShadowPass::render_point_shadow(const Light& light) {
    auto* cubemap = get_shadow_cubemap(light.shadow_map_index);
    if (!cubemap) return;

    float far_plane = 100.0f;
    cubemap->far_plane = far_plane;

    auto matrices = calculate_point_light_matrices(light, far_plane);
    for (int i = 0; i < 6; ++i) {
        cubemap->light_space_matrices[i] = matrices[i];
    }

    for (int face = 0; face < 6; ++face) {
        cubemap->bind_for_writing(face);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);

        render_shadow_renderables_cube(cubemap->light_space_matrices[face],
                                       light.position, far_plane);

        glCullFace(GL_BACK);
        cubemap->unbind();
    }
}

glm::mat4 ShadowPass::calculate_directional_light_matrix(const Light& light, const Camera& camera) {
    glm::vec3 center = camera.position();

    float ortho_size = 50.0f;
    float near_plane = 0.1f;
    float far_plane = 100.0f;

    glm::mat4 light_projection = glm::ortho(
        -ortho_size, ortho_size,
        -ortho_size, ortho_size,
        near_plane, far_plane
    );

    glm::vec3 light_pos = center - light.direction * 50.0f;
    glm::mat4 light_view = glm::lookAt(
        light_pos,
        center,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return light_projection * light_view;
}

glm::mat4 ShadowPass::calculate_spot_light_matrix(const Light& light) {
    float aspect = 1.0f;
    float near_plane = 0.1f;
    float far_plane = 100.0f;
    float fov = light.outer_cutoff * 2.0f;

    glm::mat4 light_projection = glm::perspective(fov, aspect, near_plane, far_plane);
    glm::mat4 light_view = glm::lookAt(
        light.position,
        light.position + light.direction,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return light_projection * light_view;
}

std::array<glm::mat4, 6> ShadowPass::calculate_point_light_matrices(const Light& light, float far_plane) {
    float near_plane = 0.1f;
    glm::mat4 shadow_proj = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

    std::array<glm::mat4, 6> matrices;
    matrices[0] = shadow_proj * glm::lookAt(light.position, light.position + glm::vec3( 1, 0, 0), glm::vec3(0,-1, 0));
    matrices[1] = shadow_proj * glm::lookAt(light.position, light.position + glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0));
    matrices[2] = shadow_proj * glm::lookAt(light.position, light.position + glm::vec3( 0, 1, 0), glm::vec3(0, 0, 1));
    matrices[3] = shadow_proj * glm::lookAt(light.position, light.position + glm::vec3( 0,-1, 0), glm::vec3(0, 0,-1));
    matrices[4] = shadow_proj * glm::lookAt(light.position, light.position + glm::vec3( 0, 0, 1), glm::vec3(0,-1, 0));
    matrices[5] = shadow_proj * glm::lookAt(light.position, light.position + glm::vec3( 0, 0,-1), glm::vec3(0,-1, 0));

    return matrices;
}

}  // namespace engine::pbr
