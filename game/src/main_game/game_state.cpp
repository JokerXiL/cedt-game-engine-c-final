#include <game/main_game/game_state.hpp>

namespace main_game {

void GameState::process_input() {
    // Update camera first to optimize for minimal perceived latency
    camera.process_input();

    player.process_input(*this);
}

void GameState::update(float delta) {
    player.update(*this, delta);

    // Update wave system (handles enemy spawning)
    wave_system.update(*this, delta);

    // Update enemies
    enemy_manager.update(*this, delta);

    // Update projectiles
    projectile_manager.update(*this, delta);

    // Update particles
    particle_system.update(delta);

    // Update camera last to target it on updated player
    camera.update(*this, delta);
}

void GameState::start_game() {
    wave_system.start_game();
}

}  // namespace main_game
