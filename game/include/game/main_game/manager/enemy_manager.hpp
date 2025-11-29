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

private:
    void remove_dead_enemies();

    std::vector<Enemy> _enemies;
};

}  // namespace main_game
