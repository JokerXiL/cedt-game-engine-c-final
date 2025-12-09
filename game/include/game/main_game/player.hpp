#pragma once

#include <glm/glm.hpp>

#include <game/main_game/weapon.hpp>
#include <game/main_game/player_animation_controller.hpp>
#include <engine/pbr/skeleton.hpp>

#include <memory>

namespace main_game {
class GameState;
class Camera;

class Player {
public:
    Player();

    void process_input(GameState&);
    void update(GameState&, float delta);

    void take_damage(float amount);
    void use_stamina(float amount);
    void heal(float amount);
    void attack_main(GameState& game_state);
    void attack_sub(GameState& game_state);

    glm::vec3 position() const { return _position; }
    float rotation_y() const { return _rotation_y; }

    // Weapons
    const Weapon& main_weapon() const { return *_main_weapon; }
    const Weapon& sub_weapon() const { return *_sub_weapon; }

    float health() const { return _health; }
    float max_health() const { return _max_health; }
    float stamina() const { return _stamina; }
    float max_stamina() const { return _max_stamina; }
    bool is_alive() const { return _health > 0.0f; }

    // Stat modifiers (for perks)
    float damage_multiplier() const { return _damage_multiplier; }
    float attack_speed_multiplier() const { return _attack_speed_multiplier; }
    float move_speed_multiplier() const { return _move_speed_multiplier; }
    float health_regen_rate() const { return _health_regen_rate; }
    float lifesteal_percent() const { return _lifesteal_percent; }
    float damage_reduction() const { return _damage_reduction; }

    // Perk application methods
    void add_max_health(float amount);
    void add_max_stamina(float amount);
    void multiply_damage(float multiplier);
    void multiply_attack_speed(float multiplier);
    void multiply_move_speed(float multiplier);
    void add_health_regen(float rate);
    void add_lifesteal(float percent);
    void add_damage_reduction(float percent);

    // Lifesteal callback (called when player deals damage)
    void on_damage_dealt(float damage);

    // Skeleton (for skeletal animation)
    engine::pbr::Skeleton& get_skeleton() { return _skeleton; }
    const engine::pbr::Skeleton& get_skeleton() const { return _skeleton; }

    // Animation control
    PlayerAnimationController& get_animation_controller() { return _animation_controller; }
    const PlayerAnimationController& get_animation_controller() const { return _animation_controller; }

    // Animation clips (set from game initialization)
    void set_animation_clips(
        std::shared_ptr<engine::pbr::AnimationClip> idle,
        std::shared_ptr<engine::pbr::AnimationClip> run,
        std::shared_ptr<engine::pbr::AnimationClip> melee_attack,
        std::shared_ptr<engine::pbr::AnimationClip> range_attack);

private:
    // Movement
    glm::vec3 _position;
    glm::vec3 _velocity;
    glm::vec3 _input_direction;
    float _rotation_y;
    float _move_speed;

    // Skeleton (for skeletal animation)
    engine::pbr::Skeleton _skeleton;

    // Animation controller
    PlayerAnimationController _animation_controller;

    // Stats
    float _health = 100.0f;
    float _max_health = 100.0f;
    float _stamina = 100.0f;
    float _max_stamina = 100.0f;
    float _stamina_regen_rate = 10.0f;

    // Equipment
    std::unique_ptr<Weapon> _main_weapon;
    std::unique_ptr<Weapon> _sub_weapon;

    // Attack cooldowns (separate for each weapon)
    float _main_attack_cooldown = 0.0f;
    float _sub_attack_cooldown = 0.0f;

    // Stat modifiers (from perks)
    float _damage_multiplier = 1.0f;
    float _attack_speed_multiplier = 1.0f;
    float _move_speed_multiplier = 1.0f;
    float _health_regen_rate = 0.0f;       // HP per second
    float _lifesteal_percent = 0.0f;        // % of damage dealt healed
    float _damage_reduction = 0.0f;         // % damage reduction
};
}  // namespace main_game
