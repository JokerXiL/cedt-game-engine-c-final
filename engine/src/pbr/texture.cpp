#include <engine/pbr/texture.hpp>

#include <SOIL2/SOIL2.h>

#include <stdexcept>

namespace engine::pbr {

Texture::Texture(const std::string& filepath) {
    _id = SOIL_load_OGL_texture(
        filepath.c_str(),
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );

    if (_id == 0) {
        throw std::runtime_error("Failed to load texture: " + filepath);
    }

    // Get dimensions
    glBindTexture(GL_TEXTURE_2D, _id);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_height);

    setup_texture_params();
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const unsigned char* data, size_t size) {
    // Don't invert Y for embedded textures (glTF/GLB already has correct orientation)
    _id = SOIL_load_OGL_texture_from_memory(
        data,
        static_cast<int>(size),
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS
    );

    if (_id == 0) {
        throw std::runtime_error("Failed to load texture from memory");
    }

    // Get dimensions
    glBindTexture(GL_TEXTURE_2D, _id);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_height);

    setup_texture_params();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setup_texture_params() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::~Texture() {
    cleanup();
}

Texture::Texture(Texture&& other) noexcept
    : _id(other._id),
      _width(other._width),
      _height(other._height) {
    other._id = 0;
    other._width = 0;
    other._height = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        cleanup();

        _id = other._id;
        _width = other._width;
        _height = other._height;

        other._id = 0;
        other._width = 0;
        other._height = 0;
    }
    return *this;
}

void Texture::cleanup() {
    if (_id != 0) {
        glDeleteTextures(1, &_id);
        _id = 0;
    }
}

}  // namespace engine::pbr
