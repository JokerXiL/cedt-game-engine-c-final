#include <game/main_game/game_state.hpp>
#include <engine/input/input_system.hpp>

namespace main_game {

GameState::GameState() {
    camera.set_orbit_distance(2.0f);
}

void GameState::update(float delta) {
    player.process_input(*this);
    player.update(*this, delta);

    // Update camera orbit
    engine::input::InputSystem& input = engine::input::InputSystem::get_instance();
    float dx, dy;
    input.get_mouse_delta(dx, dy);
    camera.orbit_rotate(dx * 0.1f, dy * 0.1f);

    // Zoom with scroll wheel (clamped to 0.25 - 2.0 meters)
    constexpr float MIN_CAMERA_DISTANCE = 0.25f;
    constexpr float MAX_CAMERA_DISTANCE = 2.0f;
    constexpr float ZOOM_SENSITIVITY = 0.1f;

    float scroll = input.get_scroll_delta();
    if (scroll != 0.0f) {
        float new_distance = camera.distance() - scroll * ZOOM_SENSITIVITY;
        new_distance = glm::clamp(new_distance, MIN_CAMERA_DISTANCE, MAX_CAMERA_DISTANCE);
        camera.set_orbit_distance(new_distance);
    }

    // Follow player
    camera.set_orbit_target(player.position() + glm::vec3(0.0, 1.0, 0.0));
}

} // namespace main_game
