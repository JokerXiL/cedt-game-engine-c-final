#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

namespace engine {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec4 color;           // Optional, defaults to white
    glm::vec4 joint_weights;   // For skinned meshes, defaults to zero
    glm::ivec4 joint_indices;  // For skinned meshes, defaults to zero

    Vertex()
        : position(0.0f),
          normal(0.0f, 1.0f, 0.0f),
          uv(0.0f),
          color(1.0f),
          joint_weights(0.0f),
          joint_indices(0) {}
};

class Mesh {
public:
    // Constructor - creates and uploads mesh to GPU
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices = {});

    // Destructor - cleans up OpenGL resources
    ~Mesh();

    // Delete copy constructor and assignment (OpenGL resources shouldn't be copied)
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Move constructor and assignment
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    // Bind the mesh for rendering
    void bind() const;

    // Unbind the mesh
    void unbind() const;

    // Draw the mesh (assumes already bound and shader is active)
    void draw() const;

    // Getters
    size_t get_vertex_count() const { return _vertex_count; }
    size_t get_index_count() const { return _index_count; }
    bool has_indices() const { return _index_count > 0; }

private:
    GLuint _vao = 0;
    GLuint _vbo = 0;
    GLuint _ebo = 0;

    size_t _vertex_count = 0;
    size_t _index_count = 0;

    void setup_mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void cleanup();
};

}  // namespace engine
