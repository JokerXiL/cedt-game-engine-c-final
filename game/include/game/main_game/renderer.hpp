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
class Mesh;
class Model;
class Scene;
class ShadowPass;
class PBRPass;
}  // namespace engine::pbr

namespace engine::ui {
class UIPass;
}  // namespace engine::ui

namespace main_game {

class GameState;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void render(const GameState& game_state, std::function<void()> ui_callback = nullptr);

    // Access scene for light manipulation
    engine::pbr::Scene& scene() { return *_scene; }
    const engine::pbr::Scene& scene() const { return *_scene; }

private:
    // Render graph
    engine::RenderGraph _graph;

    // PBR context (shared between shadow and pbr passes)
    engine::pbr::PBRContext _pbr_context;

    // Pass pointers for per-frame configuration
    engine::pbr::ShadowPass* _shadow_pass = nullptr;
    engine::pbr::PBRPass* _pbr_pass = nullptr;
    engine::ui::UIPass* _ui_pass = nullptr;

    // Scene (camera + lights)
    std::unique_ptr<engine::pbr::Scene> _scene;

    // Caches
    std::unique_ptr<engine::resource::ShaderCache> _shader_cache;
    std::unique_ptr<engine::resource::TextureCache> _texture_cache;
    std::unique_ptr<engine::resource::ModelCache> _model_cache;

    // Materials
    std::unique_ptr<engine::pbr::StandardMaterial> _ground_material;

    // Meshes
    std::unique_ptr<engine::pbr::Mesh> _ground_mesh;

    // Player model
    std::shared_ptr<engine::pbr::Model> _player_model;

    // Helper to extract player transform from game state
    glm::mat4 get_player_transform(const GameState& game_state) const;
};

} // namespace main_game
