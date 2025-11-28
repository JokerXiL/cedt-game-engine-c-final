#include <engine/render/mesh.hpp>

namespace engine {

Mesh::Mesh(const MeshData& data) {
    _vertex_count = data.vertex_count();
    _index_count = data.indices.size();

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    // Position (layout 0) - always required
    glGenBuffers(1, &_vbo_positions);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_positions);
    glBufferData(GL_ARRAY_BUFFER, data.positions.size() * sizeof(glm::vec3), data.positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

    // Normal (layout 1)
    if (data.has_normals()) {
        glGenBuffers(1, &_vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, data.normals.size() * sizeof(glm::vec3), data.normals.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    }

    // UV (layout 2)
    if (data.has_uvs()) {
        glGenBuffers(1, &_vbo_uvs);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_uvs);
        glBufferData(GL_ARRAY_BUFFER, data.uvs.size() * sizeof(glm::vec2), data.uvs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
    }

    // Color (layout 3)
    if (data.has_colors()) {
        glGenBuffers(1, &_vbo_colors);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_colors);
        glBufferData(GL_ARRAY_BUFFER, data.colors.size() * sizeof(glm::vec4), data.colors.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
    }

    // Joint weights (layout 4)
    if (data.has_skinning()) {
        glGenBuffers(1, &_vbo_joint_weights);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_joint_weights);
        glBufferData(GL_ARRAY_BUFFER, data.joint_weights.size() * sizeof(glm::vec4), data.joint_weights.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);

        // Joint indices (layout 5)
        glGenBuffers(1, &_vbo_joint_indices);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_joint_indices);
        glBufferData(GL_ARRAY_BUFFER, data.joint_indices.size() * sizeof(glm::ivec4), data.joint_indices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(glm::ivec4), nullptr);
    }

    // Indices
    if (data.has_indices() > 0) {
        glGenBuffers(1, &_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    cleanup();
}

Mesh::Mesh(Mesh&& other) noexcept
    : _vao(other._vao),
      _vbo_positions(other._vbo_positions),
      _vbo_normals(other._vbo_normals),
      _vbo_uvs(other._vbo_uvs),
      _vbo_colors(other._vbo_colors),
      _vbo_joint_weights(other._vbo_joint_weights),
      _vbo_joint_indices(other._vbo_joint_indices),
      _ebo(other._ebo),
      _vertex_count(other._vertex_count),
      _index_count(other._index_count) {
    other._vao = 0;
    other._vbo_positions = 0;
    other._vbo_normals = 0;
    other._vbo_uvs = 0;
    other._vbo_colors = 0;
    other._vbo_joint_weights = 0;
    other._vbo_joint_indices = 0;
    other._ebo = 0;
    other._vertex_count = 0;
    other._index_count = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        cleanup();

        _vao = other._vao;
        _vbo_positions = other._vbo_positions;
        _vbo_normals = other._vbo_normals;
        _vbo_uvs = other._vbo_uvs;
        _vbo_colors = other._vbo_colors;
        _vbo_joint_weights = other._vbo_joint_weights;
        _vbo_joint_indices = other._vbo_joint_indices;
        _ebo = other._ebo;
        _vertex_count = other._vertex_count;
        _index_count = other._index_count;

        other._vao = 0;
        other._vbo_positions = 0;
        other._vbo_normals = 0;
        other._vbo_uvs = 0;
        other._vbo_colors = 0;
        other._vbo_joint_weights = 0;
        other._vbo_joint_indices = 0;
        other._ebo = 0;
        other._vertex_count = 0;
        other._index_count = 0;
    }
    return *this;
}

void Mesh::cleanup() {
    if (_vao != 0) {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }
    if (_vbo_positions != 0) {
        glDeleteBuffers(1, &_vbo_positions);
        _vbo_positions = 0;
    }
    if (_vbo_normals != 0) {
        glDeleteBuffers(1, &_vbo_normals);
        _vbo_normals = 0;
    }
    if (_vbo_uvs != 0) {
        glDeleteBuffers(1, &_vbo_uvs);
        _vbo_uvs = 0;
    }
    if (_vbo_colors != 0) {
        glDeleteBuffers(1, &_vbo_colors);
        _vbo_colors = 0;
    }
    if (_vbo_joint_weights != 0) {
        glDeleteBuffers(1, &_vbo_joint_weights);
        _vbo_joint_weights = 0;
    }
    if (_vbo_joint_indices != 0) {
        glDeleteBuffers(1, &_vbo_joint_indices);
        _vbo_joint_indices = 0;
    }
    if (_ebo != 0) {
        glDeleteBuffers(1, &_ebo);
        _ebo = 0;
    }
}

}  // namespace engine
