#pragma once

#include <engine/pbr/orbit_camera.hpp>

namespace main_game {
class GameState;

class GameCamera {
public:
    GameCamera();

    void process_input();
    void update(GameState&, float delta);

    engine::pbr::OrbitCamera& orbit_camera() { return _camera; }
    const engine::pbr::OrbitCamera& orbit_camera() const { return _camera; }

    // Forward common camera methods
    glm::vec3 forward_xz() const { return _camera.forward_xz(); }
    glm::vec3 right_xz() const { return _camera.right_xz(); }

    // Get aim direction from player to crosshair target (raycast through screen center)
    glm::vec3 get_aim_direction(const glm::vec3& player_position) const;

    // Swap which shoulder the camera is over
    void swap_shoulder() { _shoulder_side_target = -_shoulder_side_target; }

    // Toggle aim mode
    void toggle_aim() { _is_aiming = !_is_aiming; }
    bool is_aiming() const { return _is_aiming; }

private:
    void clamp_camera_height();

    engine::pbr::OrbitCamera _camera;

    // Aim mode
    bool _is_aiming = false;
    float _aim_blend = 0.0f;  // 0 = normal, 1 = aiming

    // Camera settings
    static constexpr float MIN_CAMERA_HEIGHT = 0.1f;
    static constexpr float ROTATION_SENSITIVITY = 0.1f;

    // Over-the-shoulder settings (always active)
    static constexpr float AIM_SHOULDER_OFFSET = 0.4f;   // Horizontal offset magnitude
    static constexpr float SHOULDER_BLEND_SPEED = 15.0f;  // How fast to swap shoulders
    float _shoulder_side_target = 1.0f;  // 1.0 = right shoulder, -1.0 = left shoulder
    float _shoulder_side_current = 1.0f; // Current interpolated value

    // Distance settings
    static constexpr float NORMAL_DISTANCE = 2.0f;       // Default camera distance
    static constexpr float AIM_DISTANCE = 1.0f;          // Zoomed in distance when aiming
    static constexpr float AIM_BLEND_SPEED = 10.0f;      // How fast to zoom transition
};
}  // namespace main_game
