#pragma once

#include <engine/render/orbit_camera.hpp>
#include <game/main_game/manager/enemy_manager.hpp>
#include <game/main_game/manager/projectile_manager.hpp>
#include <game/main_game/map.hpp>
#include <game/main_game/player.hpp>

namespace main_game {
class GameState {
public:
    GameState();
    ~GameState() = default;

    void update(float delta);

    engine::OrbitCamera camera;
    Player player;
    Map map;
    EnemyManager enemy_manager;
    ProjectileManager projectile_manager;
};
}  // namespace main_game
