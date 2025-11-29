#pragma once

#include <game/main_game/projectile.hpp>

#include <glm/glm.hpp>

#include <vector>

namespace main_game {
class GameState;

class ProjectileManager {
public:
    ProjectileManager() = default;

    void update(GameState& game_state, float delta);

    void spawn_projectile(glm::vec3 position, glm::vec3 direction,
                          float damage, bool is_player_projectile,
                          float speed = 15.0f);

    void clear_all();

    const std::vector<Projectile>& projectiles() const { return _projectiles; }

private:
    void move_projectiles(float delta);
    void check_collisions(GameState& game_state);
    void remove_expired();

    std::vector<Projectile> _projectiles;

    static constexpr float PROJECTILE_SPEED = 15.0f;
};

}  // namespace main_game
