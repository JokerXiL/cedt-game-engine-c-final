#pragma once

#include <glm/glm.hpp>

namespace main_game {
class GameState;

enum class EnemyType {
    Melee,   // Chases player, attacks in close range
    Ranged   // Keeps distance, shoots projectiles
};

enum class EnemyState {
    Idle,      // Not aware of player
    Chasing,   // Moving toward player
    Attacking, // Actively attacking
    Dead       // Dead, awaiting removal
};

class Enemy {
public:
    Enemy(EnemyType type, glm::vec3 spawn_position);

    void update(GameState& game_state, float delta);
    void take_damage(float amount, GameState& game_state, glm::vec3 impact_pos);

    // Accessors
    glm::vec3 position() const { return _position; }
    float rotation_y() const { return _rotation_y; }
    float health() const { return _health; }
    float max_health() const { return _max_health; }
    bool is_alive() const { return _health > 0.0f; }
    bool should_remove() const { return _state == EnemyState::Dead && _death_timer >= DEATH_DURATION; }
    EnemyType type() const { return _type; }
    EnemyState state() const { return _state; }

    // Collision radius for hit detection
    float collision_radius() const { return _collision_radius; }

private:
    // AI state handlers
    void update_idle(GameState& game_state, float delta);
    void update_chasing(GameState& game_state, float delta);
    void update_attacking(GameState& game_state, float delta);
    void update_dead(GameState& game_state, float delta);

    // Helpers
    float distance_to_player(const GameState& game_state) const;
    glm::vec3 direction_to_player(const GameState& game_state) const;
    void face_player(const GameState& game_state);
    void move_toward_player(const GameState& game_state, float delta);
    void attack_melee(GameState& game_state);
    void attack_ranged(GameState& game_state);

    // Type and state
    EnemyType _type;
    EnemyState _state = EnemyState::Idle;

    // Transform
    glm::vec3 _position;
    glm::vec3 _velocity{0.0f};
    float _rotation_y = 0.0f;

    // Stats (set by type)
    float _health;
    float _max_health;
    float _move_speed;
    float _damage;
    float _attack_range;
    float _detection_range;
    float _collision_radius = 0.5f;

    // Attack timing
    float _attack_cooldown = 0.0f;
    float _attack_rate;  // Attacks per second

    // Death timer
    float _death_timer = 0.0f;
    static constexpr float DEATH_DURATION = 1.0f;  // Time before removal
};

}  // namespace main_game
