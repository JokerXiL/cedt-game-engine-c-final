#pragma once

#include <engine/physics/rigid_body.hpp>
#include <engine/physics/collision.hpp>

#include <glm/glm.hpp>

#include <vector>
#include <functional>

namespace engine::physics {

// Collision callback signature
using CollisionCallback = std::function<void(RigidBody* a, RigidBody* b, const CollisionResult& result)>;

struct RaycastHit {
    RigidBody* body = nullptr;
    float distance = 0.0f;
    glm::vec3 point{0.0f};
    glm::vec3 normal{0.0f};
};

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld() = default;

    // Main update (call once per frame)
    void update(float delta);

    // Gravity
    void set_gravity(glm::vec3 gravity);
    glm::vec3 gravity() const { return _gravity; }

    // Body management
    void add_body(RigidBody* body);
    void remove_body(RigidBody* body);
    void clear_bodies();

    // Queries
    std::vector<RigidBody*> query_sphere(glm::vec3 center, float radius);
    std::vector<RigidBody*> query_aabb(glm::vec3 min, glm::vec3 max);
    bool raycast(glm::vec3 origin, glm::vec3 direction, float max_dist, RaycastHit& hit);

    // Collision callback (called for each collision)
    void set_collision_callback(CollisionCallback callback);

private:
    void integrate(float delta);
    void detect_and_resolve_collisions();
    void resolve_collision(RigidBody* a, RigidBody* b, const CollisionResult& result);

    std::vector<RigidBody*> _bodies;
    glm::vec3 _gravity{0.0f, -9.81f, 0.0f};
    CollisionCallback _collision_callback;
};

}  // namespace engine::physics
