#pragma once

#include <game/main_game/game_camera.hpp>
#include <game/main_game/manager/enemy_manager.hpp>
#include <game/main_game/manager/projectile_manager.hpp>
#include <game/main_game/map.hpp>
#include <game/main_game/player.hpp>

namespace main_game {
class GameState {
public:
    GameState() = default;
    ~GameState() = default;

    void process_input();
    void update(float delta);

    GameCamera camera;
    Player player;
    Map map;
    EnemyManager enemy_manager;
    ProjectileManager projectile_manager;
};
}  // namespace main_game
