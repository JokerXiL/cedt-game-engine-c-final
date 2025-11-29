#include <game/main_game/game_state.hpp>

namespace main_game {

void GameState::process_input() {
    // Update camera first to optimize for minimal perceived latency
    camera.process_input();

    player.process_input(*this);
}

void GameState::update(float delta) {
    player.update(*this, delta);

    // Update camera last to target it on updated player
    camera.update(*this);
}

}  // namespace main_game
