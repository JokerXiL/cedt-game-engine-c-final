#pragma once

#include <engine/render/material.hpp>
#include <engine/render/mesh.hpp>
#include <engine/render/skeleton.hpp>

#include <glm/glm.hpp>

namespace engine {

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

    // Lighting parameters (set these before rendering)
    glm::vec3 light_pos;
    glm::vec3 light_color;
    glm::vec3 view_pos;

    // Projection and view matrices (set these before rendering)
    glm::mat4 projection;
    glm::mat4 view;

    // Constructor
    StandardMaterial(
        const glm::vec3& albedo = glm::vec3(1.0f),
        const glm::vec3& specular = glm::vec3(1.0f),
        float metallic = 0.0f,
        float roughness = 0.5f
    );

    // Destructor (textures should be managed externally)
    ~StandardMaterial() override = default;

    // Render methods
    void render(const Mesh& mesh, const glm::mat4& transform) override;
    void render_skinned(const Mesh& mesh, const glm::mat4& transform, const Skeleton& skeleton) override;

private:
    void set_common_uniforms(const glm::mat4& model);
    void set_bone_transforms(const Skeleton* skeleton);
    void bind_textures();
};

}  // namespace engine
