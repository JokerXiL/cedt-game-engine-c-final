#include <engine/physics/rigid_body.hpp>

namespace engine::physics {

void RigidBody::apply_force(glm::vec3 force) {
    if (is_static) return;
    _accumulated_force += force;
}

void RigidBody::apply_impulse(glm::vec3 impulse) {
    if (is_static) return;
    velocity += impulse * inv_mass;
}

void RigidBody::clear_forces() {
    _accumulated_force = glm::vec3(0.0f);
}

void RigidBody::integrate(float delta, glm::vec3 gravity) {
    if (is_static) return;

    // Apply gravity
    if (use_gravity) {
        velocity += gravity * delta;
    }

    // Apply accumulated forces (F = ma -> a = F/m -> a = F * inv_mass)
    velocity += _accumulated_force * inv_mass * delta;

    // Apply drag
    velocity *= (1.0f - drag * delta);

    // Update position
    position += velocity * delta;

    // Clear forces for next frame
    clear_forces();
}

glm::vec3 RigidBody::world_position() const {
    if (shape) {
        return position + shape->offset;
    }
    return position;
}

void RigidBody::set_sphere(float radius) {
    shape = std::make_unique<SphereShape>(radius);
    inv_mass = is_static ? 0.0f : 1.0f / mass;
}

void RigidBody::set_aabb(glm::vec3 half_extents) {
    shape = std::make_unique<AABBShape>(half_extents);
    inv_mass = is_static ? 0.0f : 1.0f / mass;
}

void RigidBody::set_capsule(float radius, float height) {
    shape = std::make_unique<CapsuleShape>(radius, height);
    inv_mass = is_static ? 0.0f : 1.0f / mass;
}

}  // namespace engine::physics
