#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.hpp"

namespace main_game {

class GameState;
class Camera;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void render(const GameState& game_state, const Camera& camera, const glm::vec3& light_pos);
    void end_frame(GLFWwindow* window);

private:
    void init_graphics();
    void cleanup();

    Shader _shader;
    glm::mat4 _projection;

    // Player cube
    unsigned int _player_vao;
    unsigned int _player_vbo;

    // Ground
    unsigned int _ground_vao;
    unsigned int _ground_vbo;
};

} // namespace main_game
