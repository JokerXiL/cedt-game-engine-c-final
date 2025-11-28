#include <game/main_game/camera.hpp>
#include <game/main_game/game_state.hpp>

namespace main_game {

void Camera::update(GameState& game_state, float delta) {
    _target = game_state.player.position();
}

} // namespace main_game
