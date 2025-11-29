#include <game/main_game/renderer.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <game/main_game/game_state.hpp>
#include <game/main_game/player.hpp>
#include <game/main_game/map.hpp>
#include <engine/render/render_graph.hpp>
#include <engine/pbr/standard_material.hpp>
#include <engine/pbr/mesh.hpp>
#include <engine/pbr/mesh_factory.hpp>
#include <engine/pbr/model.hpp>
#include <engine/pbr/model_cache.hpp>
#include <engine/pbr/texture_cache.hpp>
#include <engine/pbr/shader_cache.hpp>
#include <engine/pbr/scene.hpp>
#include <engine/pbr/light.hpp>
#include <engine/pbr/pass/shadow_pass.hpp>
#include <engine/pbr/pass/pbr_pass.hpp>
#include <engine/ui/pass/ui_pass.hpp>

namespace main_game {

Renderer::Renderer()
    : _scene(std::make_unique<engine::pbr::Scene>())
    , _shader_cache(std::make_unique<engine::pbr::ShaderCache>())
    , _model_cache(std::make_unique<engine::pbr::ModelCache>(*_shader_cache))
    , _texture_cache(std::make_unique<engine::pbr::TextureCache>()) {

    // Build render graph with passes
    _shadow_pass = _graph.add_pass(std::make_unique<engine::pbr::ShadowPass>());
    _pbr_pass = _graph.add_pass(std::make_unique<engine::pbr::PBRPass>());
    _ui_pass = _graph.add_pass(std::make_unique<engine::ui::UIPass>());
    _graph.compile();

    // Set shared PBR context on PBR passes
    _shadow_pass->set_context(&_pbr_context);
    _pbr_pass->set_context(&_pbr_context);
    _pbr_pass->set_shadow_pass(_shadow_pass);

    // Create ground mesh
    _ground_mesh = engine::pbr::mesh_factory::create_plane(100.0f, 100.0f);

    // Load player model (materials are created inside ModelCache)
    _player_model = _model_cache->load("player.glb");

    // Ground: green material (albedo = 0.3, 0.6, 0.3)
    _ground_material = std::make_unique<engine::pbr::StandardMaterial>(
        *_shader_cache,
        glm::vec3(0.3f, 0.6f, 0.3f),  // albedo (green)
        glm::vec3(1.0f),                // specular
        0.0f,                            // metallic
        0.8f                             // roughness
    );

    // Setup default lighting
    _scene->set_ambient(glm::vec3(0.15f, 0.15f, 0.2f));

    // Main directional light (sun) with shadows
    auto sun = engine::pbr::Light::create_directional(
        glm::vec3(-0.5f, -1.0f, -0.3f),  // direction
        glm::vec3(1.0f, 0.98f, 0.9f),    // warm white color
        1.0f                              // intensity
    );
    sun.casts_shadow = true;
    sun.shadow_map_index = 0;
    _scene->add_light(sun);

    // Fill light (point light, no shadow)
    auto fill = engine::pbr::Light::create_point(
        glm::vec3(-5.0f, 8.0f, 5.0f),   // position
        glm::vec3(0.4f, 0.5f, 0.6f),    // cool blue tint
        0.5f,                            // intensity
        30.0f                            // range
    );
    _scene->add_light(fill);
}

Renderer::~Renderer() = default;

void Renderer::render(const GameState& game_state, std::function<void()> ui_callback) {
    // Update scene camera
    _scene->set_camera(&game_state.camera.orbit_camera());

    // Build PBR context
    _pbr_context.clear();
    _pbr_context.scene = _scene.get();

    // Submit 3D renderables
    _pbr_context.submit(*_ground_mesh, *_ground_material, glm::mat4(1.0f));
    _pbr_context.submit(*_player_model, get_player_transform(game_state));

    // Set UI callback on the UI pass
    if (ui_callback) {
        _ui_pass->set_callback(std::move(ui_callback));
    }

    // Execute render graph
    _graph.execute();
}

glm::mat4 Renderer::get_player_transform(const GameState& game_state) const {
    const Player& player = game_state.player;
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, player.position());
    transform = glm::rotate(transform, player.rotation_y(), glm::vec3(0.0f, 1.0f, 0.0f));
    return transform;
}

} // namespace main_game
