#pragma once

#include <game/main_game/weapon/weapon.hpp>

namespace main_game {

// Gun - ranged weapon that shoots projectiles
class Gun : public Weapon {
public:
    Gun();
    Gun(std::string name, int damage, float attack_speed, float range, float projectile_speed = 15.0f);

    void attack(GameState& game_state, glm::vec3 position, glm::vec3 facing) override;

    float projectile_speed() const { return _projectile_speed; }

private:
    float _projectile_speed;
};

}  // namespace main_game
