#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

// Forward declarations
namespace engine {
class StandardMaterial;
class Mesh;
}  // namespace engine

namespace main_game {

class GameState;
class Camera;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void render(const GameState& game_state);

private:
    glm::mat4 _projection;

    // Materials
    std::unique_ptr<engine::StandardMaterial> _player_material;
    std::unique_ptr<engine::StandardMaterial> _ground_material;

    // Meshes
    std::unique_ptr<engine::Mesh> _cube_mesh;
    std::unique_ptr<engine::Mesh> _ground_mesh;
};

} // namespace main_game
