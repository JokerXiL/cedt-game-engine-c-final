#pragma once

#include <engine/pbr/camera.hpp>
#include <engine/pbr/light.hpp>

#include <glm/glm.hpp>

#include <array>
#include <vector>

namespace engine::pbr {

/// Shadow data for shader uniforms
struct ShadowData {
    std::array<glm::mat4, MAX_SHADOW_MAPS> light_space_matrices;
    std::array<float, MAX_SHADOW_CUBEMAPS> point_light_far_planes;
    int shadow_map_texture_unit = 5;
    int shadow_cubemap_texture_unit = 5 + MAX_SHADOW_MAPS;
};

/// Scene container holding camera and lights for rendering
class Scene {
public:
    Scene() = default;

    // Camera access
    void set_camera(const Camera* camera) { _camera = camera; }
    const Camera* camera() const { return _camera; }

    // Shadow data access (set by ShadowPass before PBR rendering)
    void set_shadow_data(const ShadowData* data) { _shadow_data = data; }
    const ShadowData* shadow_data() const { return _shadow_data; }

    // Light management
    void add_light(const Light& light);
    void clear_lights();
    void set_light(size_t index, const Light& light);
    void remove_light(size_t index);
    Light& light(size_t index) { return _lights[index]; }
    const Light& light(size_t index) const { return _lights[index]; }

    const std::vector<Light>& lights() const { return _lights; }
    size_t light_count() const { return _lights.size(); }

    // Ambient light (global illumination approximation)
    void set_ambient(const glm::vec3& color) { _ambient_color = color; }
    const glm::vec3& ambient() const { return _ambient_color; }

    // Helper to get shadow-casting lights sorted by priority
    // (directional lights first, then by intensity)
    std::vector<const Light*> get_shadow_casting_lights() const;

private:
    const Camera* _camera = nullptr;
    const ShadowData* _shadow_data = nullptr;
    std::vector<Light> _lights;
    glm::vec3 _ambient_color{0.1f, 0.1f, 0.1f};
};

}  // namespace engine::pbr
