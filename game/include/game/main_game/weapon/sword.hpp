#pragma once

#include <game/main_game/weapon/weapon.hpp>

namespace main_game {

// Sword - melee weapon that slashes in an arc
class Sword : public Weapon {
public:
    Sword();
    Sword(std::string name, int damage, float attack_speed, float range, float arc_angle = 1.57f);

    void attack(GameState& game_state, glm::vec3 position, glm::vec3 facing) override;

    float arc_angle() const { return _arc_angle; }

private:
    float _arc_angle;  // Attack arc in radians (default ~90 degrees)
};

}  // namespace main_game
