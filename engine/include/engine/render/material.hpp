#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

namespace engine {

class Mesh;
class Model;
class Skeleton;

class Material {
public:
    virtual ~Material();

    virtual void render(const Mesh& mesh, const glm::mat4& transform) = 0;
    virtual void render_skinned(const Mesh& mesh, const glm::mat4& transform, const Skeleton& skeleton) = 0;

protected:
    // Shader program ID - managed internally by each material
    GLuint _shader_program_id = 0;

    // Shader compilation and linking utilities
    static GLuint compile_shader(const char* shader_path, GLenum shader_type);
    static GLuint link_program(GLuint vertex_shader, GLuint fragment_shader);
    static GLuint create_shader_program(const char* vertex_path, const char* fragment_path);

    static void use_program(GLuint program_id);
    static void set_bool(GLuint program_id, const std::string& name, bool value);
    static void set_int(GLuint program_id, const std::string& name, int value);
    static void set_float(GLuint program_id, const std::string& name, float value);
    static void set_vec3(GLuint program_id, const std::string& name, const glm::vec3& value);
    static void set_vec3(GLuint program_id, const std::string& name, float x, float y, float z);
    static void set_vec4(GLuint program_id, const std::string& name, const glm::vec4& value);
    static void set_mat4(GLuint program_id, const std::string& name, const glm::mat4& mat);
};

}  // namespace engine
