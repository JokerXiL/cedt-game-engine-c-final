#include <game/main_game/game_state.hpp>
#include <game/main_game/player.hpp>
#include <engine/input/input_system.hpp>
#include <engine/input/key_codes.hpp>

#include <cmath>

namespace main_game {

Player::Player()
    : _position(0.0f, 0.0f, 0.0f)
    , _velocity(0.0f)
    , _input_direction(0.0f)
    , _rotation_y(0.0f)
    , _move_speed(5.0f) {}

void Player::update(GameState& game_state, float delta) {
    // Normalize and apply movement
    if (glm::length(_input_direction) > 0.0f) {
        glm::vec3 direction = glm::normalize(_input_direction);
        _velocity = direction * _move_speed * delta;

        // Rotate player to face movement direction
        _rotation_y = atan2(direction.x, direction.z);
    } else {
        _velocity = glm::vec3(0.0f);
    }

    // Apply velocity to position
    _position += _velocity;
    _input_direction = glm::vec3(0.0f);
}

void Player::process_input(GameState& state) {
    auto& camera = state.camera;
    engine::input::InputSystem& input = engine::input::InputSystem::get_instance();

    glm::vec3 direction(0.0f);

    // Get camera-relative directions
    glm::vec3 forward = camera.forward_xz();
    glm::vec3 right = camera.right_xz();

    // WASD movement
    using engine::input::KeyCode;
    if (input.is_key_pressed(KeyCode::W)) {
        direction += forward;
    }
    if (input.is_key_pressed(KeyCode::S)) {
        direction -= forward;
    }
    if (input.is_key_pressed(KeyCode::A)) {
        direction -= right;
    }
    if (input.is_key_pressed(KeyCode::D)) {
        direction += right;
    }

    _input_direction = direction;
}

void Player::take_damage(float amount) {
    _health -= amount;
    if (_health < 0.0f) _health = 0.0f;
}

void Player::use_stamina(float amount) {
    _stamina -= amount;
    if (_stamina < 0.0f) _stamina = 0.0f;
}

void Player::heal(float amount) {
    _health += amount;
    if (_health > _max_health) _health = _max_health;
}

} // namespace main_game
