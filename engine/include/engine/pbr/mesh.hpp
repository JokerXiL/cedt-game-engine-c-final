#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

namespace engine::pbr {

/// CPU-side mesh data (Structure of Arrays layout)
/// Used for loading/generating mesh data before uploading to GPU
struct MeshData {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec4> joint_weights;
    std::vector<glm::ivec4> joint_indices;
    std::vector<unsigned int> indices;

    size_t vertex_count() const { return positions.size(); }
    bool has_normals() const { return !normals.empty(); }
    bool has_uvs() const { return !uvs.empty(); }
    bool has_colors() const { return !colors.empty(); }
    bool has_skinning() const { return !joint_weights.empty(); }
    bool has_indices() const { return !indices.empty(); }
};

/// GPU-side mesh handle
/// Owns OpenGL VAO/VBO resources, created from MeshData
class Mesh {
public:
    // Creates and uploads mesh to GPU
    explicit Mesh(const MeshData& data);

    // Cleans up OpenGL resources
    ~Mesh();

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    // Delete copy constructor and assignment (OpenGL resources shouldn't be copied)
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    GLuint vao() const { return _vao; }
    size_t vertex_count() const { return _vertex_count; }
    size_t index_count() const { return _index_count; }

private:
    GLuint _vao = 0;
    GLuint _vbo_positions = 0;
    GLuint _vbo_normals = 0;
    GLuint _vbo_uvs = 0;
    GLuint _vbo_colors = 0;
    GLuint _vbo_joint_weights = 0;
    GLuint _vbo_joint_indices = 0;
    GLuint _ebo = 0;

    size_t _vertex_count = 0;
    size_t _index_count = 0;
    
    void cleanup();
};

}  // namespace engine::pbr
