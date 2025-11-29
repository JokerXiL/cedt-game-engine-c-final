#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

namespace engine::pbr {

/// GPU-side shader program handle
/// Owns OpenGL shader program resource, created from vertex/fragment shader files
class Shader {
public:
    /// Load from vertex and fragment shader file paths
    Shader(const std::string& vertex_path, const std::string& fragment_path);

    ~Shader();

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    /// Get the OpenGL program ID
    GLuint id() const { return _id; }

    /// Check if shader compiled and linked successfully
    bool valid() const { return _id != 0; }

    /// Activate this shader program
    void use() const;

    // Uniform setters
    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, float value) const;
    void set_vec3(const std::string& name, const glm::vec3& value) const;
    void set_vec3(const std::string& name, float x, float y, float z) const;
    void set_vec4(const std::string& name, const glm::vec4& value) const;
    void set_mat4(const std::string& name, const glm::mat4& value) const;

private:
    GLuint _id = 0;

    void cleanup();

    static GLuint compile_shader(const std::string& path, GLenum type);
    static GLuint link_program(GLuint vertex_shader, GLuint fragment_shader);
};

}  // namespace engine::pbr
