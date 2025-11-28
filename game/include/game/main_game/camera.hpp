#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <engine/input/input_system.hpp>

namespace main_game {
class GameState;

class Camera {
public:
    Camera(float distance = 10.0f, float pitch = 30.0f, float yaw = 0.0f)
        : _distance(distance), _pitch(pitch), _yaw(yaw), _target(0.0f) {}

    void update(GameState& game_state, float delta);

    void process_input(GameState& state) {
        float sensitivity = 0.1f;

        engine::input::InputSystem& input = engine::input::InputSystem::get_instance();
        float dx, dy;
        input.get_mouse_delta(dx, dy);

        _yaw -= dx * sensitivity;
        _pitch += dy * sensitivity;

        // Constrain pitch
        if (_pitch > 89.0f) _pitch = 89.0f;
        if (_pitch < -89.0f) _pitch = -89.0f;
    }

    glm::mat4 view_matrix() const {
        glm::vec3 position = calculate_position();
        return glm::lookAt(position, _target, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 position() const {
        return calculate_position();
    }

    // Get forward direction on XZ plane (for player movement)
    glm::vec3 forward() const {
        float yaw_rad = glm::radians(_yaw);
        return glm::normalize(glm::vec3(sin(yaw_rad), 0.0f, cos(yaw_rad)));
    }

    // Get right direction on XZ plane (for player movement)
    glm::vec3 right() const {
        float yaw_rad = glm::radians(_yaw);
        return glm::normalize(glm::vec3(-cos(yaw_rad), 0.0f, sin(yaw_rad)));
    }

    float yaw() const { return _yaw; }
    float pitch() const { return _pitch; }
    float distance() const { return _distance; }

private:
    glm::vec3 calculate_position() const {
        float pitch_rad = glm::radians(_pitch);
        float yaw_rad = glm::radians(_yaw);

        float x = _target.x - _distance * cos(pitch_rad) * sin(yaw_rad);
        float y = _target.y + _distance * sin(pitch_rad);
        float z = _target.z - _distance * cos(pitch_rad) * cos(yaw_rad);

        return glm::vec3(x, y, z);
    }

    float _distance;
    float _pitch;
    float _yaw;
    glm::vec3 _target;
};

} // namespace main_game
