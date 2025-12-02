#pragma once

#include <game/main_game/enemy.hpp>

#include <glm/glm.hpp>

#include <vector>

namespace main_game {
class GameState;

class EnemyManager {
public:
    EnemyManager() = default;

    void update(GameState& game_state, float delta);

    // Spawning
    void spawn_enemy(EnemyType type, glm::vec3 position);
    void clear_all();

    // Accessors
    const std::vector<Enemy>& enemies() const { return _enemies; }
    std::vector<Enemy>& enemies() { return _enemies; }
    size_t alive_count() const;
    size_t total_count() const { return _enemies.size(); }

    // Configuration
    static constexpr float DESPAWN_DISTANCE = 50.0f;  // Distance from player to despawn

private:
    void remove_dead_enemies();
    void despawn_far_enemies(const glm::vec3& player_pos);

    std::vector<Enemy> _enemies;
};

}  // namespace main_game
