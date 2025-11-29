#pragma once

#include <engine/pbr/pass/pbr_render_pass.hpp>

namespace engine::pbr {

class ShadowPass;

/// Main PBR rendering pass
/// Reads: shadow_maps
/// Writes: color_buffer, depth_buffer
class PBRPass : public PBRRenderPass {
public:
    PBRPass();

    void execute() override;

    /// Set reference to shadow pass for accessing shadow textures
    void set_shadow_pass(ShadowPass* shadow_pass) { _shadow_pass = shadow_pass; }

private:
    ShadowPass* _shadow_pass = nullptr;
};

}  // namespace engine::pbr
