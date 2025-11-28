#include <engine/render/material.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

namespace engine {

Material::~Material() {
    // Clean up shader program
    if (_shader_program_id != 0) {
        glDeleteProgram(_shader_program_id);
    }
}

GLuint Material::compile_shader(const char* shader_path, GLenum shader_type) {
    std::string shader_code;
    std::ifstream shader_file;

    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        shader_file.open(shader_path);
        std::stringstream shader_stream;
        shader_stream << shader_file.rdbuf();
        shader_file.close();
        shader_code = shader_stream.str();
    } catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << shader_path << " - " << e.what() << std::endl;
        return 0;
    }

    const char* shader_code_cstr = shader_code.c_str();

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_code_cstr, NULL);
    glCompileShader(shader);

    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED: " << shader_path << "\n" << info_log << std::endl;
    }

    return shader;
}

GLuint Material::link_program(GLuint vertex_shader, GLuint fragment_shader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
    }

    return program;
}

GLuint Material::create_shader_program(const char* vertex_path, const char* fragment_path) {
    GLuint vertex_shader = compile_shader(vertex_path, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment_path, GL_FRAGMENT_SHADER);

    GLuint program = link_program(vertex_shader, fragment_shader);

    // Clean up shaders after linking
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

void Material::use_program(GLuint program_id) {
    glUseProgram(program_id);
}

void Material::set_bool(GLuint program_id, const std::string& name, bool value) {
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), (int)value);
}

void Material::set_int(GLuint program_id, const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), value);
}

void Material::set_float(GLuint program_id, const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(program_id, name.c_str()), value);
}

void Material::set_vec3(GLuint program_id, const std::string& name, const glm::vec3& value) {
    glUniform3fv(glGetUniformLocation(program_id, name.c_str()), 1, glm::value_ptr(value));
}

void Material::set_vec3(GLuint program_id, const std::string& name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(program_id, name.c_str()), x, y, z);
}

void Material::set_vec4(GLuint program_id, const std::string& name, const glm::vec4& value) {
    glUniform4fv(glGetUniformLocation(program_id, name.c_str()), 1, glm::value_ptr(value));
}

void Material::set_mat4(GLuint program_id, const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

}  // namespace engine
