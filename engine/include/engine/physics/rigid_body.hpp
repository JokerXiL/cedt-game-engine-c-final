#pragma once

#include <engine/physics/collision_shape.hpp>

#include <glm/glm.hpp>

#include <memory>

namespace engine::physics {

class RigidBody {
public:
    RigidBody() = default;
    ~RigidBody() = default;

    // Disable copy, allow move
    RigidBody(const RigidBody&) = delete;
    RigidBody& operator=(const RigidBody&) = delete;
    RigidBody(RigidBody&&) = default;
    RigidBody& operator=(RigidBody&&) = default;

    // Apply a force (accumulated over frame, affected by mass)
    void apply_force(glm::vec3 force);

    // Apply an impulse (immediate velocity change)
    void apply_impulse(glm::vec3 impulse);

    // Clear accumulated forces (called after integration)
    void clear_forces();

    // Integrate motion (called by PhysicsWorld)
    void integrate(float delta, glm::vec3 gravity);

    // Position with shape offset applied
    glm::vec3 world_position() const;

    // Setters for shape
    void set_sphere(float radius);
    void set_aabb(glm::vec3 half_extents);
    void set_capsule(float radius, float height);

    // Transform
    glm::vec3 position{0.0f};
    glm::vec3 velocity{0.0f};

    // Physics properties
    float mass = 1.0f;
    float inv_mass = 1.0f;        // Inverse mass (0 for static objects)
    float drag = 0.1f;            // Linear drag coefficient
    float restitution = 0.0f;     // Bounciness (0 = no bounce, 1 = perfect bounce)

    // Flags
    bool use_gravity = true;
    bool is_static = false;       // Static bodies don't move
    bool is_trigger = false;      // Trigger bodies detect overlap but don't resolve

    // Collision shape
    std::unique_ptr<CollisionShape> shape;

    // User data pointer for game objects
    void* user_data = nullptr;

private:
    glm::vec3 _accumulated_force{0.0f};
};

}  // namespace engine::physics
