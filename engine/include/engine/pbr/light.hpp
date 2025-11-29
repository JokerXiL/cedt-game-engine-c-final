#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace engine::pbr {

/// Maximum number of lights supported in forward rendering
constexpr int MAX_LIGHTS = 8;

/// Maximum number of shadow-casting directional/spot lights
constexpr int MAX_SHADOW_MAPS = 4;

/// Maximum number of shadow-casting point lights (cubemaps)
constexpr int MAX_SHADOW_CUBEMAPS = 2;

/// Light type enumeration
enum class LightType : int {
    Directional = 0,  // Sun-like, parallel rays, no attenuation
    Point = 1,        // Omnidirectional, attenuates with distance
    Spot = 2          // Cone-shaped, with inner/outer cutoff angles
};

/// Light properties for all light types
struct Light {
    LightType type = LightType::Point;

    // Position (used by Point/Spot) or Direction TO the light (used by Directional)
    glm::vec3 position{0.0f, 10.0f, 0.0f};

    // Direction the light points (for Spot lights, and stored for Directional)
    glm::vec3 direction{0.0f, -1.0f, 0.0f};

    // Light color and intensity
    glm::vec3 color{1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;

    // Attenuation (Point/Spot only): 1 / (constant + linear*d + quadratic*d^2)
    float attenuation_constant = 1.0f;
    float attenuation_linear = 0.09f;
    float attenuation_quadratic = 0.032f;

    // Spot light cone angles (in radians)
    float inner_cutoff = glm::radians(12.5f);  // Full intensity inside this angle
    float outer_cutoff = glm::radians(17.5f);  // Fade to zero at this angle

    // Shadow mapping
    bool casts_shadow = false;
    int shadow_map_index = -1;  // Index into shadow map array (-1 = no shadow)

    /// Create a directional light (sun-like)
    /// direction - Direction the light points (will be normalized)
    /// color - Light color
    /// intensity - Light intensity multiplier
    static Light create_directional(
        const glm::vec3& direction,
        const glm::vec3& color = glm::vec3(1.0f),
        float intensity = 1.0f
    );

    /// Create a point light (omnidirectional)
    /// position - World position of the light
    /// color - Light color
    /// intensity - Light intensity multiplier
    /// range - Distance at which light reaches ~1% intensity
    static Light create_point(
        const glm::vec3& position,
        const glm::vec3& color = glm::vec3(1.0f),
        float intensity = 1.0f,
        float range = 50.0f
    );

    /// Create a spot light (cone-shaped)
    /// position - World position of the light
    /// direction - Direction the light points (will be normalized)
    /// inner_angle_degrees - Full intensity cone angle
    /// outer_angle_degrees - Outer fade cone angle
    /// color - Light color
    /// intensity - Light intensity multiplier
    /// range - Distance at which light reaches ~1% intensity
    static Light create_spot(
        const glm::vec3& position,
        const glm::vec3& direction,
        float inner_angle_degrees,
        float outer_angle_degrees,
        const glm::vec3& color = glm::vec3(1.0f),
        float intensity = 1.0f,
        float range = 50.0f
    );
};

}  // namespace engine::pbr
