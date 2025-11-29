#pragma once

#include <engine/pbr/pass/pbr_render_pass.hpp>
#include <engine/pbr/light.hpp>
#include <engine/pbr/camera.hpp>
#include <engine/pbr/shadow_map.hpp>

#include <glm/glm.hpp>
#include <array>

namespace engine::pbr {

/// Shadow map rendering pass
/// Owns and manages all shadow map resources
class ShadowPass : public PBRRenderPass {
public:
    ShadowPass();

    void execute() override;

    /// Get shadow map for a directional/spot light by index
    ShadowMap* get_shadow_map(int index);

    /// Get cubemap for a point light by index
    ShadowCubeMap* get_shadow_cubemap(int index);

    /// Bind all shadow textures for sampling in main render pass
    void bind_shadow_textures(int start_texture_unit = 5);

    /// Get the texture unit where shadow maps start
    int shadow_map_texture_unit() const { return 5; }

    /// Get the texture unit where shadow cubemaps start
    int shadow_cubemap_texture_unit() const { return 5 + MAX_SHADOW_MAPS; }

private:
    void render_directional_shadow(const Light& light, const Camera& camera);
    void render_spot_shadow(const Light& light);
    void render_point_shadow(const Light& light);

    glm::mat4 calculate_directional_light_matrix(const Light& light, const Camera& camera);
    glm::mat4 calculate_spot_light_matrix(const Light& light);
    std::array<glm::mat4, 6> calculate_point_light_matrices(const Light& light, float far_plane);

    std::array<ShadowMap, MAX_SHADOW_MAPS> _shadow_maps;
    std::array<ShadowCubeMap, MAX_SHADOW_CUBEMAPS> _shadow_cubemaps;
    bool _initialized = false;
};

}  // namespace engine::pbr
