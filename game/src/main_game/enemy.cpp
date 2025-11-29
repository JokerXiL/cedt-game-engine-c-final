#include <game/main_game/enemy.hpp>
#include <game/main_game/game_state.hpp>

#include <cmath>

namespace main_game {

// Type-specific stats
struct EnemyStats {
    float health;
    float move_speed;
    float damage;
    float attack_range;
    float detection_range;
    float attack_rate;
    float collision_radius;
};

static const EnemyStats MELEE_STATS = {
    .health = 50.0f,
    .move_speed = 3.5f,
    .damage = 15.0f,
    .attack_range = 1.5f,
    .detection_range = 15.0f,
    .attack_rate = 1.0f,
    .collision_radius = 0.5f
};

static const EnemyStats RANGED_STATS = {
    .health = 30.0f,
    .move_speed = 2.0f,
    .damage = 10.0f,
    .attack_range = 12.0f,
    .detection_range = 20.0f,
    .attack_rate = 0.5f,
    .collision_radius = 0.5f
};

Enemy::Enemy(EnemyType type, glm::vec3 spawn_position)
    : _type(type)
    , _position(spawn_position)
{
    const EnemyStats& stats = (type == EnemyType::Melee) ? MELEE_STATS : RANGED_STATS;
    _health = stats.health;
    _max_health = stats.health;
    _move_speed = stats.move_speed;
    _damage = stats.damage;
    _attack_range = stats.attack_range;
    _detection_range = stats.detection_range;
    _attack_rate = stats.attack_rate;
    _collision_radius = stats.collision_radius;
}

void Enemy::update(GameState& game_state, float delta) {
    // Update attack cooldown
    if (_attack_cooldown > 0.0f) {
        _attack_cooldown -= delta;
    }

    // State machine
    switch (_state) {
        case EnemyState::Idle:
            update_idle(game_state, delta);
            break;
        case EnemyState::Chasing:
            update_chasing(game_state, delta);
            break;
        case EnemyState::Attacking:
            update_attacking(game_state, delta);
            break;
        case EnemyState::Dead:
            update_dead(game_state, delta);
            break;
    }
}

void Enemy::update_idle(GameState& game_state, float delta) {
    float dist = distance_to_player(game_state);
    if (dist < _detection_range && game_state.player.is_alive()) {
        _state = EnemyState::Chasing;
    }
}

void Enemy::update_chasing(GameState& game_state, float delta) {
    if (!game_state.player.is_alive()) {
        _state = EnemyState::Idle;
        return;
    }

    float dist = distance_to_player(game_state);

    if (_type == EnemyType::Melee) {
        // Melee: attack when close
        if (dist <= _attack_range) {
            _state = EnemyState::Attacking;
            return;
        }
    } else {
        // Ranged: maintain preferred distance
        float preferred_distance = _attack_range * 0.7f;

        if (dist <= _attack_range && dist >= preferred_distance) {
            _state = EnemyState::Attacking;
            return;
        }

        // Too close - back away
        if (dist < preferred_distance) {
            glm::vec3 away = -direction_to_player(game_state);
            _velocity = away * _move_speed;
            _position += _velocity * delta;
            face_player(game_state);
            return;
        }
    }

    // Move toward player
    move_toward_player(game_state, delta);
}

void Enemy::update_attacking(GameState& game_state, float delta) {
    if (!game_state.player.is_alive()) {
        _state = EnemyState::Idle;
        return;
    }

    float dist = distance_to_player(game_state);
    face_player(game_state);

    // Check if player left attack range
    if (_type == EnemyType::Melee && dist > _attack_range * 1.2f) {
        _state = EnemyState::Chasing;
        return;
    }
    if (_type == EnemyType::Ranged && dist > _detection_range) {
        _state = EnemyState::Idle;
        return;
    }

    // Attack if cooldown ready
    if (_attack_cooldown <= 0.0f) {
        if (_type == EnemyType::Melee) {
            attack_melee(game_state);
        } else {
            attack_ranged(game_state);
        }
        _attack_cooldown = 1.0f / _attack_rate;
    }
}

void Enemy::update_dead(GameState& game_state, float delta) {
    _death_timer += delta;
}

void Enemy::take_damage(float amount, GameState& game_state, glm::vec3 impact_pos) {
    if (_state == EnemyState::Dead) return;

    _health -= amount;

    // Spawn impact particles at hit location
    game_state.particle_system.spawn_impact(impact_pos, 6);

    if (_health <= 0.0f) {
        _health = 0.0f;
        _state = EnemyState::Dead;
        _death_timer = 0.0f;

        // Spawn explosion particles at enemy center
        glm::vec3 center = _position + glm::vec3(0.0f, 0.5f, 0.0f);
        game_state.particle_system.spawn_explosion(center, 25);
    }
}

float Enemy::distance_to_player(const GameState& game_state) const {
    return glm::length(game_state.player.position() - _position);
}

glm::vec3 Enemy::direction_to_player(const GameState& game_state) const {
    glm::vec3 diff = game_state.player.position() - _position;
    if (glm::length(diff) > 0.001f) {
        return glm::normalize(diff);
    }
    return glm::vec3(0.0f, 0.0f, 1.0f);
}

void Enemy::face_player(const GameState& game_state) {
    glm::vec3 dir = direction_to_player(game_state);
    _rotation_y = atan2(dir.x, dir.z);
}

void Enemy::move_toward_player(const GameState& game_state, float delta) {
    glm::vec3 dir = direction_to_player(game_state);
    _velocity = dir * _move_speed;
    _position += _velocity * delta;
    _rotation_y = atan2(dir.x, dir.z);
}

void Enemy::attack_melee(GameState& game_state) {
    // Direct damage to player
    game_state.player.take_damage(_damage);
}

void Enemy::attack_ranged(GameState& game_state) {
    // Spawn projectile via ProjectileManager
    glm::vec3 spawn_pos = _position + glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 direction = direction_to_player(game_state);
    game_state.projectile_manager.spawn_projectile(spawn_pos, direction, _damage, false);
}

}  // namespace main_game
