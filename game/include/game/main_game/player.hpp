#pragma once

#include <glm/glm.hpp>

#include <game/main_game/weapon.hpp>

namespace main_game {
class GameState;
class Camera;

class Player {
public:
    Player();

    void process_input(GameState&);
    void update(GameState&, float delta);

    // Getters
    glm::vec3 position() const { return _position; }
    float rotation_y() const { return _rotation_y; }
private:
    glm::vec3 _position;
    glm::vec3 _velocity;
    glm::vec3 _input_direction;
    float _rotation_y;
    float _move_speed;

    Weapon _main_weapon;
    Weapon _sub_weapon;
};
}  // namespace main_game
