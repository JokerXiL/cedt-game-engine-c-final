#pragma once

#include "state/main_game/manager/enemy_manager.hpp"
#include "state/main_game/manager/projectile_manager.hpp"
#include "state/main_game/player.hpp"
#include "state/main_game/map.hpp"

namespace main_game {
class GameState {
public:
    void update();

    // Getters
    Player& player() { return _player; }
    const Player& player() const { return _player; }
    Map& map() { return _map; }
    const Map& map() const { return _map; }

private:
    Player _player;
    Map _map;
    EnemyManager _enemy_manager;
    ProjectileManager _projectile_manager;
};
}  // namespace main_game
