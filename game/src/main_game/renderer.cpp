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
#include <engine/render/camera.hpp>

namespace main_game {

Renderer::Renderer()
    : _projection(glm::perspective(glm::radians(45.0f),
                  static_cast<float>(engine::window::SCR_WIDTH) / static_cast<float>(engine::window::SCR_HEIGHT),
                  0.1f, 100.0f)) {

    // Create meshes
    _cube_mesh = engine::mesh_factory::create_cube(1.0f);
    _ground_mesh = engine::mesh_factory::create_plane(100.0f, 100.0f);

    // Create materials with different colors
    // Player: blue material (albedo = 0.2, 0.4, 0.8)
    _player_material = std::make_unique<engine::StandardMaterial>(
        glm::vec3(0.2f, 0.4f, 0.8f),  // albedo (blue)
        glm::vec3(1.0f),                // specular
        0.1f,                            // metallic
        0.6f                             // roughness
    );

    // Ground: green material (albedo = 0.3, 0.6, 0.3)
    _ground_material = std::make_unique<engine::StandardMaterial>(
        glm::vec3(0.3f, 0.6f, 0.3f),  // albedo (green)
        glm::vec3(1.0f),                // specular
        0.0f,                            // metallic
        0.8f                             // roughness
    );
}

Renderer::~Renderer() = default;

void Renderer::render(const GameState& game_state) {
    const auto& camera = game_state.camera;

    glm::vec3 light_pos(10.0f, 20.0f, 10.0f);

    // Clear screen
    engine::render::RenderSystem::get_instance().clear_screen();

    // Update ground material parameters
    _ground_material->projection = camera.projection();
    _ground_material->view = camera.view();
    _ground_material->light_pos = light_pos;
    _ground_material->view_pos = camera.position();
    _ground_material->light_color = glm::vec3(1.0f, 1.0f, 1.0f);

    // Render ground
    glm::mat4 ground_transform = glm::mat4(1.0f);
    _ground_material->render(*_ground_mesh, ground_transform);

    // Render player
    const Player& player = game_state.player;
    glm::mat4 player_transform = glm::mat4(1.0f);
    player_transform = glm::translate(player_transform, player.position());
    player_transform = glm::rotate(player_transform, player.rotation_y(), glm::vec3(0.0f, 1.0f, 0.0f));
    _player_material->projection = camera.projection();
    _player_material->view = camera.view();
    _player_material->light_pos = light_pos;
    _player_material->view_pos = camera.position();
    _player_material->render(*_cube_mesh, player_transform);
}

} // namespace main_game
