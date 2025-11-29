#pragma once

#include <engine/render/camera.hpp>
#include <engine/render/light.hpp>

#include <glm/glm.hpp>

#include <vector>

namespace engine {

/// Scene container holding camera and lights for rendering
class Scene {
public:
    Scene() = default;

    // Camera access
    void set_camera(const Camera* camera) { _camera = camera; }
    const Camera* camera() const { return _camera; }

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
    std::vector<Light> _lights;
    glm::vec3 _ambient_color{0.1f, 0.1f, 0.1f};
};

}  // namespace engine
