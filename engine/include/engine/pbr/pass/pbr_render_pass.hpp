#pragma once

#include <engine/render/render_graph.hpp>
#include <engine/pbr/scene.hpp>

#include <glm/glm.hpp>

#include <vector>

namespace engine::pbr {

/// Resource slot identifiers for PBR passes
namespace RenderResource {
    constexpr const char* ShadowMaps = "shadow_maps";
    constexpr const char* ColorBuffer = "color_buffer";
    constexpr const char* DepthBuffer = "depth_buffer";
}

// Forward declarations
class Model;
class Mesh;
class StandardMaterial;
class Skeleton;

/// Renderable object for PBR passes
struct Renderable {
    Model* model = nullptr;
    Mesh* mesh = nullptr;
    StandardMaterial* material = nullptr;
    glm::mat4 transform{1.0f};
    bool casts_shadow = true;
    const Skeleton* skeleton = nullptr;
    glm::vec3 albedo_override{-1.0f};  // If >= 0, overrides material albedo
};

/// Context shared between PBR passes (shadow, main)
struct PBRContext {
    Scene* scene = nullptr;
    std::vector<Renderable> renderables;
    ShadowData shadow_data;  // Populated by ShadowPass, read by materials via Scene

    void clear() {
        renderables.clear();
    }

    void submit(Model& model, const glm::mat4& transform, bool casts_shadow = true) {
        renderables.push_back({&model, nullptr, nullptr, transform, casts_shadow, nullptr});
    }

    void submit_skinned(Model& model, const glm::mat4& transform,
                        const Skeleton& skeleton, bool casts_shadow = true) {
        renderables.push_back({&model, nullptr, nullptr, transform, casts_shadow, &skeleton});
    }

    void submit(Mesh& mesh, StandardMaterial& material,
                const glm::mat4& transform, bool casts_shadow = true) {
        renderables.push_back({nullptr, &mesh, &material, transform, casts_shadow, nullptr, glm::vec3(-1.0f)});
    }

    void submit(Mesh& mesh, StandardMaterial& material,
                const glm::mat4& transform, const glm::vec3& albedo_override, bool casts_shadow = true) {
        renderables.push_back({nullptr, &mesh, &material, transform, casts_shadow, nullptr, albedo_override});
    }

    void submit_skinned(Mesh& mesh, StandardMaterial& material,
                        const glm::mat4& transform, const Skeleton& skeleton,
                        bool casts_shadow = true) {
        renderables.push_back({nullptr, &mesh, &material, transform, casts_shadow, &skeleton});
    }
};

/// Base class for PBR render passes that share a common context
class PBRRenderPass : public RenderPass {
public:
    explicit PBRRenderPass(std::string name) : RenderPass(std::move(name)) {}

    /// Set the shared PBR context
    void set_context(PBRContext* ctx) { _pbr_context = ctx; }

protected:
    PBRContext* _pbr_context = nullptr;
};

}  // namespace engine::pbr
