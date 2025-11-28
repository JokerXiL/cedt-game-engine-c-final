#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>
#include <optional>

#include <game/title_screen.hpp>
#include <engine/window_system.hpp>
#include <engine/input_system.hpp>

int main() {
    engine::WindowSystem::get_instance().init();
    engine::InputSystem::get_instance().init();

    std::optional<std::unique_ptr<State>> state =
        std::make_unique<title_screen::TitleScreen>();

    while (true) {
        if (state.has_value()) {
            state = state.value()->run();
        } else {
            break;
        }
    }
}