#include "state/main_game.hpp"

#include <glm/glm.hpp>

#include <memory>

#include "state/game_summary.hpp"
#include "state/main_game/game_state.hpp"
#include "state/main_game/camera.hpp"
#include "state/main_game/renderer.hpp"
#include "window_system.hpp"
#include "input_system.hpp"

namespace main_game {

std::unique_ptr<State> MainGame::run() {
    // Get systems
    GLFWwindow* window = WindowSystem::get_instance().window();
    InputSystem& input = InputSystem::get_instance();

    // Capture mouse for camera control
    input.capture_mouse();

    // Create game state, renderer, and camera
    GameState game_state;
    Renderer renderer;
    Camera camera(8.0f, 25.0f, 0.0f);

    // Light position
    glm::vec3 light_pos(10.0f, 20.0f, 10.0f);

    // Timing
    float delta_time = 0.0f;
    float last_frame = 0.0f;

    // Main game loop
    while (true) {
        if (glfwWindowShouldClose(window)) {
            break;
        }

        // Check for exit
        if (input.is_key_pressed(GLFW_KEY_ESCAPE)) {
            break;
        }

        // Calculate delta time
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // Process camera input
        float dx, dy;
        input.get_mouse_delta(dx, dy);
        camera.process_mouse_movement(dx, dy);

        // Process player input and update
        game_state.player().process_input(camera, delta_time);
        camera.set_target(game_state.player().position());

        // Render
        renderer.render(game_state, camera, light_pos);
        renderer.end_frame(window);
    }

    // Release mouse
    input.release_mouse();

    return std::make_unique<game_summary::GameSummary>();
}

}  // namespace main_game
