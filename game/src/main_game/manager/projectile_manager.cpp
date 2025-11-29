#include <game/main_game/manager/projectile_manager.hpp>
#include <game/main_game/game_state.hpp>

#include <algorithm>

namespace main_game {

void ProjectileManager::update(GameState& game_state, float delta) {
    move_projectiles(delta);
    check_collisions(game_state);
    remove_expired();
}

void ProjectileManager::spawn_projectile(glm::vec3 position, glm::vec3 direction,
                                         float damage, bool is_player_projectile,
                                         float speed) {
    Projectile proj;
    proj.position = position;
    proj.velocity = glm::normalize(direction) * speed;
    proj.damage = damage;
    proj.is_player_projectile = is_player_projectile;
    proj.lifetime = 5.0f;

    _projectiles.push_back(proj);
}

void ProjectileManager::clear_all() {
    _projectiles.clear();
}

void ProjectileManager::move_projectiles(float delta) {
    for (auto& proj : _projectiles) {
        proj.position += proj.velocity * delta;
        proj.lifetime -= delta;
    }
}

void ProjectileManager::check_collisions(GameState& game_state) {
    const float player_collision_radius = 0.5f;

    for (auto& proj : _projectiles) {
        if (proj.is_expired()) continue;

        if (proj.is_player_projectile) {
            // Check collision with enemies
            for (auto& enemy : game_state.enemy_manager.enemies()) {
                if (!enemy.is_alive()) continue;

                // Enemy center is at Y=0.5 (cube sits on ground)
                glm::vec3 enemy_center = enemy.position() + glm::vec3(0.0f, 0.5f, 0.0f);
                float dist = glm::length(enemy_center - proj.position);
                if (dist < proj.radius + enemy.collision_radius()) {
                    enemy.take_damage(proj.damage, game_state, proj.position);
                    // Apply lifesteal if player has it
                    game_state.player.on_damage_dealt(proj.damage);
                    proj.lifetime = 0.0f;  // Mark for removal
                    break;
                }
            }
        } else {
            // Check collision with player
            if (!game_state.player.is_alive()) continue;

            // Player center is at Y=1.0 (approximate torso height)
            glm::vec3 player_center = game_state.player.position() + glm::vec3(0.0f, 1.0f, 0.0f);
            float dist = glm::length(player_center - proj.position);
            if (dist < proj.radius + player_collision_radius) {
                game_state.player.take_damage(proj.damage);
                proj.lifetime = 0.0f;
            }
        }
    }
}

void ProjectileManager::remove_expired() {
    _projectiles.erase(
        std::remove_if(_projectiles.begin(), _projectiles.end(),
            [](const Projectile& p) { return p.is_expired(); }),
        _projectiles.end()
    );
}

}  // namespace main_game
