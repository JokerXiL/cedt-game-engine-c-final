#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>

#include "state/title_screen.hpp"
#include "window_system.hpp"

int main() {
    WindowSystem::get_instance().init();

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