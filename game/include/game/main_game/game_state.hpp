#pragma once

#include <game/main_game/game_camera.hpp>
#include <game/main_game/manager/enemy_manager.hpp>
#include <game/main_game/manager/projectile_manager.hpp>
#include <game/main_game/system/particle_system.hpp>
#include <game/main_game/system/wave_system.hpp>
#include <game/main_game/map.hpp>
#include <game/main_game/player.hpp>

namespace main_game {

class Renderer;

class GameState {
public:
    GameState() = default;
    ~GameState() = default;

    void process_input();
    void update(float delta);

    // Start the wave system
    void start_game();

    // Set renderer for visual effects triggered by game events
    void set_renderer(Renderer* renderer) { _renderer = renderer; }
    Renderer* renderer() const { return _renderer; }

    GameCamera camera;
    Player player;
    Map map;
    EnemyManager enemy_manager;
    ProjectileManager projectile_manager;
    ParticleSystem particle_system;
    WaveSystem wave_system;

private:
    Renderer* _renderer = nullptr;
};
}  // namespace main_game
