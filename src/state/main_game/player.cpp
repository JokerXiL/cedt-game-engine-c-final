#include "state/main_game/player.hpp"
#include "state/main_game/camera.hpp"
#include "input_system.hpp"

#include <GLFW/glfw3.h>
#include <cmath>

namespace main_game {

Player::Player()
    : _position(0.0f, 0.5f, 0.0f)
    , _velocity(0.0f)
    , _rotation_y(0.0f)
    , _move_speed(5.0f) {}

void Player::update(GameState& game_state) {
    // Update position based on velocity
    _position += _velocity;
    _velocity = glm::vec3(0.0f);
}

void Player::process_input(const Camera& camera, float delta_time) {
    InputSystem& input = InputSystem::get_instance();

    glm::vec3 direction(0.0f);

    // Get camera-relative directions
    glm::vec3 forward = camera.get_forward();
    glm::vec3 right = camera.get_right();

    // WASD movement
    if (input.is_key_pressed(GLFW_KEY_W)) {
        direction += forward;
    }
    if (input.is_key_pressed(GLFW_KEY_S)) {
        direction -= forward;
    }
    if (input.is_key_pressed(GLFW_KEY_A)) {
        direction -= right;
    }
    if (input.is_key_pressed(GLFW_KEY_D)) {
        direction += right;
    }

    // Normalize and apply movement
    if (glm::length(direction) > 0.0f) {
        direction = glm::normalize(direction);
        _velocity = direction * _move_speed * delta_time;

        // Rotate player to face movement direction
        _rotation_y = atan2(direction.x, direction.z);
    }
}

} // namespace main_game
