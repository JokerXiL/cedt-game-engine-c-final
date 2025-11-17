#pragma once

#include <glm/glm.hpp>

#include "state/main_game/weapon.hpp"

namespace main_game {
class GameState;

class Player {
public:
    void update(GameState&);

private:
    glm::vec3 _position;
    Weapon _main_weapon;
    Weapon _sub_weapon;
};
}  // namespace main_game