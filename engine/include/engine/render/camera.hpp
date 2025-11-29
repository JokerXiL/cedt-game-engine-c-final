#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace engine {

/// Base 3D camera with transform (position + orientation)
class Camera {
public:
    Camera() = default;
    virtual ~Camera() = default;

    Camera(const glm::vec3& position, const glm::quat& orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
        : _position(position), _orientation(orientation) {}

    void set_projection(const glm::mat4& projection) { _projection = projection; }
    void set_position(const glm::vec3& position) { _position = position; }
    void set_orientation(const glm::quat& orientation) { _orientation = orientation; }

    void set_euler(float pitch, float yaw, float roll = 0.0f) {
        _orientation = glm::quat(glm::vec3(pitch, yaw, roll));
    }

    void look_at(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)) {
        glm::mat4 look = glm::lookAt(_position, target, up);
        _orientation = glm::quat_cast(glm::inverse(look));
    }

    const glm::mat4& projection() const { return _projection; }
    const glm::vec3& position() const { return _position; }
    const glm::quat& orientation() const { return _orientation; }

    glm::mat4 view() const {
        glm::mat4 rotation = glm::mat4_cast(glm::conjugate(_orientation));
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), -_position);
        return rotation * translation;
    }

    /// Get forward direction (negative Z in camera space)
    glm::vec3 forward() const {
        return _orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    /// Get right direction (positive X in camera space)
    glm::vec3 right() const {
        return _orientation * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    /// Get up direction (positive Y in camera space)
    glm::vec3 up() const {
        return _orientation * glm::vec3(0.0f, 1.0f, 0.0f);
    }

    /// Get forward direction on XZ plane (for movement)
    virtual glm::vec3 forward_xz() const {
        glm::vec3 fwd = forward();
        return glm::normalize(glm::vec3(fwd.x, 0.0f, fwd.z));
    }

    /// Get right direction on XZ plane (for movement)
    virtual glm::vec3 right_xz() const {
        glm::vec3 r = right();
        return glm::normalize(glm::vec3(r.x, 0.0f, r.z));
    }

protected:
    glm::mat4 _projection{1.0f};
    glm::vec3 _position{0.0f, 0.0f, 0.0f};
    glm::quat _orientation{1.0f, 0.0f, 0.0f, 0.0f};
};

}  // namespace engine
