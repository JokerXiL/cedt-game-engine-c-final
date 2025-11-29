#include <game/main_game/game_state.hpp>
#include <game/main_game/player.hpp>
#include <engine/input/input_system.hpp>
#include <engine/input/key_codes.hpp>

#include <cmath>
#include <limits>

namespace main_game {

Player::Player()
    : _position(0.0f, 0.0f, 0.0f)
    , _velocity(0.0f)
    , _input_direction(0.0f)
    , _rotation_y(0.0f)
    , _move_speed(5.0f)
    , _main_weapon(std::make_unique<Sword>())
    , _sub_weapon(std::make_unique<Gun>()) {}

void Player::update(GameState& game_state, float delta) {
    // Update attack cooldowns
    if (_main_attack_cooldown > 0.0f) {
        _main_attack_cooldown -= delta;
    }
    if (_sub_attack_cooldown > 0.0f) {
        _sub_attack_cooldown -= delta;
    }

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

    // Left-click: main weapon (sword)
    if (input.is_mouse_button_just_pressed(0)) {
        attack_main(state);
    }

    // Right-click: sub weapon (gun)
    if (input.is_mouse_button_just_pressed(1)) {
        attack_sub(state);
    }
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

void Player::attack_main(GameState& game_state) {
    if (_main_attack_cooldown > 0.0f) return;

    _main_attack_cooldown = _main_weapon->cooldown();

    glm::vec3 facing = game_state.camera.get_aim_direction(_position);
    _main_weapon->attack(game_state, _position, facing);
}

void Player::attack_sub(GameState& game_state) {
    if (_sub_attack_cooldown > 0.0f) return;

    _sub_attack_cooldown = _sub_weapon->cooldown();

    glm::vec3 facing = game_state.camera.get_aim_direction(_position);
    _sub_weapon->attack(game_state, _position, facing);
}

} // namespace main_game
