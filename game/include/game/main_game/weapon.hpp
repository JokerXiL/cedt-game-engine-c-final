#pragma once

#include <string>

namespace main_game {

struct Weapon {
    std::string name = "Fists";
    int damage = 5;
    float attack_speed = 1.0f;
    float range = 1.5f;
};

}  // namespace main_game