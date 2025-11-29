#include <engine/pbr/scene.hpp>

#include <algorithm>

namespace engine::pbr {

void Scene::add_light(const Light& light) {
    if (_lights.size() < MAX_LIGHTS) {
        _lights.push_back(light);
    }
}

void Scene::clear_lights() {
    _lights.clear();
}

void Scene::set_light(size_t index, const Light& light) {
    if (index < _lights.size()) {
        _lights[index] = light;
    }
}

void Scene::remove_light(size_t index) {
    if (index < _lights.size()) {
        _lights.erase(_lights.begin() + static_cast<std::ptrdiff_t>(index));
    }
}

std::vector<const Light*> Scene::get_shadow_casting_lights() const {
    std::vector<const Light*> shadow_lights;
    shadow_lights.reserve(_lights.size());

    for (const auto& light : _lights) {
        if (light.casts_shadow) {
            shadow_lights.push_back(&light);
        }
    }

    // Sort: directional lights first, then by intensity (descending)
    std::sort(shadow_lights.begin(), shadow_lights.end(),
        [](const Light* a, const Light* b) {
            if (a->type == LightType::Directional && b->type != LightType::Directional) {
                return true;
            }
            if (b->type == LightType::Directional && a->type != LightType::Directional) {
                return false;
            }
            return a->intensity > b->intensity;
        });

    return shadow_lights;
}

}  // namespace engine::pbr
