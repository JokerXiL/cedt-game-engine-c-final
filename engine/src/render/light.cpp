#include <engine/render/light.hpp>

namespace engine {

Light Light::create_directional(
    const glm::vec3& direction,
    const glm::vec3& color,
    float intensity
) {
    Light light;
    light.type = LightType::Directional;
    light.direction = glm::normalize(direction);
    // For directional lights, position stores the direction TO the light (inverse)
    light.position = -light.direction;
    light.color = color;
    light.intensity = intensity;
    // Directional lights have no attenuation
    light.attenuation_constant = 1.0f;
    light.attenuation_linear = 0.0f;
    light.attenuation_quadratic = 0.0f;
    return light;
}

Light Light::create_point(
    const glm::vec3& position,
    const glm::vec3& color,
    float intensity,
    float range
) {
    Light light;
    light.type = LightType::Point;
    light.position = position;
    light.direction = glm::vec3(0.0f, -1.0f, 0.0f);  // Not used for point lights
    light.color = color;
    light.intensity = intensity;

    // Calculate attenuation from range
    // Light reaches ~1% intensity at `range` distance
    // Using formula from Ogre3D wiki for realistic falloff
    light.attenuation_constant = 1.0f;
    light.attenuation_linear = 4.5f / range;
    light.attenuation_quadratic = 75.0f / (range * range);

    return light;
}

Light Light::create_spot(
    const glm::vec3& position,
    const glm::vec3& direction,
    float inner_angle_degrees,
    float outer_angle_degrees,
    const glm::vec3& color,
    float intensity,
    float range
) {
    Light light;
    light.type = LightType::Spot;
    light.position = position;
    light.direction = glm::normalize(direction);
    light.color = color;
    light.intensity = intensity;
    light.inner_cutoff = glm::radians(inner_angle_degrees);
    light.outer_cutoff = glm::radians(outer_angle_degrees);

    // Calculate attenuation from range (same as point light)
    light.attenuation_constant = 1.0f;
    light.attenuation_linear = 4.5f / range;
    light.attenuation_quadratic = 75.0f / (range * range);

    return light;
}

}  // namespace engine
