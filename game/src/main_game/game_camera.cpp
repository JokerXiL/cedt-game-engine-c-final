#include <game/main_game/game_camera.hpp>
#include <game/main_game/game_state.hpp>
#include <engine/input/input_system.hpp>
#include <engine/input/key_codes.hpp>

#include <cmath>

namespace main_game {

GameCamera::GameCamera() {
    _camera.set_orbit_distance(NORMAL_DISTANCE);
}

void GameCamera::process_input() {
    engine::input::InputSystem& input = engine::input::InputSystem::get_instance();

    // Rotate camera based on mouse movement
    float dx, dy;
    input.get_mouse_delta(dx, dy);
    _camera.orbit_rotate(dx * ROTATION_SENSITIVITY, dy * ROTATION_SENSITIVITY);

    // Q key: swap shoulder side
    if (input.is_key_just_pressed(engine::input::KeyCode::Q)) {
        swap_shoulder();
    }

    // Middle-click: toggle aim
    if (input.is_mouse_button_just_pressed(2)) {
        toggle_aim();
    }
}

void GameCamera::clamp_camera_height() {
    glm::vec3 pos = _camera.position();
    if (pos.y < MIN_CAMERA_HEIGHT) {
        pos.y = MIN_CAMERA_HEIGHT;
        _camera.set_position_direct(pos);
    }
}

void GameCamera::update(GameState& state, float delta) {
    // Smoothly blend aim mode (for zoom)
    float target_blend = _is_aiming ? 1.0f : 0.0f;
    if (_aim_blend < target_blend) {
        _aim_blend = glm::min(_aim_blend + AIM_BLEND_SPEED * delta, target_blend);
    } else if (_aim_blend > target_blend) {
        _aim_blend = glm::max(_aim_blend - AIM_BLEND_SPEED * delta, target_blend);
    }

    // Smoothly blend shoulder side
    if (_shoulder_side_current < _shoulder_side_target) {
        _shoulder_side_current = glm::min(_shoulder_side_current + SHOULDER_BLEND_SPEED * delta, _shoulder_side_target);
    } else if (_shoulder_side_current > _shoulder_side_target) {
        _shoulder_side_current = glm::max(_shoulder_side_current - SHOULDER_BLEND_SPEED * delta, _shoulder_side_target);
    }

    // Base target is player position + height offset
    glm::vec3 base_target = state.player.position() + glm::vec3(0.0f, 1.0f, 0.0f);

    // Always use over-the-shoulder offset (Warframe style)
    glm::vec3 right = _camera.right_xz();
    glm::vec3 shoulder_offset = right * AIM_SHOULDER_OFFSET * _shoulder_side_current;

    _camera.set_orbit_target(base_target + shoulder_offset);

    // Zoom in when aiming
    float current_distance = glm::mix(NORMAL_DISTANCE, AIM_DISTANCE, _aim_blend);
    _camera.set_orbit_distance(current_distance);

    // Clamp camera position to stay above ground (after all orbit updates)
    clamp_camera_height();
}

glm::vec3 GameCamera::get_aim_direction(const glm::vec3& player_position) const {
    // Cast ray from camera in forward direction
    glm::vec3 ray_origin = _camera.position();
    glm::vec3 ray_dir = _camera.forward();

    // Find target point along the ray (where crosshair is pointing)
    // Use a point far along the ray
    glm::vec3 target_point = ray_origin + ray_dir * 50.0f;

    // Projectile spawns at player position + height offset
    glm::vec3 spawn_pos = player_position + glm::vec3(0.0f, 1.0f, 0.0f);

    // Calculate direction from spawn position to target point
    glm::vec3 aim_dir = target_point - spawn_pos;
    if (glm::length(aim_dir) > 0.001f) {
        return glm::normalize(aim_dir);
    }

    // Fallback to camera forward
    return ray_dir;
}

}  // namespace main_game
