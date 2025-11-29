#pragma once

#include <engine/render/orbit_camera.hpp>

namespace main_game {
class GameState;

class GameCamera {
public:
    GameCamera();

    void process_input();
    void update(GameState&);

    engine::OrbitCamera& orbit_camera() { return _camera; }
    const engine::OrbitCamera& orbit_camera() const { return _camera; }

    // Forward common camera methods
    glm::vec3 forward_xz() const { return _camera.forward_xz(); }
    glm::vec3 right_xz() const { return _camera.right_xz(); }

private:
    void constrain_pitch_above_ground();

    engine::OrbitCamera _camera;

    // Camera settings
    static constexpr float MIN_CAMERA_HEIGHT = 0.1f;
    static constexpr float MIN_DISTANCE = 0.25f;
    static constexpr float MAX_DISTANCE = 2.0f;
    static constexpr float ZOOM_SENSITIVITY = 0.1f;
    static constexpr float ROTATION_SENSITIVITY = 0.1f;
};
}  // namespace main_game
