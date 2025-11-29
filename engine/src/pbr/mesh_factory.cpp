#include <engine/pbr/mesh_factory.hpp>

#include <glm/glm.hpp>
#include <cmath>

namespace engine::pbr {
namespace mesh_factory {

std::unique_ptr<Mesh> create_cube(float size) {
    float half = size * 0.5f;

    MeshData data;

    // Helper to add a face (4 vertices)
    auto add_face = [&](const glm::vec3& normal,
                        const glm::vec3& p0, const glm::vec3& p1,
                        const glm::vec3& p2, const glm::vec3& p3) {
        data.positions.push_back(p0);
        data.positions.push_back(p1);
        data.positions.push_back(p2);
        data.positions.push_back(p3);

        data.normals.push_back(normal);
        data.normals.push_back(normal);
        data.normals.push_back(normal);
        data.normals.push_back(normal);

        data.uvs.push_back(glm::vec2(0.0f, 0.0f));
        data.uvs.push_back(glm::vec2(1.0f, 0.0f));
        data.uvs.push_back(glm::vec2(1.0f, 1.0f));
        data.uvs.push_back(glm::vec2(0.0f, 1.0f));

        // Default white vertex color
        data.colors.push_back(glm::vec4(1.0f));
        data.colors.push_back(glm::vec4(1.0f));
        data.colors.push_back(glm::vec4(1.0f));
        data.colors.push_back(glm::vec4(1.0f));
    };

    // Front face (Z+)
    add_face(glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec3(-half, -half, half), glm::vec3(half, -half, half),
             glm::vec3(half, half, half), glm::vec3(-half, half, half));

    // Back face (Z-)
    add_face(glm::vec3(0.0f, 0.0f, -1.0f),
             glm::vec3(half, -half, -half), glm::vec3(-half, -half, -half),
             glm::vec3(-half, half, -half), glm::vec3(half, half, -half));

    // Top face (Y+)
    add_face(glm::vec3(0.0f, 1.0f, 0.0f),
             glm::vec3(-half, half, half), glm::vec3(half, half, half),
             glm::vec3(half, half, -half), glm::vec3(-half, half, -half));

    // Bottom face (Y-)
    add_face(glm::vec3(0.0f, -1.0f, 0.0f),
             glm::vec3(-half, -half, -half), glm::vec3(half, -half, -half),
             glm::vec3(half, -half, half), glm::vec3(-half, -half, half));

    // Right face (X+)
    add_face(glm::vec3(1.0f, 0.0f, 0.0f),
             glm::vec3(half, -half, half), glm::vec3(half, -half, -half),
             glm::vec3(half, half, -half), glm::vec3(half, half, half));

    // Left face (X-)
    add_face(glm::vec3(-1.0f, 0.0f, 0.0f),
             glm::vec3(-half, -half, -half), glm::vec3(-half, -half, half),
             glm::vec3(-half, half, half), glm::vec3(-half, half, -half));

    // Indices (6 faces * 2 triangles * 3 vertices = 36 indices)
    for (unsigned int i = 0; i < 6; ++i) {
        unsigned int base = i * 4;
        data.indices.push_back(base + 0);
        data.indices.push_back(base + 1);
        data.indices.push_back(base + 2);
        data.indices.push_back(base + 0);
        data.indices.push_back(base + 2);
        data.indices.push_back(base + 3);
    }

    return std::make_unique<Mesh>(data);
}

std::unique_ptr<Mesh> create_plane(float width, float height) {
    float half_width = width * 0.5f;
    float half_height = height * 0.5f;

    MeshData data;

    // Create a plane in the XZ plane (Y = 0), facing up (+Y)
    data.positions = {
        glm::vec3(-half_width, 0.0f, -half_height),
        glm::vec3(half_width, 0.0f, -half_height),
        glm::vec3(half_width, 0.0f, half_height),
        glm::vec3(-half_width, 0.0f, half_height)
    };

    data.normals = {
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    };

    data.uvs = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f)
    };

    data.colors = {
        glm::vec4(1.0f),
        glm::vec4(1.0f),
        glm::vec4(1.0f),
        glm::vec4(1.0f)
    };

    data.indices = {0, 1, 2, 0, 2, 3};

    return std::make_unique<Mesh>(data);
}

std::unique_ptr<Mesh> create_sphere(float radius, int segments) {
    MeshData data;

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

            glm::vec3 position;
            position.x = radius * sin_theta * cos_phi;
            position.y = radius * cos_theta;
            position.z = radius * sin_theta * sin_phi;

            data.positions.push_back(position);
            data.normals.push_back(glm::normalize(position));
            data.uvs.push_back(glm::vec2(static_cast<float>(lon) / segments,
                                          static_cast<float>(lat) / segments));
            data.colors.push_back(glm::vec4(1.0f));
        }
    }

    // Generate indices
    for (int lat = 0; lat < segments; ++lat) {
        for (int lon = 0; lon < segments; ++lon) {
            unsigned int first = (lat * (segments + 1)) + lon;
            unsigned int second = first + segments + 1;

            data.indices.push_back(first);
            data.indices.push_back(second);
            data.indices.push_back(first + 1);

            data.indices.push_back(second);
            data.indices.push_back(second + 1);
            data.indices.push_back(first + 1);
        }
    }

    return std::make_unique<Mesh>(data);
}

}  // namespace mesh_factory
}  // namespace engine::pbr
