#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace main_game {

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 color;
    float lifetime;
    float max_lifetime;
    float size;
};

class ParticleSystem {
public:
    ParticleSystem() = default;

    void update(float delta);

    // Spawn an explosion of particles at a position (for death)
    void spawn_explosion(glm::vec3 position, int count = 20);

    // Spawn impact particles (smaller burst for damage hits)
    void spawn_impact(glm::vec3 position, int count = 8);

    // Access particles for rendering
    const std::vector<Particle>& particles() const { return _particles; }

private:
    std::vector<Particle> _particles;

    // Random float between min and max
    static float random_range(float min, float max);

    // Random unit vector
    static glm::vec3 random_direction();

    // Random color from red to orange
    static glm::vec3 random_fire_color();
};

}  // namespace main_game
