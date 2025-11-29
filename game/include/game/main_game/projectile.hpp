#pragma once

#include <glm/glm.hpp>

namespace main_game {

struct Projectile {
    glm::vec3 position{0.0f};
    glm::vec3 velocity{0.0f};
    float damage = 10.0f;
    float lifetime = 5.0f;
    float radius = 0.2f;
    bool is_player_projectile = true;  // true = damages enemies, false = damages player

    bool is_expired() const { return lifetime <= 0.0f; }
};

}  // namespace main_game
