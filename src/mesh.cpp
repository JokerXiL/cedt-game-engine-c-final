#include "mesh.hpp"

#include <iostream>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    setup_mesh(vertices, indices);
}

Mesh::~Mesh() {
    cleanup();
}

Mesh::Mesh(Mesh&& other) noexcept
    : _vao(other._vao),
      _vbo(other._vbo),
      _ebo(other._ebo),
      _vertex_count(other._vertex_count),
      _index_count(other._index_count) {
    // Take ownership of OpenGL resources
    other._vao = 0;
    other._vbo = 0;
    other._ebo = 0;
    other._vertex_count = 0;
    other._index_count = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        // Clean up existing resources
        cleanup();

        // Take ownership of other's resources
        _vao = other._vao;
        _vbo = other._vbo;
        _ebo = other._ebo;
        _vertex_count = other._vertex_count;
        _index_count = other._index_count;

        other._vao = 0;
        other._vbo = 0;
        other._ebo = 0;
        other._vertex_count = 0;
        other._index_count = 0;
    }
    return *this;
}

void Mesh::setup_mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    _vertex_count = vertices.size();
    _index_count = indices.size();

    // Generate and bind VAO
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Upload index data if provided
    if (!indices.empty()) {
        glGenBuffers(1, &_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    // Vertex attribute pointers
    // Layout location 0: position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    // Layout location 1: normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // Layout location 2: uv
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    // Layout location 3: color
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    // Layout location 4: joint_weights
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, joint_weights));

    // Layout location 5: joint_indices
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, joint_indices));

    // Unbind
    glBindVertexArray(0);
}

void Mesh::cleanup() {
    if (_vao != 0) {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }
    if (_vbo != 0) {
        glDeleteBuffers(1, &_vbo);
        _vbo = 0;
    }
    if (_ebo != 0) {
        glDeleteBuffers(1, &_ebo);
        _ebo = 0;
    }
}

void Mesh::bind() const {
    glBindVertexArray(_vao);
}

void Mesh::unbind() const {
    glBindVertexArray(0);
}

void Mesh::draw() const {
    bind();
    if (_index_count > 0) {
        glDrawElements(GL_TRIANGLES, _index_count, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, _vertex_count);
    }
    unbind();
}
