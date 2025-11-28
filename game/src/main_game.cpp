#include <game/main_game.hpp>

#include <glm/glm.hpp>

#include <iostream>
#include <memory>

#include <game/game_summary.hpp>
#include <game/main_game/game_state.hpp>
#include <game/main_game/camera.hpp>
#include <game/main_game/renderer.hpp>
#include <engine/window_system.hpp>
#include <engine/input_system.hpp>

namespace main_game {

std::unique_ptr<State> MainGame::run() {
    auto& window_system = engine::WindowSystem::get_instance();
    auto& input_system = engine::InputSystem::get_instance();

    input_system.capture_mouse();

    GameState game_state;
    Renderer renderer;

    float delta_time = 0.0f;
    float last_frame = 0.0f;

    while (true) {
        // Check for exit
        if (window_system.should_close() || input_system.is_key_pressed(GLFW_KEY_ESCAPE)) {
            break;
        }

        // Calculate delta time
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // Process input
        game_state.process_input();

        // Update
        game_state.update(delta_time);

        // Debug output
        auto cam_pos = game_state.camera.position();
        auto player_pos = game_state.player.position();
        std::cout << "Camera: (" << cam_pos.x << ", " << cam_pos.y << ", " << cam_pos.z << ") "
                  << "Player: (" << player_pos.x << ", " << player_pos.y << ", " << player_pos.z << ")\n";

        // Render
        renderer.render(game_state);

        window_system.swap_buffer();
        window_system.poll_events();
    }

    input_system.release_mouse();

    auto result = std::make_unique<game_summary::GameSummary>(); // todo
    return result;
}

}  // namespace main_game
