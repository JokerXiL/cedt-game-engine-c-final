#include <game/main_game/game_state.hpp>
#include <engine/input/input_system.hpp>

namespace main_game {

void GameState::update(float delta) {
    player.process_input(*this);
    player.update(*this, delta);

    // Update camera orbit
    engine::input::InputSystem& input = engine::input::InputSystem::get_instance();
    float dx, dy;
    input.get_mouse_delta(dx, dy);
    camera.orbit_rotate(dx * 0.1f, dy * 0.1f);

    // Follow player
    camera.set_orbit_target(player.position());
}

} // namespace main_game
