#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "scene.hpp"
#include "input_system.hpp"

int main() {
    // todo init window
    // todo init input_system;

    Transition t = Transition {
        MAIN_GAME
    };

    while (true) {
        switch (t.kind) {   
            MAIN_GAME:
                t = main_game();
        }
    }
}