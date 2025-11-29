#pragma once

#include <glm/glm.hpp>

namespace engine::physics {

enum class ShapeType {
    Sphere,
    AABB,
    Capsule
};

struct CollisionShape {
    ShapeType type;
    glm::vec3 offset{0.0f};  // Local offset from entity position

    explicit CollisionShape(ShapeType t) : type(t) {}
    virtual ~CollisionShape() = default;
};

struct SphereShape : CollisionShape {
    float radius = 0.5f;

    SphereShape() : CollisionShape(ShapeType::Sphere) {}
    explicit SphereShape(float r) : CollisionShape(ShapeType::Sphere), radius(r) {}
    SphereShape(float r, glm::vec3 off) : CollisionShape(ShapeType::Sphere), radius(r) {
        offset = off;
    }
};

struct AABBShape : CollisionShape {
    glm::vec3 half_extents{0.5f};  // Half-size in each axis

    AABBShape() : CollisionShape(ShapeType::AABB) {}
    explicit AABBShape(glm::vec3 half) : CollisionShape(ShapeType::AABB), half_extents(half) {}
    AABBShape(glm::vec3 half, glm::vec3 off) : CollisionShape(ShapeType::AABB), half_extents(half) {
        offset = off;
    }
};

struct CapsuleShape : CollisionShape {
    float radius = 0.5f;
    float height = 2.0f;  // Total height (including hemispherical caps)

    CapsuleShape() : CollisionShape(ShapeType::Capsule) {}
    CapsuleShape(float r, float h) : CollisionShape(ShapeType::Capsule), radius(r), height(h) {}
    CapsuleShape(float r, float h, glm::vec3 off) : CollisionShape(ShapeType::Capsule), radius(r), height(h) {
        offset = off;
    }

    // Height of the cylindrical portion (excluding caps)
    float cylinder_height() const { return height - 2.0f * radius; }

    // Half-height of the cylindrical portion
    float half_cylinder_height() const { return cylinder_height() * 0.5f; }
};

}  // namespace engine::physics
