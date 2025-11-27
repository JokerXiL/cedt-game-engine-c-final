#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

// Forward declarations
class StandardMaterial;
class Mesh;

namespace main_game {

class GameState;
class Camera;

class Renderer {
public:
    Renderer();
    ~Renderer();  // Must be defined in .cpp to delete unique_ptr of forward-declared types

    void render(const GameState& game_state, const Camera& camera, const glm::vec3& light_pos);
    void end_frame(GLFWwindow* window);

private:
    glm::mat4 _projection;

    // Materials
    std::unique_ptr<StandardMaterial> _player_material;
    std::unique_ptr<StandardMaterial> _ground_material;

    // Meshes
    std::unique_ptr<Mesh> _cube_mesh;
    std::unique_ptr<Mesh> _ground_mesh;
};

} // namespace main_game
