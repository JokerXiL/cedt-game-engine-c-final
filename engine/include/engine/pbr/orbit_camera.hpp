#pragma once

#include <engine/pbr/camera.hpp>

#include <cmath>

namespace engine::pbr {

/// Camera that orbits around a target point
class OrbitCamera : public Camera {
public:
    OrbitCamera() { update_from_orbit(); }

    OrbitCamera(const glm::vec3& target, float distance, float pitch = 30.0f, float yaw = 0.0f)
        : _target(target), _distance(distance), _pitch(pitch), _yaw(yaw) {
        update_from_orbit();
    }

    void set_orbit(const glm::vec3& target, float distance, float pitch, float yaw) {
        _target = target;
        _distance = distance;
        _pitch = pitch;
        _yaw = yaw;
        update_from_orbit();
    }

    void set_pitch(float pitch) {
        _pitch = pitch;
        update_from_orbit();
    }

    void orbit_rotate(float delta_yaw, float delta_pitch) {
        _yaw -= delta_yaw;
        _pitch += delta_pitch;

        // Constrain pitch
        if (_pitch > 89.0f) _pitch = 89.0f;
        if (_pitch < -89.0f) _pitch = -89.0f;

        update_from_orbit();
    }

    void set_orbit_target(const glm::vec3& target) {
        _target = target;
        update_from_orbit();
    }

    void set_orbit_distance(float distance) {
        _distance = distance;
        update_from_orbit();
    }

    float yaw() const { return _yaw; }
    float pitch() const { return _pitch; }
    float distance() const { return _distance; }
    const glm::vec3& target() const { return _target; }

    /// Get forward direction on XZ plane (based on orbit yaw)
    glm::vec3 forward_xz() const override {
        float yaw_rad = glm::radians(_yaw);
        return glm::normalize(glm::vec3(std::sin(yaw_rad), 0.0f, std::cos(yaw_rad)));
    }

    /// Get right direction on XZ plane (based on orbit yaw)
    glm::vec3 right_xz() const override {
        float yaw_rad = glm::radians(_yaw);
        return glm::normalize(glm::vec3(-std::cos(yaw_rad), 0.0f, std::sin(yaw_rad)));
    }

private:
    void update_from_orbit() {
        float pitch_rad = glm::radians(_pitch);
        float yaw_rad = glm::radians(_yaw);

        float x = _target.x - _distance * std::cos(pitch_rad) * std::sin(yaw_rad);
        float y = _target.y + _distance * std::sin(pitch_rad);
        float z = _target.z - _distance * std::cos(pitch_rad) * std::cos(yaw_rad);

        _position = glm::vec3(x, y, z);
        look_at(_target);
    }

    glm::vec3 _target{0.0f};
    float _distance{10.0f};
    float _pitch{30.0f};
    float _yaw{0.0f};
};

}  // namespace engine::pbr
