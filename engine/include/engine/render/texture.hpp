#pragma once

#include <glad/glad.h>

#include <string>
#include <cstddef>

namespace engine {

/// GPU-side texture handle
/// Owns OpenGL texture resource, created from image file or memory
class Texture {
public:
    /// Load from file path
    explicit Texture(const std::string& filepath);

    /// Load from memory buffer (e.g., embedded texture in GLB)
    Texture(const unsigned char* data, size_t size);

    ~Texture();

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    GLuint id() const { return _id; }
    int width() const { return _width; }
    int height() const { return _height; }

private:
    GLuint _id = 0;
    int _width = 0;
    int _height = 0;

    void cleanup();
    void setup_texture_params();
};

}  // namespace engine
