#pragma once

#include <memory>

// Forward declarations
namespace engine {
class StandardMaterial;
class Mesh;
class Model;
class ModelCache;
class TextureCache;
class Scene;
}  // namespace engine

namespace main_game {

class GameState;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void render(const GameState& game_state);

    // Access scene for light manipulation
    engine::Scene& scene() { return *_scene; }
    const engine::Scene& scene() const { return *_scene; }

private:
    // Scene (camera + lights)
    std::unique_ptr<engine::Scene> _scene;

    // Caches
    std::unique_ptr<engine::ModelCache> _model_cache;
    std::unique_ptr<engine::TextureCache> _texture_cache;

    // Materials
    std::unique_ptr<engine::StandardMaterial> _ground_material;

    // Meshes
    std::unique_ptr<engine::Mesh> _ground_mesh;

    // Player model
    std::shared_ptr<engine::Model> _player_model;

    // Render passes
    void render_shadow_pass(const GameState& game_state);
    void render_main_pass(const GameState& game_state);
};

} // namespace main_game
