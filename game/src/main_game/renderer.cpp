#include <game/main_game/renderer.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <game/main_game/game_state.hpp>
#include <game/main_game/player.hpp>
#include <game/main_game/map.hpp>
#include <engine/window/window_system.hpp>
#include <engine/render/render_system.hpp>
#include <engine/render/standard_material.hpp>
#include <engine/render/mesh.hpp>
#include <engine/render/mesh_factory.hpp>
#include <engine/render/model.hpp>
#include <engine/render/model_cache.hpp>
#include <engine/render/texture_cache.hpp>
#include <engine/render/camera.hpp>
#include <engine/render/scene.hpp>
#include <engine/render/light.hpp>
#include <engine/render/shadow_map.hpp>

namespace main_game {

Renderer::Renderer()
    : _scene(std::make_unique<engine::Scene>())
    , _model_cache(std::make_unique<engine::ModelCache>())
    , _texture_cache(std::make_unique<engine::TextureCache>()) {

    // Initialize shadow system
    engine::ShadowSystem::get_instance().initialize();
    engine::StandardMaterial::initialize_shadow_shaders();

    // Create ground mesh
    _ground_mesh = engine::mesh_factory::create_plane(100.0f, 100.0f);

    // Load player model (materials are created inside ModelCache)
    _player_model = _model_cache->load("player.glb");

    // Ground: green material (albedo = 0.3, 0.6, 0.3)
    _ground_material = std::make_unique<engine::StandardMaterial>(
        glm::vec3(0.3f, 0.6f, 0.3f),  // albedo (green)
        glm::vec3(1.0f),                // specular
        0.0f,                            // metallic
        0.8f                             // roughness
    );

    // Setup default lighting
    _scene->set_ambient(glm::vec3(0.15f, 0.15f, 0.2f));

    // Main directional light (sun) with shadows
    auto sun = engine::Light::create_directional(
        glm::vec3(-0.5f, -1.0f, -0.3f),  // direction
        glm::vec3(1.0f, 0.98f, 0.9f),    // warm white color
        1.0f                              // intensity
    );
    sun.casts_shadow = true;
    sun.shadow_map_index = 0;
    _scene->add_light(sun);

    // Fill light (point light, no shadow)
    auto fill = engine::Light::create_point(
        glm::vec3(-5.0f, 8.0f, 5.0f),   // position
        glm::vec3(0.4f, 0.5f, 0.6f),    // cool blue tint
        0.5f,                            // intensity
        30.0f                            // range
    );
    _scene->add_light(fill);
}

Renderer::~Renderer() = default;

void Renderer::render(const GameState& game_state) {
    // Update scene camera
    _scene->set_camera(&game_state.camera.orbit_camera());

    // Shadow pass
    render_shadow_pass(game_state);

    // Main render pass
    render_main_pass(game_state);
}

void Renderer::render_shadow_pass(const GameState& game_state) {
    auto shadow_lights = _scene->get_shadow_casting_lights();
    auto& shadow_system = engine::ShadowSystem::get_instance();

    for (const auto* light : shadow_lights) {
        if (light->shadow_map_index < 0) continue;

        if (light->type == engine::LightType::Directional) {
            auto* shadow_map = shadow_system.get_shadow_map(light->shadow_map_index);
            if (!shadow_map) continue;

            // Calculate orthographic light space matrix for directional light
            // Center around camera position for better shadow coverage
            const auto& camera = *_scene->camera();
            glm::vec3 center = camera.position();

            float ortho_size = 50.0f;  // Size of shadow frustum
            float near_plane = 0.1f;
            float far_plane = 100.0f;

            glm::mat4 light_projection = glm::ortho(
                -ortho_size, ortho_size,
                -ortho_size, ortho_size,
                near_plane, far_plane
            );

            // Position light at a distance along its direction from the center
            glm::vec3 light_pos = center - light->direction * 50.0f;
            glm::mat4 light_view = glm::lookAt(
                light_pos,
                center,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );

            shadow_map->light_space_matrix = light_projection * light_view;

            // Render to shadow map
            shadow_map->bind_for_writing();
            glEnable(GL_DEPTH_TEST);
            glCullFace(GL_FRONT);  // Reduce shadow acne

            // Render ground to shadow map
            _ground_material->render_shadow(*_ground_mesh, glm::mat4(1.0f),
                                            shadow_map->light_space_matrix);

            // Render player to shadow map
            const Player& player = game_state.player;
            glm::mat4 player_transform = glm::mat4(1.0f);
            player_transform = glm::translate(player_transform, player.position());
            player_transform = glm::rotate(player_transform, player.rotation_y(), glm::vec3(0.0f, 1.0f, 0.0f));

            _player_model->render_shadow(player_transform, shadow_map->light_space_matrix);

            glCullFace(GL_BACK);  // Restore
            shadow_map->unbind();
        }
        else if (light->type == engine::LightType::Spot) {
            auto* shadow_map = shadow_system.get_shadow_map(light->shadow_map_index);
            if (!shadow_map) continue;

            // Perspective projection for spot light
            float aspect = 1.0f;
            float near_plane = 0.1f;
            float far_plane = 100.0f;
            float fov = light->outer_cutoff * 2.0f;

            glm::mat4 light_projection = glm::perspective(fov, aspect, near_plane, far_plane);
            glm::mat4 light_view = glm::lookAt(
                light->position,
                light->position + light->direction,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );

            shadow_map->light_space_matrix = light_projection * light_view;

            // Render to shadow map
            shadow_map->bind_for_writing();
            glEnable(GL_DEPTH_TEST);
            glCullFace(GL_FRONT);

            _ground_material->render_shadow(*_ground_mesh, glm::mat4(1.0f),
                                            shadow_map->light_space_matrix);

            const Player& player = game_state.player;
            glm::mat4 player_transform = glm::mat4(1.0f);
            player_transform = glm::translate(player_transform, player.position());
            player_transform = glm::rotate(player_transform, player.rotation_y(), glm::vec3(0.0f, 1.0f, 0.0f));

            _player_model->render_shadow(player_transform, shadow_map->light_space_matrix);

            glCullFace(GL_BACK);
            shadow_map->unbind();
        }
        else if (light->type == engine::LightType::Point) {
            // Point light shadow requires cubemap rendering (6 faces)
            auto* cubemap = shadow_system.get_shadow_cubemap(light->shadow_map_index);
            if (!cubemap) continue;

            float near_plane = 0.1f;
            float far_plane = 100.0f;
            cubemap->far_plane = far_plane;

            glm::mat4 shadow_proj = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

            // Calculate 6 view matrices for cube faces
            cubemap->light_space_matrices[0] = shadow_proj * glm::lookAt(light->position, light->position + glm::vec3( 1, 0, 0), glm::vec3(0,-1, 0));
            cubemap->light_space_matrices[1] = shadow_proj * glm::lookAt(light->position, light->position + glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0));
            cubemap->light_space_matrices[2] = shadow_proj * glm::lookAt(light->position, light->position + glm::vec3( 0, 1, 0), glm::vec3(0, 0, 1));
            cubemap->light_space_matrices[3] = shadow_proj * glm::lookAt(light->position, light->position + glm::vec3( 0,-1, 0), glm::vec3(0, 0,-1));
            cubemap->light_space_matrices[4] = shadow_proj * glm::lookAt(light->position, light->position + glm::vec3( 0, 0, 1), glm::vec3(0,-1, 0));
            cubemap->light_space_matrices[5] = shadow_proj * glm::lookAt(light->position, light->position + glm::vec3( 0, 0,-1), glm::vec3(0,-1, 0));

            // Render to each cube face
            for (int face = 0; face < 6; ++face) {
                cubemap->bind_for_writing(face);
                glEnable(GL_DEPTH_TEST);

                // Note: For full point light shadows, you'd render all scene objects here
                // For now, using a simplified single-face approach

                cubemap->unbind();
            }
        }
    }

    // Restore viewport to window size
    glViewport(0, 0, engine::window::SCR_WIDTH, engine::window::SCR_HEIGHT);
}

void Renderer::render_main_pass(const GameState& game_state) {
    // Ensure we're rendering to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Get actual framebuffer size (important for Retina displays)
    int fb_width, fb_height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    // Clear screen
    engine::render::RenderSystem::get_instance().clear_screen();

    // Render ground using Scene
    glm::mat4 ground_transform = glm::mat4(1.0f);
    _ground_material->render(*_ground_mesh, ground_transform, *_scene);

    // Render player model using Scene
    const Player& player = game_state.player;
    glm::mat4 player_transform = glm::mat4(1.0f);
    player_transform = glm::translate(player_transform, player.position());
    player_transform = glm::rotate(player_transform, player.rotation_y(), glm::vec3(0.0f, 1.0f, 0.0f));

    _player_model->render(player_transform, *_scene);
}

} // namespace main_game
