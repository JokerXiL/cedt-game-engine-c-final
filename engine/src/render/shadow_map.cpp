#include <engine/render/shadow_map.hpp>

#include <utility>

namespace engine {

// ============================================================================
// ShadowMap Implementation
// ============================================================================

ShadowMap::ShadowMap() = default;

ShadowMap::~ShadowMap() {
    cleanup();
}

ShadowMap::ShadowMap(ShadowMap&& other) noexcept
    : _fbo(other._fbo)
    , _depth_texture(other._depth_texture)
    , light_space_matrix(other.light_space_matrix)
    , _initialized(other._initialized) {
    other._fbo = 0;
    other._depth_texture = 0;
    other._initialized = false;
}

ShadowMap& ShadowMap::operator=(ShadowMap&& other) noexcept {
    if (this != &other) {
        cleanup();
        _fbo = other._fbo;
        _depth_texture = other._depth_texture;
        light_space_matrix = other.light_space_matrix;
        _initialized = other._initialized;
        other._fbo = 0;
        other._depth_texture = 0;
        other._initialized = false;
    }
    return *this;
}

void ShadowMap::initialize() {
    if (_initialized) return;

    // Create depth texture
    glGenTextures(1, &_depth_texture);
    glBindTexture(GL_TEXTURE_2D, _depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    // Create framebuffer
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth_texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _initialized = true;
}

void ShadowMap::bind_for_writing() {
    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::cleanup() {
    if (_depth_texture) {
        glDeleteTextures(1, &_depth_texture);
        _depth_texture = 0;
    }
    if (_fbo) {
        glDeleteFramebuffers(1, &_fbo);
        _fbo = 0;
    }
    _initialized = false;
}

// ============================================================================
// ShadowCubeMap Implementation
// ============================================================================

ShadowCubeMap::ShadowCubeMap() = default;

ShadowCubeMap::~ShadowCubeMap() {
    cleanup();
}

ShadowCubeMap::ShadowCubeMap(ShadowCubeMap&& other) noexcept
    : _fbo(other._fbo)
    , _depth_cubemap(other._depth_cubemap)
    , light_space_matrices(other.light_space_matrices)
    , far_plane(other.far_plane)
    , _initialized(other._initialized) {
    other._fbo = 0;
    other._depth_cubemap = 0;
    other._initialized = false;
}

ShadowCubeMap& ShadowCubeMap::operator=(ShadowCubeMap&& other) noexcept {
    if (this != &other) {
        cleanup();
        _fbo = other._fbo;
        _depth_cubemap = other._depth_cubemap;
        light_space_matrices = other.light_space_matrices;
        far_plane = other.far_plane;
        _initialized = other._initialized;
        other._fbo = 0;
        other._depth_cubemap = 0;
        other._initialized = false;
    }
    return *this;
}

void ShadowCubeMap::initialize() {
    if (_initialized) return;

    // Create depth cubemap
    glGenTextures(1, &_depth_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _depth_cubemap);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Create framebuffer
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth_cubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _initialized = true;
}

void ShadowCubeMap::bind_for_writing(int face) {
    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, _depth_cubemap, 0);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowCubeMap::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowCubeMap::cleanup() {
    if (_depth_cubemap) {
        glDeleteTextures(1, &_depth_cubemap);
        _depth_cubemap = 0;
    }
    if (_fbo) {
        glDeleteFramebuffers(1, &_fbo);
        _fbo = 0;
    }
    _initialized = false;
}

// ============================================================================
// ShadowSystem Implementation
// ============================================================================

ShadowSystem& ShadowSystem::get_instance() {
    static ShadowSystem instance;
    return instance;
}

void ShadowSystem::initialize() {
    if (_initialized) return;

    for (auto& shadow_map : _shadow_maps) {
        shadow_map.initialize();
    }
    for (auto& cubemap : _shadow_cubemaps) {
        cubemap.initialize();
    }

    _initialized = true;
}

void ShadowSystem::shutdown() {
    // Resources are cleaned up by destructors
    _initialized = false;
}

ShadowMap* ShadowSystem::get_shadow_map(int index) {
    if (index >= 0 && index < MAX_SHADOW_MAPS) {
        return &_shadow_maps[static_cast<size_t>(index)];
    }
    return nullptr;
}

ShadowCubeMap* ShadowSystem::get_shadow_cubemap(int index) {
    if (index >= 0 && index < MAX_SHADOW_CUBEMAPS) {
        return &_shadow_cubemaps[static_cast<size_t>(index)];
    }
    return nullptr;
}

void ShadowSystem::bind_shadow_textures(int start_texture_unit) {
    // Bind 2D shadow maps
    for (int i = 0; i < MAX_SHADOW_MAPS; ++i) {
        glActiveTexture(GL_TEXTURE0 + start_texture_unit + i);
        glBindTexture(GL_TEXTURE_2D, _shadow_maps[static_cast<size_t>(i)].depth_texture());
    }

    // Bind cubemap shadow maps
    int cubemap_start = start_texture_unit + MAX_SHADOW_MAPS;
    for (int i = 0; i < MAX_SHADOW_CUBEMAPS; ++i) {
        glActiveTexture(GL_TEXTURE0 + cubemap_start + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _shadow_cubemaps[static_cast<size_t>(i)].depth_cubemap());
    }

    // Reset to texture unit 0
    glActiveTexture(GL_TEXTURE0);
}

}  // namespace engine
