#pragma once

#include <glm/glm.hpp>

#include <game/main_game/weapon.hpp>

namespace main_game {
class GameState;
class Camera;

class Player {
public:
    Player();

    void process_input(const Camera& camera, float delta_time);
    void update(GameState&);

    // Getters
    glm::vec3 position() const { return _position; }
    float rotation_y() const { return _rotation_y; }

    // Setters
    void set_position(const glm::vec3& pos) { _position = pos; }

private:
    glm::vec3 _position;
    glm::vec3 _velocity;
    float _rotation_y;
    float _move_speed;

    Weapon _main_weapon;
    Weapon _sub_weapon;
};
}  // namespace main_game
