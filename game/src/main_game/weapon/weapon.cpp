#include <game/main_game/weapon/weapon.hpp>

namespace main_game {

Weapon::Weapon(std::string name, WeaponType type, int damage, float attack_speed, float range)
    : _name(std::move(name))
    , _type(type)
    , _damage(damage)
    , _attack_speed(attack_speed)
    , _range(range) {}

}  // namespace main_game
