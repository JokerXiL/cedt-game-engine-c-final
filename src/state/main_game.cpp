#include "state/main_game.hpp"

#include <GLFW/glfw3.h>

#include <memory>
#include <optional>

#include "state/game_summary.hpp"
#include "state/main_game/game_state.hpp"

namespace main_game {
std::unique_ptr<State> MainGame::run() {
    GameState game_state;
    while (true) {
        // TODO
    }
    return std::make_unique<game_summary::GameSummary>();
}
}  // namespace main_game
