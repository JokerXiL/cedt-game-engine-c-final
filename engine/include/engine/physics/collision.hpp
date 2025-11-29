#pragma once

#include <engine/physics/collision_shape.hpp>

#include <glm/glm.hpp>

namespace engine::physics {

struct CollisionResult {
    bool collided = false;
    glm::vec3 normal{0.0f};       // Collision normal (direction to separate A from B)
    float penetration = 0.0f;     // Overlap depth
    glm::vec3 contact_point{0.0f};
};

// Sphere vs Sphere collision test
CollisionResult test_sphere_sphere(
    const SphereShape& a, glm::vec3 pos_a,
    const SphereShape& b, glm::vec3 pos_b);

// Sphere vs AABB collision test
CollisionResult test_sphere_aabb(
    const SphereShape& sphere, glm::vec3 sphere_pos,
    const AABBShape& aabb, glm::vec3 aabb_pos);

// AABB vs AABB collision test
CollisionResult test_aabb_aabb(
    const AABBShape& a, glm::vec3 pos_a,
    const AABBShape& b, glm::vec3 pos_b);

// Capsule vs Capsule collision test
CollisionResult test_capsule_capsule(
    const CapsuleShape& a, glm::vec3 pos_a,
    const CapsuleShape& b, glm::vec3 pos_b);

// Capsule vs AABB collision test
CollisionResult test_capsule_aabb(
    const CapsuleShape& cap, glm::vec3 cap_pos,
    const AABBShape& aabb, glm::vec3 aabb_pos);

// Capsule vs Sphere collision test
CollisionResult test_capsule_sphere(
    const CapsuleShape& cap, glm::vec3 cap_pos,
    const SphereShape& sphere, glm::vec3 sphere_pos);

// Generic collision test dispatcher (uses shape types to call appropriate test)
CollisionResult test_collision(
    const CollisionShape& a, glm::vec3 pos_a,
    const CollisionShape& b, glm::vec3 pos_b);

}  // namespace engine::physics
