#include <engine/physics/collision.hpp>

#include <glm/gtc/constants.hpp>

#include <algorithm>
#include <cmath>

namespace engine::physics {

// Helper: Clamp a point to AABB bounds
static glm::vec3 clamp_to_aabb(glm::vec3 point, glm::vec3 aabb_center, glm::vec3 half_extents) {
    glm::vec3 min = aabb_center - half_extents;
    glm::vec3 max = aabb_center + half_extents;
    return glm::clamp(point, min, max);
}

// Helper: Find closest point on line segment to a point
static glm::vec3 closest_point_on_segment(glm::vec3 point, glm::vec3 seg_start, glm::vec3 seg_end) {
    glm::vec3 segment = seg_end - seg_start;
    float segment_length_sq = glm::dot(segment, segment);

    if (segment_length_sq < 1e-8f) {
        return seg_start;  // Degenerate segment
    }

    float t = glm::dot(point - seg_start, segment) / segment_length_sq;
    t = glm::clamp(t, 0.0f, 1.0f);
    return seg_start + t * segment;
}

// Helper: Closest points between two line segments
static void closest_points_segments(
    glm::vec3 a1, glm::vec3 a2,
    glm::vec3 b1, glm::vec3 b2,
    glm::vec3& closest_a, glm::vec3& closest_b)
{
    glm::vec3 d1 = a2 - a1;  // Direction of segment A
    glm::vec3 d2 = b2 - b1;  // Direction of segment B
    glm::vec3 r = a1 - b1;

    float a = glm::dot(d1, d1);  // Squared length of segment A
    float e = glm::dot(d2, d2);  // Squared length of segment B
    float f = glm::dot(d2, r);

    float s, t;

    // Check for degenerate cases
    if (a < 1e-8f && e < 1e-8f) {
        // Both segments degenerate into points
        closest_a = a1;
        closest_b = b1;
        return;
    }

    if (a < 1e-8f) {
        // First segment degenerates into a point
        s = 0.0f;
        t = glm::clamp(f / e, 0.0f, 1.0f);
    } else {
        float c = glm::dot(d1, r);
        if (e < 1e-8f) {
            // Second segment degenerates into a point
            t = 0.0f;
            s = glm::clamp(-c / a, 0.0f, 1.0f);
        } else {
            // General case
            float b = glm::dot(d1, d2);
            float denom = a * e - b * b;

            if (denom != 0.0f) {
                s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            } else {
                s = 0.0f;
            }

            t = (b * s + f) / e;

            if (t < 0.0f) {
                t = 0.0f;
                s = glm::clamp(-c / a, 0.0f, 1.0f);
            } else if (t > 1.0f) {
                t = 1.0f;
                s = glm::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    closest_a = a1 + s * d1;
    closest_b = b1 + t * d2;
}

CollisionResult test_sphere_sphere(
    const SphereShape& a, glm::vec3 pos_a,
    const SphereShape& b, glm::vec3 pos_b)
{
    CollisionResult result;

    glm::vec3 center_a = pos_a + a.offset;
    glm::vec3 center_b = pos_b + b.offset;

    glm::vec3 delta = center_b - center_a;
    float distance_sq = glm::dot(delta, delta);
    float radius_sum = a.radius + b.radius;

    if (distance_sq >= radius_sum * radius_sum) {
        return result;  // No collision
    }

    float distance = std::sqrt(distance_sq);
    result.collided = true;

    if (distance > 1e-6f) {
        result.normal = delta / distance;
        result.penetration = radius_sum - distance;
        result.contact_point = center_a + result.normal * a.radius;
    } else {
        // Spheres at same position, pick arbitrary normal
        result.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        result.penetration = radius_sum;
        result.contact_point = center_a;
    }

    return result;
}

CollisionResult test_sphere_aabb(
    const SphereShape& sphere, glm::vec3 sphere_pos,
    const AABBShape& aabb, glm::vec3 aabb_pos)
{
    CollisionResult result;

    glm::vec3 sphere_center = sphere_pos + sphere.offset;
    glm::vec3 aabb_center = aabb_pos + aabb.offset;

    // Find closest point on AABB to sphere center
    glm::vec3 closest = clamp_to_aabb(sphere_center, aabb_center, aabb.half_extents);

    glm::vec3 delta = sphere_center - closest;
    float distance_sq = glm::dot(delta, delta);

    if (distance_sq >= sphere.radius * sphere.radius) {
        return result;  // No collision
    }

    result.collided = true;
    result.contact_point = closest;

    float distance = std::sqrt(distance_sq);
    if (distance > 1e-6f) {
        result.normal = delta / distance;
        result.penetration = sphere.radius - distance;
    } else {
        // Sphere center is inside AABB, find minimum separation axis
        glm::vec3 to_center = sphere_center - aabb_center;
        glm::vec3 overlap;
        overlap.x = aabb.half_extents.x - std::abs(to_center.x);
        overlap.y = aabb.half_extents.y - std::abs(to_center.y);
        overlap.z = aabb.half_extents.z - std::abs(to_center.z);

        // Find axis with minimum penetration
        if (overlap.x <= overlap.y && overlap.x <= overlap.z) {
            result.normal = glm::vec3(to_center.x >= 0 ? 1.0f : -1.0f, 0.0f, 0.0f);
            result.penetration = overlap.x + sphere.radius;
        } else if (overlap.y <= overlap.z) {
            result.normal = glm::vec3(0.0f, to_center.y >= 0 ? 1.0f : -1.0f, 0.0f);
            result.penetration = overlap.y + sphere.radius;
        } else {
            result.normal = glm::vec3(0.0f, 0.0f, to_center.z >= 0 ? 1.0f : -1.0f);
            result.penetration = overlap.z + sphere.radius;
        }
    }

    return result;
}

CollisionResult test_aabb_aabb(
    const AABBShape& a, glm::vec3 pos_a,
    const AABBShape& b, glm::vec3 pos_b)
{
    CollisionResult result;

    glm::vec3 center_a = pos_a + a.offset;
    glm::vec3 center_b = pos_b + b.offset;

    glm::vec3 delta = center_b - center_a;
    glm::vec3 total_extents = a.half_extents + b.half_extents;

    // Check overlap on each axis
    glm::vec3 overlap;
    overlap.x = total_extents.x - std::abs(delta.x);
    overlap.y = total_extents.y - std::abs(delta.y);
    overlap.z = total_extents.z - std::abs(delta.z);

    if (overlap.x <= 0 || overlap.y <= 0 || overlap.z <= 0) {
        return result;  // No collision
    }

    result.collided = true;

    // Find axis with minimum penetration (separation axis theorem)
    if (overlap.x <= overlap.y && overlap.x <= overlap.z) {
        result.normal = glm::vec3(delta.x >= 0 ? 1.0f : -1.0f, 0.0f, 0.0f);
        result.penetration = overlap.x;
    } else if (overlap.y <= overlap.z) {
        result.normal = glm::vec3(0.0f, delta.y >= 0 ? 1.0f : -1.0f, 0.0f);
        result.penetration = overlap.y;
    } else {
        result.normal = glm::vec3(0.0f, 0.0f, delta.z >= 0 ? 1.0f : -1.0f);
        result.penetration = overlap.z;
    }

    // Contact point at center of overlap region
    glm::vec3 contact_min = glm::max(center_a - a.half_extents, center_b - b.half_extents);
    glm::vec3 contact_max = glm::min(center_a + a.half_extents, center_b + b.half_extents);
    result.contact_point = (contact_min + contact_max) * 0.5f;

    return result;
}

CollisionResult test_capsule_capsule(
    const CapsuleShape& a, glm::vec3 pos_a,
    const CapsuleShape& b, glm::vec3 pos_b)
{
    CollisionResult result;

    glm::vec3 center_a = pos_a + a.offset;
    glm::vec3 center_b = pos_b + b.offset;

    // Capsule line segments (vertical, along Y axis)
    float half_height_a = a.half_cylinder_height();
    float half_height_b = b.half_cylinder_height();

    glm::vec3 a1 = center_a + glm::vec3(0.0f, -half_height_a, 0.0f);
    glm::vec3 a2 = center_a + glm::vec3(0.0f, half_height_a, 0.0f);
    glm::vec3 b1 = center_b + glm::vec3(0.0f, -half_height_b, 0.0f);
    glm::vec3 b2 = center_b + glm::vec3(0.0f, half_height_b, 0.0f);

    // Find closest points between the two line segments
    glm::vec3 closest_a, closest_b;
    closest_points_segments(a1, a2, b1, b2, closest_a, closest_b);

    // Now treat as sphere-sphere between closest points
    glm::vec3 delta = closest_b - closest_a;
    float distance_sq = glm::dot(delta, delta);
    float radius_sum = a.radius + b.radius;

    if (distance_sq >= radius_sum * radius_sum) {
        return result;  // No collision
    }

    float distance = std::sqrt(distance_sq);
    result.collided = true;

    if (distance > 1e-6f) {
        result.normal = delta / distance;
        result.penetration = radius_sum - distance;
        result.contact_point = closest_a + result.normal * a.radius;
    } else {
        result.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        result.penetration = radius_sum;
        result.contact_point = closest_a;
    }

    return result;
}

CollisionResult test_capsule_aabb(
    const CapsuleShape& cap, glm::vec3 cap_pos,
    const AABBShape& aabb, glm::vec3 aabb_pos)
{
    CollisionResult result;

    glm::vec3 cap_center = cap_pos + cap.offset;
    glm::vec3 aabb_center = aabb_pos + aabb.offset;

    // Capsule line segment (vertical)
    float half_height = cap.half_cylinder_height();
    glm::vec3 cap_bottom = cap_center + glm::vec3(0.0f, -half_height, 0.0f);
    glm::vec3 cap_top = cap_center + glm::vec3(0.0f, half_height, 0.0f);

    // Find closest point on capsule segment to AABB
    // We need to find the point on the segment that minimizes distance to AABB

    // Sample several points on the segment and find closest to AABB
    float min_dist_sq = std::numeric_limits<float>::max();
    glm::vec3 best_cap_point = cap_center;
    glm::vec3 best_aabb_point = aabb_center;

    const int samples = 5;
    for (int i = 0; i <= samples; ++i) {
        float t = static_cast<float>(i) / samples;
        glm::vec3 cap_point = cap_bottom + t * (cap_top - cap_bottom);
        glm::vec3 aabb_point = clamp_to_aabb(cap_point, aabb_center, aabb.half_extents);

        glm::vec3 delta = aabb_point - cap_point;
        float dist_sq = glm::dot(delta, delta);

        if (dist_sq < min_dist_sq) {
            min_dist_sq = dist_sq;
            best_cap_point = cap_point;
            best_aabb_point = aabb_point;
        }
    }

    // Now we have approximate closest point, test sphere-point collision
    glm::vec3 delta = best_aabb_point - best_cap_point;
    float distance_sq = glm::dot(delta, delta);

    if (distance_sq >= cap.radius * cap.radius) {
        return result;  // No collision
    }

    result.collided = true;
    result.contact_point = best_aabb_point;

    float distance = std::sqrt(distance_sq);
    if (distance > 1e-6f) {
        result.normal = -delta / distance;  // Normal points from AABB to capsule
        result.penetration = cap.radius - distance;
    } else {
        // Capsule center is inside AABB
        glm::vec3 to_center = best_cap_point - aabb_center;
        glm::vec3 overlap;
        overlap.x = aabb.half_extents.x - std::abs(to_center.x);
        overlap.y = aabb.half_extents.y - std::abs(to_center.y);
        overlap.z = aabb.half_extents.z - std::abs(to_center.z);

        if (overlap.x <= overlap.y && overlap.x <= overlap.z) {
            result.normal = glm::vec3(to_center.x >= 0 ? 1.0f : -1.0f, 0.0f, 0.0f);
            result.penetration = overlap.x + cap.radius;
        } else if (overlap.y <= overlap.z) {
            result.normal = glm::vec3(0.0f, to_center.y >= 0 ? 1.0f : -1.0f, 0.0f);
            result.penetration = overlap.y + cap.radius;
        } else {
            result.normal = glm::vec3(0.0f, 0.0f, to_center.z >= 0 ? 1.0f : -1.0f);
            result.penetration = overlap.z + cap.radius;
        }
    }

    return result;
}

CollisionResult test_capsule_sphere(
    const CapsuleShape& cap, glm::vec3 cap_pos,
    const SphereShape& sphere, glm::vec3 sphere_pos)
{
    CollisionResult result;

    glm::vec3 cap_center = cap_pos + cap.offset;
    glm::vec3 sphere_center = sphere_pos + sphere.offset;

    // Capsule line segment (vertical)
    float half_height = cap.half_cylinder_height();
    glm::vec3 cap_bottom = cap_center + glm::vec3(0.0f, -half_height, 0.0f);
    glm::vec3 cap_top = cap_center + glm::vec3(0.0f, half_height, 0.0f);

    // Find closest point on capsule segment to sphere center
    glm::vec3 closest_cap = closest_point_on_segment(sphere_center, cap_bottom, cap_top);

    // Now treat as sphere-sphere
    glm::vec3 delta = sphere_center - closest_cap;
    float distance_sq = glm::dot(delta, delta);
    float radius_sum = cap.radius + sphere.radius;

    if (distance_sq >= radius_sum * radius_sum) {
        return result;  // No collision
    }

    float distance = std::sqrt(distance_sq);
    result.collided = true;

    if (distance > 1e-6f) {
        result.normal = delta / distance;  // Normal points from capsule to sphere
        result.penetration = radius_sum - distance;
        result.contact_point = closest_cap + result.normal * cap.radius;
    } else {
        result.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        result.penetration = radius_sum;
        result.contact_point = closest_cap;
    }

    return result;
}

CollisionResult test_collision(
    const CollisionShape& a, glm::vec3 pos_a,
    const CollisionShape& b, glm::vec3 pos_b)
{
    // Dispatch based on shape types
    if (a.type == ShapeType::Sphere) {
        const auto& sphere_a = static_cast<const SphereShape&>(a);
        switch (b.type) {
            case ShapeType::Sphere:
                return test_sphere_sphere(sphere_a, pos_a,
                    static_cast<const SphereShape&>(b), pos_b);
            case ShapeType::AABB:
                return test_sphere_aabb(sphere_a, pos_a,
                    static_cast<const AABBShape&>(b), pos_b);
            case ShapeType::Capsule: {
                // Reverse the test and flip the normal
                auto result = test_capsule_sphere(
                    static_cast<const CapsuleShape&>(b), pos_b, sphere_a, pos_a);
                result.normal = -result.normal;
                return result;
            }
        }
    } else if (a.type == ShapeType::AABB) {
        const auto& aabb_a = static_cast<const AABBShape&>(a);
        switch (b.type) {
            case ShapeType::Sphere: {
                auto result = test_sphere_aabb(
                    static_cast<const SphereShape&>(b), pos_b, aabb_a, pos_a);
                result.normal = -result.normal;
                return result;
            }
            case ShapeType::AABB:
                return test_aabb_aabb(aabb_a, pos_a,
                    static_cast<const AABBShape&>(b), pos_b);
            case ShapeType::Capsule: {
                auto result = test_capsule_aabb(
                    static_cast<const CapsuleShape&>(b), pos_b, aabb_a, pos_a);
                result.normal = -result.normal;
                return result;
            }
        }
    } else if (a.type == ShapeType::Capsule) {
        const auto& capsule_a = static_cast<const CapsuleShape&>(a);
        switch (b.type) {
            case ShapeType::Sphere:
                return test_capsule_sphere(capsule_a, pos_a,
                    static_cast<const SphereShape&>(b), pos_b);
            case ShapeType::AABB:
                return test_capsule_aabb(capsule_a, pos_a,
                    static_cast<const AABBShape&>(b), pos_b);
            case ShapeType::Capsule:
                return test_capsule_capsule(capsule_a, pos_a,
                    static_cast<const CapsuleShape&>(b), pos_b);
        }
    }

    return CollisionResult{};  // Unknown shape types
}

}  // namespace engine::physics
