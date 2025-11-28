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

    void take_damage(float amount);
    void use_stamina(float amount);
    void heal(float amount);

    glm::vec3 position() const { return _position; }
    float rotation_y() const { return _rotation_y; }

    float health() const { return _health; }
    float max_health() const { return _max_health; }
    float stamina() const { return _stamina; }
    float max_stamina() const { return _max_stamina; }
    bool is_alive() const { return _health > 0.0f; }

    const Weapon& main_weapon() const { return _main_weapon; }
    const Weapon& sub_weapon() const { return _sub_weapon; }

private:
    // Movement
    glm::vec3 _position;
    glm::vec3 _velocity;
    glm::vec3 _input_direction;
    float _rotation_y;
    float _move_speed;

    // Stats
    float _health = 100.0f;
    float _max_health = 100.0f;
    float _stamina = 100.0f;
    float _max_stamina = 100.0f;
    float _stamina_regen_rate = 10.0f;

    // Equipment
    Weapon _main_weapon;
    Weapon _sub_weapon;
};
}  // namespace main_game
