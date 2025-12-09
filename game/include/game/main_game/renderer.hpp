#pragma once

#include <engine/render/render_graph.hpp>
#include <engine/pbr/pass/pbr_render_pass.hpp>
#include <engine/resource/caches.hpp>

#include <glm/glm.hpp>

#include <functional>
#include <memory>

// Forward declarations
namespace engine::pbr {
class StandardMaterial;
class GroundMaterial;
class Mesh;
class Model;
class Scene;
class ShadowPass;
class PBRPass;
class SkyPass;
}  // namespace engine::pbr

namespace engine::ui {
class UIPass;
}  // namespace engine::ui

namespace game::main_game {
class SweepMaterial;
}

namespace main_game {

class GameState;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void render(const GameState& game_state, std::function<void()> ui_callback = nullptr);
    void update(float delta);

    // Trigger attack visual effect
    void show_melee_attack(glm::vec3 position, float rotation_y, float range);

    // Access scene for light manipulation
    engine::pbr::Scene& scene() { return *_scene; }
    const engine::pbr::Scene& scene() const { return *_scene; }
    const engine::pbr::Model& get_player_model() const { return *_player_model; } 
private:
    // Render graph
    engine::RenderGraph _graph;

    // PBR context (shared between shadow and pbr passes)
    engine::pbr::PBRContext _pbr_context;

    // Pass pointers for per-frame configuration
    engine::pbr::ShadowPass* _shadow_pass = nullptr;
    engine::pbr::SkyPass* _sky_pass = nullptr;
    engine::pbr::PBRPass* _pbr_pass = nullptr;
    engine::ui::UIPass* _ui_pass = nullptr;

    // Scene (camera + lights)
    std::unique_ptr<engine::pbr::Scene> _scene;

    // Caches
    std::unique_ptr<engine::resource::ShaderCache> _shader_cache;
    std::unique_ptr<engine::resource::TextureCache> _texture_cache;
    std::unique_ptr<engine::resource::ModelCache> _model_cache;

    // Materials
    std::unique_ptr<engine::pbr::GroundMaterial> _ground_material;
    std::unique_ptr<game::main_game::SweepMaterial> _sweep_material;
    std::unique_ptr<engine::pbr::StandardMaterial> _projectile_material;

    // Meshes
    std::unique_ptr<engine::pbr::Mesh> _ground_mesh;
    std::unique_ptr<engine::pbr::Mesh> _attack_arc_mesh;
    std::unique_ptr<engine::pbr::Mesh> _projectile_mesh;

    // Player model
    std::shared_ptr<engine::pbr::Model> _player_model;

    // Enemy mesh and materials (cube shape, different colors per type)
    std::unique_ptr<engine::pbr::Mesh> _enemy_mesh;
    std::unique_ptr<engine::pbr::StandardMaterial> _melee_enemy_material;
    std::unique_ptr<engine::pbr::StandardMaterial> _ranged_enemy_material;

    // Particle mesh (small sphere)
    std::unique_ptr<engine::pbr::Mesh> _particle_mesh;
    std::unique_ptr<engine::pbr::StandardMaterial> _particle_material;

    // Helper to extract transforms from game state
    glm::mat4 get_player_transform(const GameState& game_state) const;
    glm::mat4 get_enemy_transform(const class Enemy& enemy) const;

    // Attack visual state
    float _attack_visual_timer = 0.0f;
    glm::vec3 _attack_position{0.0f};
    float _attack_rotation_y = 0.0f;
    float _attack_range = 0.0f;
    static constexpr float ATTACK_VISUAL_DURATION = 0.2f;
};

} // namespace main_game
