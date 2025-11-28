#include <game/main_game/game_state.hpp>
#include <engine/input_system.hpp>

namespace main_game {

void GameState::update(float delta) {
    player.process_input(*this);
    player.update(*this, delta);

    camera.process_input(*this);
    camera.update(*this, delta);
}

} // namespace main_game
