#pragma once

#include <engine/render/material.hpp>
#include <engine/render/mesh.hpp>
#include <engine/render/skeleton.hpp>

#include <glm/glm.hpp>

namespace engine {

// Forward declarations
class Scene;

class StandardMaterial : public Material {
public:
    // PBR Material properties
    glm::vec3 albedo;
    glm::vec3 specular_color;
    float metallic;
    float roughness;

    // Texture IDs (0 = no texture)
    GLuint albedo_map;
    GLuint specular_map;
    GLuint metallic_roughness_map;
    GLuint normal_map;

    // Constructor
    StandardMaterial(
        const glm::vec3& albedo = glm::vec3(1.0f),
        const glm::vec3& specular = glm::vec3(1.0f),
        float metallic = 0.0f,
        float roughness = 0.5f
    );

    // Destructor
    ~StandardMaterial() override = default;

    // ========================================================================
    // New Scene-based render methods (preferred)
    // ========================================================================

    /// Render a static mesh using Scene for camera and lighting
    void render(const Mesh& mesh, const glm::mat4& transform, const Scene& scene);

    /// Render a skinned mesh using Scene for camera and lighting
    void render_skinned(const Mesh& mesh, const glm::mat4& transform,
                        const Skeleton& skeleton, const Scene& scene);

    /// Render to shadow map (depth only, no lighting)
    void render_shadow(const Mesh& mesh, const glm::mat4& transform,
                       const glm::mat4& light_space_matrix);

    /// Render skinned mesh to shadow map
    void render_shadow_skinned(const Mesh& mesh, const glm::mat4& transform,
                               const Skeleton& skeleton,
                               const glm::mat4& light_space_matrix);

    /// Render to point light shadow cubemap (single face, multi-pass approach)
    void render_shadow_cube(const Mesh& mesh, const glm::mat4& transform,
                            const glm::mat4& light_space_matrix,
                            const glm::vec3& light_pos, float far_plane);

    /// Render skinned mesh to point light shadow cubemap
    void render_shadow_cube_skinned(const Mesh& mesh, const glm::mat4& transform,
                                    const Skeleton& skeleton,
                                    const glm::mat4& light_space_matrix,
                                    const glm::vec3& light_pos, float far_plane);

    // ========================================================================
    // Legacy render methods (for backwards compatibility)
    // ========================================================================

    // Legacy lighting parameters (used by legacy render methods)
    glm::vec3 light_pos;
    glm::vec3 light_color;
    glm::vec3 view_pos;
    glm::mat4 projection;
    glm::mat4 view;

    /// Legacy render - uses member variables for camera/lighting
    void render(const Mesh& mesh, const glm::mat4& transform) override;

    /// Legacy skinned render - uses member variables for camera/lighting
    void render_skinned(const Mesh& mesh, const glm::mat4& transform,
                        const Skeleton& skeleton) override;

    // ========================================================================
    // Shadow shader access (for ShadowSystem)
    // ========================================================================

    /// Get the shadow shader program ID
    static GLuint shadow_program_id() { return _shadow_program_id; }

    /// Get the point light shadow shader program ID (with geometry shader)
    static GLuint shadow_cube_program_id() { return _shadow_cube_program_id; }

    /// Initialize shadow shaders (called once after GL context is ready)
    static void initialize_shadow_shaders();

private:
    // Shadow shader programs (shared across all materials)
    static GLuint _shadow_program_id;
    static GLuint _shadow_cube_program_id;
    static bool _shadow_shaders_initialized;

    void set_common_uniforms(const glm::mat4& model, const Scene& scene);
    void set_common_uniforms_legacy(const glm::mat4& model);
    void set_light_uniforms(const Scene& scene);
    void set_shadow_uniforms(const Scene& scene);
    void set_bone_transforms(const Skeleton* skeleton);
    void set_bone_transforms(const Skeleton* skeleton, GLuint program_id);
    void bind_textures();
    void draw_mesh(const Mesh& mesh);
};

}  // namespace engine
