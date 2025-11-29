#include <game/main_game/system/particle_system.hpp>

#include <algorithm>
#include <random>

namespace main_game {

// Thread-local random engine for particle randomness
static thread_local std::mt19937 s_rng{std::random_device{}()};

float ParticleSystem::random_range(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(s_rng);
}

glm::vec3 ParticleSystem::random_direction() {
    // Generate random point on unit sphere using spherical coordinates
    float theta = random_range(0.0f, 2.0f * 3.14159265f);
    float phi = std::acos(random_range(-1.0f, 1.0f));

    float sin_phi = std::sin(phi);
    return glm::vec3(
        sin_phi * std::cos(theta),
        sin_phi * std::sin(theta),
        std::cos(phi)
    );
}

glm::vec3 ParticleSystem::random_fire_color() {
    // Interpolate between red (1, 0, 0) and orange (1, 0.5, 0)
    float t = random_range(0.0f, 1.0f);
    return glm::vec3(
        1.0f,                      // R: always 1
        t * 1.0f,                  // G: 0 to 0.5
        0.0f                       // B: always 0
    );
}

void ParticleSystem::update(float delta) {
    // Update all particles
    for (auto& particle : _particles) {
        // Apply gravity
        particle.velocity.y -= 9.8f * delta;

        // Move particle
        particle.position += particle.velocity * delta;

        // Age particle
        particle.lifetime -= delta;
    }

    // Remove dead particles
    _particles.erase(
        std::remove_if(_particles.begin(), _particles.end(),
            [](const Particle& p) { return p.lifetime <= 0.0f; }),
        _particles.end()
    );
}

void ParticleSystem::spawn_explosion(glm::vec3 position, int count) {
    _particles.reserve(_particles.size() + count);

    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position = position;

        // Random direction with upward bias
        glm::vec3 dir = random_direction();
        dir.y = std::abs(dir.y);  // Bias upward

        // Random speed
        float speed = random_range(3.0f, 8.0f);
        p.velocity = dir * speed;

        // Random red-to-orange color
        p.color = random_fire_color();

        // Random lifetime
        p.lifetime = random_range(0.5f, 1.0f);
        p.max_lifetime = p.lifetime;

        // Random size
        p.size = random_range(0.05f, 0.15f);

        _particles.push_back(p);
    }
}

void ParticleSystem::spawn_impact(glm::vec3 position, int count) {
    _particles.reserve(_particles.size() + count);

    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position = position;

        // Random direction (all directions for impact)
        glm::vec3 dir = random_direction();

        // Lower speed than explosion
        float speed = random_range(1.5f, 4.0f);
        p.velocity = dir * speed;

        // Random red-to-orange color
        p.color = random_fire_color();

        // Shorter lifetime than explosion
        p.lifetime = random_range(0.2f, 0.4f);
        p.max_lifetime = p.lifetime;

        // Smaller size
        p.size = random_range(0.03f, 0.08f);

        _particles.push_back(p);
    }
}

}  // namespace main_game
