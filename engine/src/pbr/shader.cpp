#include <engine/pbr/shader.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

namespace engine::pbr {

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path) {
    GLuint vertex_shader = compile_shader(vertex_path, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment_path, GL_FRAGMENT_SHADER);

    if (vertex_shader != 0 && fragment_shader != 0) {
        _id = link_program(vertex_shader, fragment_shader);
    }

    // Clean up shaders after linking (they're no longer needed)
    if (vertex_shader != 0) glDeleteShader(vertex_shader);
    if (fragment_shader != 0) glDeleteShader(fragment_shader);
}

Shader::~Shader() {
    cleanup();
}

Shader::Shader(Shader&& other) noexcept : _id(other._id) {
    other._id = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        cleanup();
        _id = other._id;
        other._id = 0;
    }
    return *this;
}

void Shader::cleanup() {
    if (_id != 0) {
        glDeleteProgram(_id);
        _id = 0;
    }
}

void Shader::use() const {
    glUseProgram(_id);
}

void Shader::set_bool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(_id, name.c_str()), static_cast<int>(value));
}

void Shader::set_int(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
}

void Shader::set_float(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
}

void Shader::set_vec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(_id, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::set_vec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(_id, name.c_str()), x, y, z);
}

void Shader::set_vec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(_id, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::set_mat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

GLuint Shader::compile_shader(const std::string& path, GLenum type) {
    std::string code;
    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        code = stream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_READ: " << path << " - " << e.what() << std::endl;
        return 0;
    }

    const char* code_cstr = code.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &code_cstr, nullptr);
    glCompileShader(shader);

    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED: " << path << "\n" << info_log << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint Shader::link_program(GLuint vertex_shader, GLuint fragment_shader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, info_log);
        std::cerr << "ERROR::SHADER::LINKING_FAILED\n" << info_log << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

}  // namespace engine::pbr
