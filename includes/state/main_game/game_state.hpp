#pragma once

#include "state/main_game/manager/enemy_manager.hpp"
#include "state/main_game/manager/projectile_manager.hpp"
#include "state/main_game/player.hpp"

namespace main_game {
class GameState {
public:
    void update();

private:
    Player _player;
    EnemyManager _enemy_manager;
    ProjectileManager _projectile_manager;
};
}  // namespace main_game
