#pragma once

#include <engine/pbr/material.hpp>
#include <engine/pbr/mesh.hpp>
#include <engine/pbr/skeleton.hpp>
#include <engine/pbr/shader_cache.hpp>

#include <glm/glm.hpp>

namespace engine::pbr {

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

    // Constructor - requires ShaderCache for shader management
    StandardMaterial(
        ShaderCache& shader_cache,
        const glm::vec3& albedo = glm::vec3(1.0f),
        const glm::vec3& specular = glm::vec3(1.0f),
        float metallic = 0.0f,
        float roughness = 0.5f
    );

    // Destructor
    ~StandardMaterial() override = default;

    /// Render a static mesh using Scene for camera and lighting
    void render(const Mesh& mesh, const glm::mat4& transform, const Scene& scene) override;

    /// Render a skinned mesh using Scene for camera and lighting
    void render_skinned(const Mesh& mesh, const glm::mat4& transform,
                        const Skeleton& skeleton, const Scene& scene) override;

    /// Render to shadow map (depth only, no lighting)
    void render_shadow(const Mesh& mesh, const glm::mat4& transform,
                       const glm::mat4& light_space_matrix) override;

    /// Render skinned mesh to shadow map
    void render_shadow_skinned(const Mesh& mesh, const glm::mat4& transform,
                               const Skeleton& skeleton,
                               const glm::mat4& light_space_matrix) override;

    /// Render to point light shadow cubemap (single face, multi-pass approach)
    void render_shadow_cube(const Mesh& mesh, const glm::mat4& transform,
                            const glm::mat4& light_space_matrix,
                            const glm::vec3& light_pos, float far_plane) override;

    /// Render skinned mesh to point light shadow cubemap
    void render_shadow_cube_skinned(const Mesh& mesh, const glm::mat4& transform,
                                    const Skeleton& skeleton,
                                    const glm::mat4& light_space_matrix,
                                    const glm::vec3& light_pos, float far_plane) override;

private:
    std::shared_ptr<Shader> _shader;
    std::shared_ptr<Shader> _shadow_shader;

    void set_common_uniforms(const glm::mat4& model, const Scene& scene);
    void set_light_uniforms(const Scene& scene);
    void set_shadow_uniforms(const Scene& scene);
    void set_bone_transforms(const Skeleton* skeleton, const Shader& shader);
    void bind_textures();
    void draw_mesh(const Mesh& mesh);
};

}  // namespace engine::pbr
