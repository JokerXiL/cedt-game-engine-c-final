#pragma once

#include <glm/glm.hpp>

#include <string>

namespace main_game {

class GameState;

enum class WeaponType {
    Melee,
    Ranged
};

// Base weapon class with virtual attack method
class Weapon {
public:
    Weapon(std::string name, WeaponType type, int damage, float attack_speed, float range);
    virtual ~Weapon() = default;

    // Perform the weapon's attack
    virtual void attack(GameState& game_state, glm::vec3 position, glm::vec3 facing) = 0;

    // Accessors
    const std::string& name() const { return _name; }
    WeaponType type() const { return _type; }
    int damage() const { return _damage; }
    float attack_speed() const { return _attack_speed; }
    float range() const { return _range; }
    float cooldown() const { return 1.0f / _attack_speed; }

protected:
    std::string _name;
    WeaponType _type;
    int _damage;
    float _attack_speed;
    float _range;
};

}  // namespace main_game
