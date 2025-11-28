#include <engine/render/mesh_factory.hpp>

#include <glm/glm.hpp>
#include <cmath>

namespace engine {
namespace mesh_factory {

std::unique_ptr<Mesh> create_cube(float size) {
    float half = size * 0.5f;

    std::vector<Vertex> vertices;

    // Cube vertices with normals (6 faces, 4 vertices each = 24 vertices)
    // We use separate vertices for each face to get correct normals

    // Front face (Z+)
    Vertex v;
    v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v.position = glm::vec3(-half, -half, half); v.uv = glm::vec2(0.0f, 0.0f); vertices.push_back(v);
    v.position = glm::vec3(half, -half, half);  v.uv = glm::vec2(1.0f, 0.0f); vertices.push_back(v);
    v.position = glm::vec3(half, half, half);   v.uv = glm::vec2(1.0f, 1.0f); vertices.push_back(v);
    v.position = glm::vec3(-half, half, half);  v.uv = glm::vec2(0.0f, 1.0f); vertices.push_back(v);

    // Back face (Z-)
    v.normal = glm::vec3(0.0f, 0.0f, -1.0f);
    v.position = glm::vec3(half, -half, -half);  v.uv = glm::vec2(0.0f, 0.0f); vertices.push_back(v);
    v.position = glm::vec3(-half, -half, -half); v.uv = glm::vec2(1.0f, 0.0f); vertices.push_back(v);
    v.position = glm::vec3(-half, half, -half);  v.uv = glm::vec2(1.0f, 1.0f); vertices.push_back(v);
    v.position = glm::vec3(half, half, -half);   v.uv = glm::vec2(0.0f, 1.0f); vertices.push_back(v);

    // Top face (Y+)
    v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    v.position = glm::vec3(-half, half, half);   v.uv = glm::vec2(0.0f, 0.0f); vertices.push_back(v);
    v.position = glm::vec3(half, half, half);    v.uv = glm::vec2(1.0f, 0.0f); vertices.push_back(v);
    v.position = glm::vec3(half, half, -half);   v.uv = glm::vec2(1.0f, 1.0f); vertices.push_back(v);
    v.position = glm::vec3(-half, half, -half);  v.uv = glm::vec2(0.0f, 1.0f); vertices.push_back(v);

    // Bottom face (Y-)
    v.normal = glm::vec3(0.0f, -1.0f, 0.0f);
    v.position = glm::vec3(-half, -half, -half); v.uv = glm::vec2(0.0f, 0.0f); vertices.push_back(v);
    v.position = glm::vec3(half, -half, -half);  v.uv = glm::vec2(1.0f, 0.0f); vertices.push_back(v);
    v.position = glm::vec3(half, -half, half);   v.uv = glm::vec2(1.0f, 1.0f); vertices.push_back(v);
    v.position = glm::vec3(-half, -half, half);  v.uv = glm::vec2(0.0f, 1.0f); vertices.push_back(v);

    // Right face (X+)
    v.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    v.position = glm::vec3(half, -half, half);   v.uv = glm::vec2(0.0f, 0.0f); vertices.push_back(v);
    v.position = glm::vec3(half, -half, -half);  v.uv = glm::vec2(1.0f, 0.0f); vertices.push_back(v);
    v.position = glm::vec3(half, half, -half);   v.uv = glm::vec2(1.0f, 1.0f); vertices.push_back(v);
    v.position = glm::vec3(half, half, half);    v.uv = glm::vec2(0.0f, 1.0f); vertices.push_back(v);

    // Left face (X-)
    v.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    v.position = glm::vec3(-half, -half, -half); v.uv = glm::vec2(0.0f, 0.0f); vertices.push_back(v);
    v.position = glm::vec3(-half, -half, half);  v.uv = glm::vec2(1.0f, 0.0f); vertices.push_back(v);
    v.position = glm::vec3(-half, half, half);   v.uv = glm::vec2(1.0f, 1.0f); vertices.push_back(v);
    v.position = glm::vec3(-half, half, -half);  v.uv = glm::vec2(0.0f, 1.0f); vertices.push_back(v);

    // Indices (6 faces * 2 triangles * 3 vertices = 36 indices)
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < 6; ++i) {
        unsigned int base = i * 4;
        // First triangle
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        // Second triangle
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }

    return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> create_plane(float width, float height) {
    float half_width = width * 0.5f;
    float half_height = height * 0.5f;

    std::vector<Vertex> vertices(4);

    // Create a plane in the XZ plane (Y = 0), facing up (+Y)
    vertices[0].position = glm::vec3(-half_width, 0.0f, -half_height);
    vertices[0].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    vertices[0].uv = glm::vec2(0.0f, 0.0f);

    vertices[1].position = glm::vec3(half_width, 0.0f, -half_height);
    vertices[1].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    vertices[1].uv = glm::vec2(1.0f, 0.0f);

    vertices[2].position = glm::vec3(half_width, 0.0f, half_height);
    vertices[2].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    vertices[2].uv = glm::vec2(1.0f, 1.0f);

    vertices[3].position = glm::vec3(-half_width, 0.0f, half_height);
    vertices[3].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    vertices[3].uv = glm::vec2(0.0f, 1.0f);

    std::vector<unsigned int> indices = {
        0, 1, 2,  // First triangle
        0, 2, 3   // Second triangle
    };

    return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> create_sphere(float radius, int segments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const float PI = 3.14159265359f;

    // Generate vertices
    for (int lat = 0; lat <= segments; ++lat) {
        float theta = lat * PI / segments;
        float sin_theta = std::sin(theta);
        float cos_theta = std::cos(theta);

        for (int lon = 0; lon <= segments; ++lon) {
            float phi = lon * 2.0f * PI / segments;
            float sin_phi = std::sin(phi);
            float cos_phi = std::cos(phi);

            Vertex vertex;
            vertex.position.x = radius * sin_theta * cos_phi;
            vertex.position.y = radius * cos_theta;
            vertex.position.z = radius * sin_theta * sin_phi;

            vertex.normal = glm::normalize(vertex.position);

            vertex.uv.x = (float)lon / segments;
            vertex.uv.y = (float)lat / segments;

            vertices.push_back(vertex);
        }
    }

    // Generate indices
    for (int lat = 0; lat < segments; ++lat) {
        for (int lon = 0; lon < segments; ++lon) {
            unsigned int first = (lat * (segments + 1)) + lon;
            unsigned int second = first + segments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    return std::make_unique<Mesh>(vertices, indices);
}

}  // namespace mesh_factory
}  // namespace engine
