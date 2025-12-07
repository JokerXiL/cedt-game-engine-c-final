#include <game/main_game/game_state.hpp>
#include <game/main_game/player.hpp>
#include <engine/input/input_system.hpp>
#include <engine/input/key_codes.hpp>

#include <iostream>
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

    // Apply health regeneration
    if (_health_regen_rate > 0.0f && _health < _max_health) {
        _health += _health_regen_rate * delta;
        if (_health > _max_health) _health = _max_health;
    }

    // Normalize and apply movement (with move speed multiplier)
    if (glm::length(_input_direction) > 0.0f) {
        glm::vec3 direction = glm::normalize(_input_direction);
        float effective_speed = _move_speed * _move_speed_multiplier;
        _velocity = direction * effective_speed * delta;

        // Rotate player to face movement direction
        _rotation_y = atan2(direction.x, direction.z);
    } else {
        _velocity = glm::vec3(0.0f);
    }

    // Apply velocity to position
    _position += _velocity;
    _input_direction = glm::vec3(0.0f);

    // Update animation state machine
    update_animation_state(delta);
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
    // Apply damage reduction
    float reduced_amount = amount * (1.0f - _damage_reduction);
    _health -= reduced_amount;
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

    // Apply attack speed multiplier to cooldown
    _main_attack_cooldown = _main_weapon->cooldown() / _attack_speed_multiplier;

    glm::vec3 facing = game_state.camera.get_aim_direction(_position);
    _main_weapon->attack(game_state, _position, facing);
    
    // Trigger melee attack animation
    transition_to_animation(AnimationStateType::MeleeAttack);
}

void Player::attack_sub(GameState& game_state) {
    if (_sub_attack_cooldown > 0.0f) return;

    // Apply attack speed multiplier to cooldown
    _sub_attack_cooldown = _sub_weapon->cooldown() / _attack_speed_multiplier;

    glm::vec3 facing = game_state.camera.get_aim_direction(_position);
    _sub_weapon->attack(game_state, _position, facing);
    
    // Trigger range attack animation
    transition_to_animation(AnimationStateType::RangeAttack);
}

// Perk application methods
void Player::add_max_health(float amount) {
    _max_health += amount;
    _health += amount;  // Also heal for the same amount
    if (_health > _max_health) _health = _max_health;
}

void Player::add_max_stamina(float amount) {
    _max_stamina += amount;
    _stamina += amount;
    if (_stamina > _max_stamina) _stamina = _max_stamina;
}

void Player::multiply_damage(float multiplier) {
    _damage_multiplier *= multiplier;
}

void Player::multiply_attack_speed(float multiplier) {
    _attack_speed_multiplier *= multiplier;
}

void Player::multiply_move_speed(float multiplier) {
    _move_speed_multiplier *= multiplier;
}

void Player::add_health_regen(float rate) {
    _health_regen_rate += rate;
}

void Player::add_lifesteal(float percent) {
    _lifesteal_percent += percent;
}

void Player::add_damage_reduction(float percent) {
    _damage_reduction += percent;
    // Cap at 75% damage reduction
    if (_damage_reduction > 0.75f) _damage_reduction = 0.75f;
}

void Player::on_damage_dealt(float damage) {
    if (_lifesteal_percent > 0.0f) {
        float heal_amount = damage * _lifesteal_percent;
        heal(heal_amount);
    }
}

void Player::set_animation_clips(
    std::shared_ptr<engine::pbr::AnimationClip> idle,
    std::shared_ptr<engine::pbr::AnimationClip> run,
    std::shared_ptr<engine::pbr::AnimationClip> melee_attack,
    std::shared_ptr<engine::pbr::AnimationClip> range_attack)
{
    _anim_idle = idle;
    _anim_run = run;
    _anim_melee_attack = melee_attack;
    _anim_range_attack = range_attack;
    
    // Start with idle animation
    if (_anim_idle) {
        _animation_state.set_clip(_anim_idle);
        _animation_state.set_looping(true);
        _animation_state.play();
    }
}

void Player::update_animation_state(float delta) {
    // Update attack animation timer - match weapon cooldown
    if (_is_playing_attack_animation) {
        // Check if attack animation finished based on cooldown
        float attack_duration = 0.0f;
        if (_current_anim_state == AnimationStateType::MeleeAttack) {
            attack_duration = _main_weapon->cooldown() / _attack_speed_multiplier;
        } else if (_current_anim_state == AnimationStateType::RangeAttack) {
            attack_duration = _sub_weapon->cooldown() / _attack_speed_multiplier;
        }
        
        // Animation ends when cooldown expires
        bool animation_finished = false;
        if (_current_anim_state == AnimationStateType::MeleeAttack) {
            animation_finished = (_main_attack_cooldown <= 0.0f);
        } else if (_current_anim_state == AnimationStateType::RangeAttack) {
            animation_finished = (_sub_attack_cooldown <= 0.0f);
        }
        
        if (animation_finished) {
            _is_playing_attack_animation = false;
            
            // Return to idle or run based on movement
            if (glm::length(_velocity) > 0.01f) {
                transition_to_animation(AnimationStateType::Running);
            } else {
                transition_to_animation(AnimationStateType::Idle);
            }
        }
    } else {
        // Not attacking - update based on movement
        bool is_moving = glm::length(_velocity) > 0.01f;
        
        if (is_moving && _current_anim_state != AnimationStateType::Running) {
            transition_to_animation(AnimationStateType::Running);
        } else if (!is_moving && _current_anim_state != AnimationStateType::Idle) {
            transition_to_animation(AnimationStateType::Idle);
        }
    }
    
    // Update blend progress
    if (_current_blend_progress < 1.0f) {
        _current_blend_progress += delta / _animation_blend_time;
        if (_current_blend_progress > 1.0f) {
            _current_blend_progress = 1.0f;
        }
    }
    
    // Update and apply animation
    if (_animation_state.get_clip()) {
        _animation_state.update(delta);
        _animation_state.apply(_skeleton);
    }
}

void Player::transition_to_animation(AnimationStateType new_state) {
    if (_current_anim_state == new_state) return;
    
    _current_anim_state = new_state;
    _current_blend_progress = 0.0f;  // Start blending
    
    std::shared_ptr<engine::pbr::AnimationClip> target_clip;
    bool should_loop = true;
    
    switch (new_state) {
        case AnimationStateType::Idle:
            target_clip = _anim_idle;
            should_loop = true;
            break;
            
        case AnimationStateType::Running:
            target_clip = _anim_run;
            should_loop = true;
            break;
            
        case AnimationStateType::MeleeAttack:
            target_clip = _anim_melee_attack;
            should_loop = false;
            _is_playing_attack_animation = true;
            _attack_animation_timer = 0.0f;
            break;
            
        case AnimationStateType::RangeAttack:
            target_clip = _anim_range_attack;
            should_loop = false;
            _is_playing_attack_animation = true;
            _attack_animation_timer = 0.0f;
            break;
    }
    
    if (target_clip) {
        _animation_state.set_clip(target_clip);
        _animation_state.set_looping(should_loop);
        _animation_state.set_current_time(0.0f);
        _animation_state.play();
    }
}

} // namespace main_game
