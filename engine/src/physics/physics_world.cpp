#include <engine/physics/physics_world.hpp>

#include <algorithm>
#include <cmath>

namespace engine::physics {

PhysicsWorld::PhysicsWorld() = default;

void PhysicsWorld::update(float delta) {
    // Step 1: Integrate velocities and positions
    integrate(delta);

    // Step 2: Detect and resolve collisions
    detect_and_resolve_collisions();
}

void PhysicsWorld::set_gravity(glm::vec3 gravity) {
    _gravity = gravity;
}

void PhysicsWorld::add_body(RigidBody* body) {
    if (body && std::find(_bodies.begin(), _bodies.end(), body) == _bodies.end()) {
        _bodies.push_back(body);
    }
}

void PhysicsWorld::remove_body(RigidBody* body) {
    auto it = std::find(_bodies.begin(), _bodies.end(), body);
    if (it != _bodies.end()) {
        _bodies.erase(it);
    }
}

void PhysicsWorld::clear_bodies() {
    _bodies.clear();
}

std::vector<RigidBody*> PhysicsWorld::query_sphere(glm::vec3 center, float radius) {
    std::vector<RigidBody*> results;

    SphereShape query_sphere(radius);

    for (RigidBody* body : _bodies) {
        if (!body->shape) continue;

        auto result = test_collision(query_sphere, center, *body->shape, body->position);
        if (result.collided) {
            results.push_back(body);
        }
    }

    return results;
}

std::vector<RigidBody*> PhysicsWorld::query_aabb(glm::vec3 min, glm::vec3 max) {
    std::vector<RigidBody*> results;

    glm::vec3 center = (min + max) * 0.5f;
    glm::vec3 half_extents = (max - min) * 0.5f;
    AABBShape query_aabb(half_extents);

    for (RigidBody* body : _bodies) {
        if (!body->shape) continue;

        auto result = test_collision(query_aabb, center, *body->shape, body->position);
        if (result.collided) {
            results.push_back(body);
        }
    }

    return results;
}

bool PhysicsWorld::raycast(glm::vec3 origin, glm::vec3 direction, float max_dist, RaycastHit& hit) {
    direction = glm::normalize(direction);
    hit = RaycastHit{};

    float closest_dist = max_dist;
    bool found = false;

    for (RigidBody* body : _bodies) {
        if (!body->shape) continue;

        // Simple ray-shape intersection based on shape type
        glm::vec3 to_body = body->world_position() - origin;
        float t = glm::dot(to_body, direction);

        if (t < 0 || t > closest_dist) continue;

        glm::vec3 closest_point = origin + direction * t;
        float dist_to_center = glm::length(body->world_position() - closest_point);

        // Get effective radius based on shape
        float effective_radius = 0.0f;
        if (body->shape->type == ShapeType::Sphere) {
            effective_radius = static_cast<SphereShape*>(body->shape.get())->radius;
        } else if (body->shape->type == ShapeType::AABB) {
            // Approximate AABB as sphere for raycast
            auto* aabb = static_cast<AABBShape*>(body->shape.get());
            effective_radius = glm::length(aabb->half_extents);
        } else if (body->shape->type == ShapeType::Capsule) {
            effective_radius = static_cast<CapsuleShape*>(body->shape.get())->radius;
        }

        if (dist_to_center <= effective_radius) {
            // Calculate actual hit point
            float half_chord = std::sqrt(effective_radius * effective_radius - dist_to_center * dist_to_center);
            float hit_t = t - half_chord;

            if (hit_t > 0 && hit_t < closest_dist) {
                closest_dist = hit_t;
                hit.body = body;
                hit.distance = hit_t;
                hit.point = origin + direction * hit_t;
                hit.normal = glm::normalize(hit.point - body->world_position());
                found = true;
            }
        }
    }

    return found;
}

void PhysicsWorld::set_collision_callback(CollisionCallback callback) {
    _collision_callback = std::move(callback);
}

void PhysicsWorld::integrate(float delta) {
    for (RigidBody* body : _bodies) {
        body->integrate(delta, _gravity);
    }
}

void PhysicsWorld::detect_and_resolve_collisions() {
    // Simple O(n^2) broad phase (can be optimized with spatial partitioning later)
    for (size_t i = 0; i < _bodies.size(); ++i) {
        for (size_t j = i + 1; j < _bodies.size(); ++j) {
            RigidBody* a = _bodies[i];
            RigidBody* b = _bodies[j];

            // Skip if both are static
            if (a->is_static && b->is_static) continue;

            // Skip if no shapes
            if (!a->shape || !b->shape) continue;

            auto result = test_collision(*a->shape, a->position, *b->shape, b->position);

            if (result.collided) {
                // Call collision callback
                if (_collision_callback) {
                    _collision_callback(a, b, result);
                }

                // Skip resolution if either is a trigger
                if (a->is_trigger || b->is_trigger) continue;

                resolve_collision(a, b, result);
            }
        }
    }
}

void PhysicsWorld::resolve_collision(RigidBody* a, RigidBody* b, const CollisionResult& result) {
    // Calculate inverse masses
    float inv_mass_a = a->is_static ? 0.0f : a->inv_mass;
    float inv_mass_b = b->is_static ? 0.0f : b->inv_mass;
    float total_inv_mass = inv_mass_a + inv_mass_b;

    if (total_inv_mass <= 0.0f) return;  // Both static

    // Position correction (separate the bodies)
    const float correction_percent = 0.8f;  // Penetration percentage to correct
    const float slop = 0.01f;  // Small allowance to prevent jitter

    float correction_magnitude = std::max(result.penetration - slop, 0.0f) * correction_percent;
    glm::vec3 correction = result.normal * correction_magnitude / total_inv_mass;

    if (!a->is_static) {
        a->position -= correction * inv_mass_a;
    }
    if (!b->is_static) {
        b->position += correction * inv_mass_b;
    }

    // Velocity resolution (impulse-based)
    glm::vec3 relative_velocity = b->velocity - a->velocity;
    float velocity_along_normal = glm::dot(relative_velocity, result.normal);

    // Don't resolve if velocities are separating
    if (velocity_along_normal > 0) return;

    // Calculate restitution (use minimum)
    float restitution = std::min(a->restitution, b->restitution);

    // Calculate impulse scalar
    float j = -(1.0f + restitution) * velocity_along_normal;
    j /= total_inv_mass;

    // Apply impulse
    glm::vec3 impulse = j * result.normal;

    if (!a->is_static) {
        a->velocity -= impulse * inv_mass_a;
    }
    if (!b->is_static) {
        b->velocity += impulse * inv_mass_b;
    }
}

}  // namespace engine::physics
