#include <game/main_game/weapon/sword.hpp>
#include <game/main_game/game_state.hpp>
#include <game/main_game/renderer.hpp>

#include <cmath>

namespace main_game {

Sword::Sword()
    : Weapon("Sword", WeaponType::Melee, 25, 1.5f, 5.0f)
    , _arc_angle(1.57f) {}  // ~90 degrees

Sword::Sword(std::string name, int damage, float attack_speed, float range, float arc_angle)
    : Weapon(std::move(name), WeaponType::Melee, damage, attack_speed, range)
    , _arc_angle(arc_angle) {}

void Sword::attack(GameState& game_state, glm::vec3 position, glm::vec3 facing) {
    // Slash attack - damage all enemies in arc
    float min_dot = std::cos(_arc_angle / 2.0f);  // Convert arc angle to dot product threshold

    for (auto& enemy : game_state.enemy_manager.enemies()) {
        if (!enemy.is_alive()) continue;

        glm::vec3 to_enemy = enemy.position() - position;
        float dist = glm::length(to_enemy);

        // Check if in weapon range
        if (dist > _range) continue;

        // Check if in front of player (within arc)
        glm::vec3 dir_to_enemy = glm::vec3(0.0f);
        if (dist > 0.001f) {
            dir_to_enemy = to_enemy / dist;
            float dot = glm::dot(facing, dir_to_enemy);
            if (dot < min_dot) continue;  // Not in attack arc
        }

        // Calculate impact position (enemy surface facing player)
        glm::vec3 enemy_center = enemy.position() + glm::vec3(0.0f, 0.5f, 0.0f);
        glm::vec3 impact_pos = enemy_center - dir_to_enemy * enemy.collision_radius();

        // Damage enemy
        enemy.take_damage(static_cast<float>(_damage), game_state, impact_pos);
    }

    // Trigger attack visual effect
    if (auto* renderer = game_state.renderer()) {
        float rotation_y = std::atan2(facing.x, facing.z);
        renderer->show_melee_attack(position, rotation_y, _range);
    }
}

}  // namespace main_game
