#include "state/main_game/renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "state/main_game/game_state.hpp"
#include "state/main_game/player.hpp"
#include "state/main_game/map.hpp"
#include "state/main_game/camera.hpp"
#include "window_system.hpp"

namespace main_game {

// Cube vertices with normals
static float cube_vertices[] = {
    // positions          // normals
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

// Ground plane vertices with normals
static float ground_vertices[] = {
    // positions          // normals
    -50.0f, 0.0f, -50.0f,  0.0f, 1.0f, 0.0f,
     50.0f, 0.0f, -50.0f,  0.0f, 1.0f, 0.0f,
     50.0f, 0.0f,  50.0f,  0.0f, 1.0f, 0.0f,
     50.0f, 0.0f,  50.0f,  0.0f, 1.0f, 0.0f,
    -50.0f, 0.0f,  50.0f,  0.0f, 1.0f, 0.0f,
    -50.0f, 0.0f, -50.0f,  0.0f, 1.0f, 0.0f
};

Renderer::Renderer()
    : _shader("shaders/basic.vert", "shaders/basic.frag")
    , _projection(glm::perspective(glm::radians(45.0f),
                  static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                  0.1f, 100.0f))
    , _player_vao(0)
    , _player_vbo(0)
    , _ground_vao(0)
    , _ground_vbo(0) {
    init_graphics();
}

Renderer::~Renderer() {
    cleanup();
}

void Renderer::init_graphics() {
    // Set up player cube VAO/VBO
    glGenVertexArrays(1, &_player_vao);
    glGenBuffers(1, &_player_vbo);

    glBindVertexArray(_player_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _player_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set up ground VAO/VBO
    glGenVertexArrays(1, &_ground_vao);
    glGenBuffers(1, &_ground_vbo);

    glBindVertexArray(_ground_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _ground_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices), ground_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Renderer::render(const GameState& game_state, const Camera& camera, const glm::vec3& light_pos) {
    // Clear screen
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use shader
    _shader.use();

    // Set common uniforms
    _shader.set_mat4("projection", _projection);
    _shader.set_mat4("view", camera.get_view_matrix());
    _shader.set_vec3("lightPos", light_pos);
    _shader.set_vec3("viewPos", camera.get_position());

    // Render ground
    glm::mat4 ground_model = glm::mat4(1.0f);
    _shader.set_mat4("model", ground_model);
    _shader.set_vec3("objectColor", 0.3f, 0.6f, 0.3f);
    glBindVertexArray(_ground_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Render player
    const Player& player = game_state.player();
    glm::mat4 player_model = glm::mat4(1.0f);
    player_model = glm::translate(player_model, player.position());
    player_model = glm::rotate(player_model, player.rotation_y(), glm::vec3(0.0f, 1.0f, 0.0f));
    _shader.set_mat4("model", player_model);
    _shader.set_vec3("objectColor", 0.2f, 0.4f, 0.8f);
    glBindVertexArray(_player_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::end_frame(GLFWwindow* window) {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::cleanup() {
    if (_player_vao != 0) {
        glDeleteVertexArrays(1, &_player_vao);
        _player_vao = 0;
    }
    if (_player_vbo != 0) {
        glDeleteBuffers(1, &_player_vbo);
        _player_vbo = 0;
    }
    if (_ground_vao != 0) {
        glDeleteVertexArrays(1, &_ground_vao);
        _ground_vao = 0;
    }
    if (_ground_vbo != 0) {
        glDeleteBuffers(1, &_ground_vbo);
        _ground_vbo = 0;
    }
}

} // namespace main_game
