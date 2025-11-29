#include <game/main_game/manager/enemy_manager.hpp>
#include <game/main_game/game_state.hpp>

#include <algorithm>

namespace main_game {

void EnemyManager::update(GameState& game_state, float delta) {
    // Update all enemies
    for (auto& enemy : _enemies) {
        enemy.update(game_state, delta);
    }

    // Remove dead enemies that have finished their death animation
    remove_dead_enemies();
}

void EnemyManager::spawn_enemy(EnemyType type, glm::vec3 position) {
    _enemies.emplace_back(type, position);
}

void EnemyManager::clear_all() {
    _enemies.clear();
}

size_t EnemyManager::alive_count() const {
    return std::count_if(_enemies.begin(), _enemies.end(),
        [](const Enemy& e) { return e.is_alive(); });
}

void EnemyManager::remove_dead_enemies() {
    _enemies.erase(
        std::remove_if(_enemies.begin(), _enemies.end(),
            [](const Enemy& e) { return e.should_remove(); }),
        _enemies.end()
    );
}

}  // namespace main_game
