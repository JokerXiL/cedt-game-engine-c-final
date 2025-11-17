#include "state/title_screen.hpp"

#include <memory>
#include <optional>

#include "state/main_game.hpp"

namespace title_screen {
std::unique_ptr<State> TitleScreen::run() {
    return std::make_unique<main_game::MainGame>();
}
}  // namespace title_screen
