#include <iostream>
#include <memory>
#include <optional>

#include <game/title_screen.hpp>
#include <engine/window/window_system.hpp>
#include <engine/input/input_system.hpp>

int main() {
    engine::window::WindowSystem::get_instance().init();
    engine::input::InputSystem::get_instance().init();

    std::unique_ptr<State> state =
        std::make_unique<title_screen::TitleScreen>();

    while (true) {
        if (state != nullptr) {
            auto next_state = state->run();
            state = std::move(next_state);
        } else {
            break;
        }
    }

    return 0;
}