#include <game/main_game/weapon/gun.hpp>
#include <game/main_game/game_state.hpp>

namespace main_game {

Gun::Gun()
    : Weapon("Gun", WeaponType::Ranged, 15, 10.0f, 20.0f)
    , _projectile_speed(30.0f) {}

Gun::Gun(std::string name, int damage, float attack_speed, float range, float projectile_speed)
    : Weapon(std::move(name), WeaponType::Ranged, damage, attack_speed, range)
    , _projectile_speed(projectile_speed) {}

void Gun::attack(GameState& game_state, glm::vec3 position, glm::vec3 facing) {
    // Apply damage multiplier from player perks
    float effective_damage = static_cast<float>(_damage) * game_state.player.damage_multiplier();

    // Shoot projectile
    glm::vec3 spawn_pos = position + glm::vec3(0.0f, 1.0f, 0.0f);  // Slightly above ground
    game_state.projectile_manager.spawn_projectile(
        spawn_pos, facing, effective_damage, true, _projectile_speed);
}

}  // namespace main_game
