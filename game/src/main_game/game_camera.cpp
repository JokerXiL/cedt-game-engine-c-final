#include <game/main_game/game_camera.hpp>
#include <game/main_game/game_state.hpp>
#include <engine/input/input_system.hpp>

#include <cmath>

namespace main_game {

GameCamera::GameCamera() {
    _camera.set_orbit_distance(MAX_DISTANCE);
}

void GameCamera::process_input() {
    engine::input::InputSystem& input = engine::input::InputSystem::get_instance();

    // Rotate camera based on mouse movement
    float dx, dy;
    input.get_mouse_delta(dx, dy);
    _camera.orbit_rotate(dx * ROTATION_SENSITIVITY, dy * ROTATION_SENSITIVITY);

    // Constrain pitch to prevent camera going below ground
    constrain_pitch_above_ground();

    // Zoom with scroll wheel
    float scroll = input.get_scroll_delta();
    if (scroll != 0.0f) {
        float new_distance = _camera.distance() - scroll * ZOOM_SENSITIVITY;
        new_distance = glm::clamp(new_distance, MIN_DISTANCE, MAX_DISTANCE);
        _camera.set_orbit_distance(new_distance);
    }
}

void GameCamera::update(GameState& state) {
    // Follow player
    _camera.set_orbit_target(state.player.position() + glm::vec3(0.0f, 1.0f, 0.0f));
}

void GameCamera::constrain_pitch_above_ground() {
    float target_y = _camera.target().y;
    float distance = _camera.distance();
    float sin_min = (MIN_CAMERA_HEIGHT - target_y) / distance;

    if (sin_min > -1.0f && sin_min < 1.0f) {
        float min_pitch = glm::degrees(std::asin(sin_min));
        if (_camera.pitch() < min_pitch) {
            _camera.set_pitch(min_pitch);
        }
    }
}

}  // namespace main_game
