#pragma once

#include "key_codes.hpp"

namespace engine {
namespace input {

class InputSystem {
public:
    // Singleton instance
    static InputSystem& get_instance();

    void init();

    // Key state
    bool is_key_pressed(KeyCode key) const {
        int k = static_cast<int>(key);
        if (k >= 0 && k < 1024) return _keys[k];
        return false;
    }

    bool is_key_just_pressed(KeyCode key) const {
        int k = static_cast<int>(key);
        if (k >= 0 && k < 1024) return _keys_just_pressed[k];
        return false;
    }

    // Call at end of frame to reset just-pressed states
    void end_frame();

    // Mouse state
    double mouse_x() const { return _mouse_x; }
    double mouse_y() const { return _mouse_y; }
    double last_mouse_x() const { return _last_mouse_x; }
    double last_mouse_y() const { return _last_mouse_y; }

    // Mouse capture
    void capture_mouse();
    void release_mouse();

    // Get mouse delta and reset for next frame
    void get_mouse_delta(float& dx, float& dy);

private:
    InputSystem() = default;
    ~InputSystem() = default;
    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;

    void set_key_state(int key, bool pressed) {
        if (key >= 0 && key < 1024) {
            if (pressed && !_keys[key]) {
                _keys_just_pressed[key] = true;
            }
            _keys[key] = pressed;
        }
    }

    void update_mouse_position(double xpos, double ypos) {
        _last_mouse_x = _mouse_x;
        _last_mouse_y = _mouse_y;
        _mouse_x = xpos;
        _mouse_y = ypos;
    }

    bool _keys[1024] = {false};
    bool _keys_just_pressed[1024] = {false};
    double _mouse_x = 0.0;
    double _mouse_y = 0.0;
    double _last_mouse_x = 0.0;
    double _last_mouse_y = 0.0;
};

}  // namespace input
}  // namespace engine
